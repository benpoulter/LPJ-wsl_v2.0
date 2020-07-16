/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ f p c _ t r e e . c                                **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

Real fpc_tree(Pft *pft)
{
  Pfttree *tree;
  Real fpc_old;
  fpc_old=pft->fpc;
  tree=pft->data;
  pft->fpc=(tree->crownarea>0.0) ? tree->crownarea*pft->nind*
                                   (1.0-exp(-1*pft->par->k_beer*lai(pft))) : 0; 
  return (pft->fpc<fpc_old) ? 0 : pft->fpc-fpc_old;
} /* of 'fpc_tree' */
