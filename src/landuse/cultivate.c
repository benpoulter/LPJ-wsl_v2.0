/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ c u l t i v a t e . c                           **/
/*******************************************************************/
#include "lpj.h"

Real cultivate (Standlist standlist, 
    const Pftpar *pftpar, 
    int vern_date20, 
    Real *landfrac,
    int id, 
    Bool irrigation, 
    Real lat, 
    int day, 
    Bool wtyp,
    int s)
{
  int p;
  int pos; /*position of new stand in list*/
  Real rharvest;
  Real dummy=-9;
  Pft *pft;
  Stand *setasidestand;
  Stand *cropstand;
  Pftcrop *crop;
  Pftcroppar *croppar;

                
  setasidestand=getstand(standlist,s);

  if(*landfrac>=setasidestand->frac-0.00001){
    setasidestand->landusetype=AGRICULTURE;
    foreachpft(pft,p,setasidestand->pftlist){
      litter_update(&setasidestand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
      delpft(setasidestand->pftlist,p);
      p--;
    }
    addpft(setasidestand->pftlist,pftpar+id);
    pft=getpft(setasidestand->pftlist,0);
    crop=pft->data;
    croppar=pft->par->data;
    phen_variety(crop,croppar,vern_date20,lat,day,wtyp);
    crop->wtyp=wtyp;
    setasidestand->irrigation=irrigation;
    setasidestand->count=0;
    return pft->bm_inc*setasidestand->frac;
  }else{
    pos=addstand(standlist);
    cropstand=getstand(standlist,pos-1);
    cropstand->landusetype=AGRICULTURE;
    cropstand->irrigation=irrigation;
    cropstand->count=0;
    cropstand->frac=*landfrac;
    reclaim_land(setasidestand,cropstand,&dummy,&dummy);
    addpft(cropstand->pftlist,pftpar+id);
    pft=getpft(cropstand->pftlist,0);
    crop=pft->data;
    croppar=pft->par->data;
    phen_variety(crop,croppar,vern_date20,lat,day,wtyp);
    crop->wtyp=wtyp;
    setasidestand->frac-=*landfrac;
    return pft->bm_inc*cropstand->frac;
  }
}

/*
- called in sowing()
- comparison of the land fraction (landfrac) of the considered cft with the 
  fraction of the set-aside stand (setasidestand->frac)
  -> is the land fraction of the cft greater or equal as the fraction of the
     set-aside stand:
  -> sets the landusetype of the set-aside stand to AGRICULTURE 
     (defined in stand.h)
  -> kills all pfts of the set-aside stand and updates the litter pools
  -> adds considered cft to the pftlist of the stand (see addpft() in 
     pftlist.h)
     (-> addpft() calls function newpft() (see newpft.c);
      -> newpft() calls specific functions (here new_crop.c, see below)) 
  -> creates a variable crop of type Pftcrop with the informations of the 
     crop-specific variables of the new cft (see getpft() in pftlist.h) 
     with the aim to change informations
  -> calls function phen_variety() (see below)
  -> sets wtyp to TRUE or FALSE (this information comes from function sowing())
  -> sets irrigation to TRUE or FALSE (this information comes from function
     sowing())

  -> is the land fraction of the cft smaller as the fraction of the set-aside 
     stand
     -> adds a new stand to the standlist (see addstand() in standlist.c)
     -> addstand() returns the length of the standlist which is also the
        position of the new stand in the standlist
     -> creates a variable cropstand of type Stand with the informations of 
        the new stand (see getstand() in stand.h) with the aim to change 
        informations
     -> calls function reclaim_land()
     -> adds considered cft to the pftlist of the stand (see addpft() in 
        pftlist.h)
     -> creates a variable crop of type Pftcrop with the informations of the
        crop-specific variables of the new cft (see getpft() in pftlist.h) 
        with the aim to change informations
     -> calls function phen_variety() (see below)
     -> sets wtyp to TRUE or FALSE (this information comes from function 
        sowing())
     -> sets the landusetype of the new cropstand to AGRICULTURE 
     -> sets irrigation to TRUE or FALSE (this information comes from function
        sowing())
     -> sets the frac of the new cropstand to the landfrac
     -> subtracts the frac of the set-aside stand with the landfrac
*/
