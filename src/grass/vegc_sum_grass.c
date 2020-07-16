/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ v e g c _ s u m _ g r a s s . c                   **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

Real vegc_sum_grass(const Pft *pft)
{
  const Pftgrass *grass;
  grass=pft->data;
  return phys_sum_grass(grass->ind)*pft->nind;
} /* of 'vegc_sum_grass' */
