/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ f i r e _ t r e e . c                              **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

Real fire_tree(Pft *pft,Real *fireprob)
{
  Real disturb,flux;
  Pfttree *tree;
  tree=pft->data;
  disturb=(1-pft->par->resist)**fireprob;
  flux=disturb*pft->nind*(tree->ind.leaf+tree->ind.sapwood+
                          tree->ind.heartwood-tree->ind.debt+tree->ind.root);
  pft->nind*=(1-disturb);
  return flux;
} /* of 'fire_tree' */
