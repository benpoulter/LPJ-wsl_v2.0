/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ i n i t _ g r a s s . c                           **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

void init_grass(Pft *pft)
{
  pft->bm_inc=pft->wscal_mean=0;
  pft->monopool=0.0;
  pft->nind=1;
} /* of 'init_grass' */
