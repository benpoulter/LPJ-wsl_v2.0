/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ m i x _ v e g _ g r a s s . c                     **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

void mix_veg_grass(Pft *pft,Real scaler)
{
  Pftgrass *grass;
  grass=pft->data;
  
  grass->ind.leaf*=scaler;
  grass->ind.root*=scaler;
  pft->bm_inc*=scaler;
  
} /* of 'mix_veg_grass' */
