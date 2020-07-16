/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ a d j u s t _ t r e e . c                          **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

void adjust_tree(Litter *litter,Pft *pft,Real fpc_tree,Bool subannual) 

{
  Real frac,rharvest;

  frac=FPC_TREE_MAX/fpc_tree;
  pft->nind*=frac;
  pft->fpc*=frac;
  litter_update_tree(litter,pft,(pft->nind/frac-pft->nind),&rharvest,FALSE,subannual); 
  
} /* of 'adjust_tree' */

/***************************************************************************/
/**                                                                       **/
/** CALLED FROM:                                                          **/
/**   establishment.c (foreachpft)                                        **/
/** PURPOSE:                                                              **/
/**   reduce nind and fpc if total fpc for all trees exceeds              **/
/**   0.95 (FPC_TREE_MAX)                                                 **/
/** DETAILED DESCRIPTION:                                                 **/
/**   litter_update_tree is used to transfer the "waste" carbon into the  **/
/**   litter pool in order to the maintain carbon balance                 **/
/**                                                                       **/
/***************************************************************************/
