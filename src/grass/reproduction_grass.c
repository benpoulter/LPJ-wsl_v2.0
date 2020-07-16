/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ r e p r o d u c t i o n _ g r a s s . c           **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

#define reprod_cost 0.1

void reproduction_grass(Litter *litter,Pft *pft)
{
  Real reprod;
  if(pft->bm_inc>0){
    reprod=pft->bm_inc*reprod_cost;
    litter->ag_grass_inc+=reprod;
    pft->bm_inc-=reprod;
  }
} /* of 'reproduction_grass' */
