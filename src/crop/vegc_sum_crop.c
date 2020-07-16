/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ v e g c _ s u m _ c r o p . c                      **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

Real vegc_sum_crop(const Pft *pft)
{
  const Pftcrop *crop;
  crop=pft->data;
  return phys_sum_crop(crop->ind)*pft->nind;
} /* of 'vegc_sum_crop' */
