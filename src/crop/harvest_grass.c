/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ h a r v e s t _ g r a s s . c                      **/
/*******************************************************************/
#include "lpj.h"

Real harvest_grass(Stand *stand,
                   Pft *pft,
                   Real *rharvest)
{
  Real harvest=0.0;
  int p;

  foreachpft(pft,p,stand->pftlist){
    //set to 0.0 (from 0.5) to not harvest the grass
    //..also, last parameter set to T because this function called during update_daily
    harvest+=litter_update(&stand->soil.litter,pft,0.0,rharvest,TRUE,TRUE);
    pft->gdd=pft->phen=0.0;
  }
  return harvest;
}

/*
- called in function update_daily() when managed grassland is harvested
*/
