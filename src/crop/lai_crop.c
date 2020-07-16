/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ l a i _ c r o p .c                                 **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

Real lai_crop(const Pft *pft)
{
  Pftcrop *crop;
  crop=pft->data;

  return (crop->lai-crop->lai_nppdeficit);
} /* of 'lai_crop' */

/*
- this function is called in interception() and returns the specific lai of a cft 
*/
