/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ f r e a d _ g r a s s . c                         **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

void fread_grass(FILE *file,Pft *pft,Bool swap)
{
  Pftgrass *grass;
  grass=new(Pftgrass);
  pft->data=grass;
  freadreal((Real *)&grass->ind,sizeof(Grassphys)/sizeof(Real),swap,file);
  fpc_grass(pft);
} /* of 'fread_grass' */
