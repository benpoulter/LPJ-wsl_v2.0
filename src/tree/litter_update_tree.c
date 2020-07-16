/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ l i t t e r _ u p d a t e _ t r e e . c            **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

Real litter_update_tree(Litter *litter, Pft *pft,Real frac,Real *rharvest, Bool isharvest,Bool subannual)
{
  Pfttree *tree;
  tree=pft->data;
  
  if(subannual){
    litter->ag_tree+=(tree->ind.leaf+tree->ind.sapwood+tree->ind.heartwood-
                      tree->ind.debt)*frac;
    litter->bg+=tree->ind.root*frac;  
  }else{
    litter->ag_tree_inc+=(tree->ind.leaf+tree->ind.sapwood+tree->ind.heartwood-
                    tree->ind.debt)*frac;
    litter->bg_inc+=tree->ind.root*frac;  
  }
  *rharvest=0;
  return 0;
} /* of 'litter_update_tree' */
