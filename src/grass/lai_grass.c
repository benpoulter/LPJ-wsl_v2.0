/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ l a i _ g r a s s . c                             **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

Real lai_grass(const Pft *pft)
{
  return ((Pftgrass *)pft->data)->ind.leaf*getpftpar(pft,sla);
} /* 'lai_grass' */
