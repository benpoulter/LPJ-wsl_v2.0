/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ f w r i t e _ c r o p . c                          **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

Bool fwrite_crop(FILE *file,const Pft *pft)
{
  return (fwrite(pft->data,sizeof(Pftcrop),1,file)!=1);
} /* of 'fwrite_crop' */
