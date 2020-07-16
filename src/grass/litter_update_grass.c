/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ l i t t e r _ u p d a t e _  g r a s s . c        **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

Real litter_update_grass(Litter *litter,Pft *pft,Real frac,Real *rharvest,Bool isharvest, Bool subannual)
{
  Real harvest=0;
  Pftgrass *grass;
  grass=pft->data;
  
  if(!isharvest){
    if(subannual){
      litter->ag_grass+=grass->ind.leaf*frac;
      litter->bg+=grass->ind.root*frac;
    }else{
      litter->ag_grass_inc+=grass->ind.leaf*frac;
      litter->bg_inc+=grass->ind.root*frac;
    }
  }else{
    harvest=grass->ind.leaf*frac;
    grass->ind.leaf*=(1.0-frac);
  }

  *rharvest=0;
  return harvest;
} /* of 'litter_update_grass' */
