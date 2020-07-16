/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ i n i t _ t r e e . c                              **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

void init_tree(Pft *pft)
{
  Pfttree *tree;
  tree=pft->data;
  pft->bm_inc=pft->wscal_mean=0;
  pft->monopool=0.0; //BVOC
  tree->gddtw=tree->aphen_raingreen=0;
/*  tree->leafondays=tree->leafoffdays=0;*/
} /* of 'init_tree' */
