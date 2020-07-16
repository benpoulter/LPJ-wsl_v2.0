/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ f p c _ g r a s s . c                             **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

Real fpc_grass(Pft *pft)
{
  Real fpc_old;
  fpc_old=pft->fpc;
  pft->fpc=pft->nind*(1.0-exp(-1*pft->par->k_beer*lai_grass(pft))); 
  return (pft->fpc<fpc_old) ? 0 : pft->fpc-fpc_old;
} /* 'fpc_grass' */
