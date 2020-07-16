/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ s e t a s i d e . c                               **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "pft.h"

void mixsoil(Stand *stand1,Stand *stand2)
{
  int l;

  mixpool(stand1->soil.cpool.slow,stand2->soil.cpool.slow,stand1->frac,stand2->frac);
  mixpool(stand1->soil.cpool.fast,stand2->soil.cpool.fast,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.ag_tree,stand2->soil.litter.ag_tree,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.ag_grass,stand2->soil.litter.ag_grass,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.bg,stand2->soil.litter.bg,stand1->frac,stand2->frac);
  //new litter scheme
  mixpool(stand1->soil.litter.ag_tree_inc,stand2->soil.litter.ag_tree_inc,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.ag_grass_inc,stand2->soil.litter.ag_grass_inc,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.bg_inc,stand2->soil.litter.bg_inc,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.ag_tree_pinc,stand2->soil.litter.ag_tree_pinc,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.ag_grass_pinc,stand2->soil.litter.ag_grass_pinc,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.bg_pinc,stand2->soil.litter.bg_pinc,stand1->frac,stand2->frac);
  
  mixpool(stand1->soil.meanw1,stand2->soil.meanw1,stand1->frac,stand2->frac);
  mixpool(stand1->soil.k_mean.slow,stand2->soil.k_mean.slow,stand1->frac,stand2->frac);
  mixpool(stand1->soil.k_mean.fast,stand2->soil.k_mean.fast,stand1->frac,stand2->frac);
  mixpool(stand1->soil.decomp_litter_mean,stand2->soil.decomp_litter_mean,stand1->frac,stand2->frac);
 
  /* snowpack is independent of fraction */
  /*mixpool(stand1->soil.snowpack,stand2->soil.snowpack,stand1->frac,stand2->frac);*/
  foreachsoillayer(l)
    mixpool(stand1->soil.w[l],stand2->soil.w[l],stand1->frac,stand2->frac);
  mixpool(stand1->soil.w_evap,stand2->soil.w_evap,stand1->frac,stand2->frac);
  /*stand1->soil.w_evap=stand1->soil.w[0]*soildepth[0]/SOILDEPTH_EVAP;*/
  mixpool(stand1->soil.alag,stand2->soil.alag,stand1->frac,stand2->frac);
  mixpool(stand1->soil.amp,stand2->soil.amp,stand1->frac,stand2->frac);
}

void mixsetaside(Standlist standlist,int s,int standid,Bool intercrop)
{
  /*assumes that all vegetation carbon pool are zero after harvest*/
  int p;
  Pft *pft;
  Stand *setasidestand,*new_cutstand;
  
  setasidestand=getstand(standlist,s);
  new_cutstand=getstand(standlist,standid);
  
  mixsoil(setasidestand,new_cutstand);

  if(intercrop){
    foreachpft(pft,p,setasidestand->pftlist)
      mix_veg(pft,setasidestand->frac/(setasidestand->frac+new_cutstand->frac));
  }
  
  setasidestand->frac+=new_cutstand->frac;
  delstand(standlist,standid);
}

void setaside(Cell *cell,int *standid,const Pftpar pftpar[],Bool intercrop,int npft, Bool allow_establish)
{
  int s,q,p;
  Pft *pft;
  Stand *stand;
  Real flux_estab=0.0;
  
  if(existlandusetype(cell->standlist,&s,SETASIDE)){
    //if there already exists another SETASIDE stand, then mix and delete one
    //..this is deprecated in gross luc -- no real functionality in current lpj version 26 may 2020
    mixsetaside(cell->standlist,s,*standid,intercrop);
    *standid-=1;
    return;
  }
  else{
    stand=getstand(cell->standlist,*standid);
    stand->landusetype=SETASIDE;
    stand->irrigation=FALSE;
    stand->pot_irrig_amount=stand->irrig_amount=0.0;
    stand->pftlist=newpftlist();

    if(allow_establish){
        if(intercrop){
          for(p=0;p<npft-1;p++){
            if(establish(cell->gdd[p],pftpar+p,&cell->climbuf, 0) && pftpar[p].type==GRASS) 
            addpft(stand->pftlist,pftpar+p);
          }
          foreachpft(pft,q,stand->pftlist)
            flux_estab+=establishment(pft,0,0,1);
        }
        cell->output.flux_estab+=flux_estab*stand->frac;
    }
    return;
  }
} /* of 'setaside' */

/*
- called in landusechange(), update_daily()
- checks if a set-aside stand already exists
  -> if TRUE: call of local function mixsetaside()
  -> if FALSE: set stand variable landusetype to SETASIDE
         if intercropping is possible:
      -> check if pft of type GRASS could be establish (see 
         establish.c)
         -> if TRUE: add pft to the pftlist and call specific 
         function establishment() (see pft.h)

mixsetaside()
 
- mixes certain variables of structure soil of the set-aside stand with the 
  considered crop stand
  -> calls local function mixpool() (see landuse.h)
- if already intercropping on set-aside stand reduce the leafs & roots dependent 
  on the fraction of the crop stand 
  -> calls specific function mix_veg() (see pft.h,mix_veg_grass.c,mix_veg_tree.c)
- adds the land fractions of the crop stand to the set-aside stand
- deletes considered crop stand (see delstand() in standlist.c)
*/
