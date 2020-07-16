/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ f p r i n t _ g r a s s . c                       **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"


void fprint_grass(FILE *file,const Pft *pft)
{
  Pftgrass *grass;
  grass=pft->data;
  fprintf(file,"Cmass: ");
  fprintgrassphys(file,grass->ind);
  fprintf(file,"\n");
} /* of 'fprint_grass' */
