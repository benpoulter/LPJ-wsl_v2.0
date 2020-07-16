/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ i s n e g _ g r a s s . c                         **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

Bool isneg_grass(const Pft *pft)
{
  Pftgrass *grass;
  grass=pft->data;
  #ifdef DEBUG3
  printf("isneg: %g %g\n",grass->ind.leaf,grass->ind.root);
  #endif
  return (grass->ind.leaf<0 || grass->ind.root<0.0 || pft->fpc<=1e-20);
} /* of 'isneg_grass' */
