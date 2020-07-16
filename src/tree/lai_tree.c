/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ l a i _ t r e e . c                                **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

Real lai_tree(const Pft *pft)
{
  Pfttree *tree;
  tree=pft->data;
    
  return (tree->crownarea>0.0) ? 
         tree->ind.leaf*getpftpar(pft,sla)/tree->crownarea : 0;
} /* of 'lai_tree' */
