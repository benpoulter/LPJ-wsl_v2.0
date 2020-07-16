/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ p h e n o l o g y _ c r o p . c                    **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

Bool phenology_crop(Pft *pft,
                    int npft,
                    Real temp,
                    Real lat,
                    Real country_laimax[],
                    int day,
                    Real daylength,
                    Bool irrigation,
                    int landusetype,
                    Real mtemp, 
                    Real swrad //default assume daily swrad=2*par
                  )
{
  Pftcrop *crop;
  const Pftcroppar *par;
  Real hu,k,c;
  Real vrf,prf;
  Real lai0,lai00;
  Real lai_inc=0;
  Real laimax;
  int hdate;
  int hlimit;
  int pft_id;

  //NOTE: FORKEL_PHENOLOGY not setup to work with crops
  crop=pft->data;
  par=pft->par->data;
  pft_id=pft->par->id;

  hdate=-1;
  crop->growingdays++;

  hlimit=(crop->wtyp && par->calcmethod_sdate==TEMP_WTYP_CALC_SDATE) ? par->hlimit+90 : par->hlimit; 

  crop->senescence0=crop->senescence;

  laimax=country_laimax[pft_id-npft];

  if(crop->husum<crop->phu){
    hu=max(0,temp-crop->basetemp);
    /* vernalization reduction factor still has no effects if temp>trg */
    if(temp<par->trg && crop->vdsum<crop->pvd){
      crop->vdsum+=1;
      vrf=min(1.0,crop->vdsum/crop->pvd);
      hu*=vrf;
    }
    /* response to photoperiodic */
    prf=(1-par->psens)*min(1,max(0,(daylength-par->pb)/(par->ps-par->pb)))+par->psens;
    hu*=prf;
    /* daily effective temperature sum (deg Cd) */
    crop->husum+=hu;
    /* fraction of growing season */
    crop->fphu=min(1.0,crop->husum/crop->phu);
    if(crop->fphu<par->fphusen){
      k=(log(par->fphuc/par->flaimaxc-par->fphuc)-log(par->fphuk/par->flaimaxk-par->fphuk))
        /(par->fphuk-par->fphuc);
      c=log(par->fphuc/par->flaimaxc-par->fphuc)+k*par->fphuc;
      crop->flaimax=crop->fphu/(crop->fphu+exp(c-k*crop->fphu));
    }else{
      crop->senescence=TRUE;
      crop->flaimax=pow(1-crop->fphu,par->shapesenescencenorm)
        /pow(1-par->fphusen,par->shapesenescencenorm)
        *(1-par->flaimaxharvest)+par->flaimaxharvest;
    }
  }else{
    hdate=day;
  }

  if(crop->growingdays==hlimit) hdate=day;

  /* determination of lai */
  lai0=crop->lai;

  if(crop->senescence /*&& !irrigation*/){
    if(!crop->senescence0) crop->laimax_adjusted=lai0;
    crop->lai=crop->flaimax*crop->laimax_adjusted;
  }else{
    crop->lai=crop->flaimax*laimax;
  }

  lai00=crop->lai000; 
  crop->lai000=crop->lai;

  if(!crop->senescence /*&& !irrigation*/){
    lai_inc=(crop->lai-lai00)*pft->wscal;
    crop->lai=lai_inc+lai0;
  }

  pft->phen=crop->lai/laimax;

  if(day==hdate)
    return FALSE;   

  return TRUE;
} /* of 'phenology_crop' */

/*
- the function is called for each pft in update_daily():
  -> leaf_phenology(pft,temp,cell->coord.lat,day,daylength,
<                     stand->irrigation,stand->landusetype);
- initialisation of local variables
- daily computation of the phenological scalar between 0 at sowing and 1 at 
  maturity (harvest)
  -> calculation of hu (heat unit)
  -> calculation of vrf (vernalization reduction factor) dependent on pvd
     (vernalization days) to reduce hu (only for winter crops)
  -> calculation of prf (photoperiodism reduction factor) to reduce hu 
     (still inactive, yet)
  -> calculation of fphu (fraction of phenological heat unit)
  -> calculation of flaimax (fraction of maximum LAI) dependent of fphu
     -> two different shapes: - development and growth (increase)
            - senescence (decrease)
  -> harvest if phenological scalar reaches 1 or growingdays (number of days for
     the growing period; see hlimit in pft.par) are reached
  -> determination of lai (leaf area index) and pft->phen

- returns FALSE on hdate, so that the landusetype will be set to KILL, the litter
  pools will be updated and the stand will be go to the set-aside stand 
  (see update_daily.c,setaside.c)
*/
