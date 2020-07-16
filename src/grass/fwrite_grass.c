/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ f w r i t e _ g r a s s . c                       **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

Bool fwrite_grass(FILE *file,const Pft *pft)
{
  const Pftgrass *grass;
  grass=pft->data;
  return fwrite(grass,sizeof(Pftgrass),1,file)!=1;
} /* of 'fwrite_grass' */
