/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ l i t t e r _ u p d a t e _ c r o p . c            **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

Real litter_update_crop(Litter *litter, Pft *pft,Real frac,Real *rharvest, Bool isharvest, Bool subannual)
{
  Real aflux=0;
  Pftcrop *crop;
  
  crop=pft->data;
  if(subannual){  //subannual then litter goes into litter pool,otherwize into increment pool
    if(RESIDUES_OUT){
      litter->ag_grass+=(crop->ind.leaf+crop->ind.pool)*0.1;
      *rharvest=(crop->ind.leaf+crop->ind.pool)*0.9;
    }else {
      litter->ag_grass+=crop->ind.leaf+crop->ind.pool;
      *rharvest=0;
    }

    aflux=crop->ind.so;
    litter->bg+=crop->ind.root;
  }else{
    if(RESIDUES_OUT){
      litter->ag_grass_inc+=(crop->ind.leaf+crop->ind.pool)*0.1;
      *rharvest=(crop->ind.leaf+crop->ind.pool)*0.9;
    }else {
      litter->ag_grass_inc+=crop->ind.leaf+crop->ind.pool;
      *rharvest=0;
    }

    aflux=crop->ind.so;
    litter->bg_inc+=crop->ind.root;
  }

#ifdef DEBUG3
  printf("%s ag_grass=%.2f bg=%.2f aflux=%.2f rharvest=%.2f ind.so=%.2f ind.leaf=%.2f ind.pool=%.2f ind.root=%.2f pft->bm_inc_%.2f\n",
         pft->par->name,litter->ag_grass,litter->bg,aflux,*rharvest,crop->ind.so,crop->ind.leaf,crop->ind.pool,crop->ind.root,pft->bm_inc);
#endif
  return aflux;
} /* of 'litter_update_crop' */

/*
- specific function called in update_daily()
- sending carbon to the litter pools
  -> differentiation between two cases: residues will be taken out or not
     -> defined RESIDUES_OUT in crop.h
     -> calculation of leaf litter dependent on RESIDUES_OUT
  -> the litter of the leaves goes to the litter pool ag_grass 
  -> updating of litter pool root (litter->bg)
- returns the harvested carbon fluxes 
*/
