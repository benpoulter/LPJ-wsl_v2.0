/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ n e w _ t r e e . c                                **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

void new_tree(Pft *pft /* Parameter of pft */
             )         
{
  Pfttree *tree;
  tree=new(Pfttree);
  check(tree);
  pft->data=tree;
  init_tree(pft);
  /* tree->leafon=FALSE;                      */
  /* pft->bm_inc=pft->wscal_mean=pft->phen=0; */
  /* tree->gddtw=tree->aphen_raingreen=0;     */
  /* tree->leafondays=tree->leafoffdays=0;    */
  tree->ind.root=tree->ind.sapwood=tree->ind.heartwood=tree->ind.leaf=0.0;
  tree->ind.debt=tree->gddtw=tree->aphen_raingreen=0.0;
  tree->height=tree->crownarea=0.0;
} /* of 'new_tree' */
