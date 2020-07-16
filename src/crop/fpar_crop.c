/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ f p a r _ c r o p . c                              **/
/*******************************************************************/

#include "lpj.h"
#include "crop.h"

Real fpar_crop(Pft *pft)
{
  Pftcrop *crop;
  crop=pft->data;

  return 1-exp(-LAMBERTBEER_K*(crop->lai-crop->lai_nppdeficit));
} /* of 'fpar_crop' */
