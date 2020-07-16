/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ v e g c _ s u mm _ t r e e . c                     **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

Real vegc_sum_tree(const Pft *pft)
{
  const Pfttree *tree;
  tree=pft->data;
  return (phys_sum_tree(tree->ind)-tree->ind.debt)*pft->nind;
} /* of 'vegc_sum_tree' */
