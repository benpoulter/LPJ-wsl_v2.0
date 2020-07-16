/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ r e p r o d u c t i o n _ t r e e . c              **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

#define reprod_cost 0.1
#define CDEBT_PAYBACK_RATE 0.2

void reproduction_tree(Litter *litter,Pft *pft)
{
  Real reprod,cmass_excess,payback;
  Pfttree *tree;
  tree=pft->data;
  if(pft->bm_inc<0)
    return;
  reprod=pft->bm_inc*reprod_cost;
  litter->ag_tree_inc+=reprod;  
  pft->bm_inc-=reprod;
  if(israingreen(pft))
  {
    cmass_excess=tree->aphen_raingreen/(pft->par->longivity*NDAYYEAR)*
                (tree->ind.leaf+tree->ind.root)*pft->nind-(tree->ind.leaf+tree->ind.root)*pft->nind;
    if (cmass_excess>0.0)
    {
      if (cmass_excess>pft->bm_inc) 
        cmass_excess=pft->bm_inc;
      litter->ag_tree_inc+=cmass_excess; 
      pft->bm_inc-=cmass_excess;
    }
  }
  payback=tree->ind.debt*pft->nind*CDEBT_PAYBACK_RATE>pft->bm_inc ? pft->bm_inc : tree->ind.debt*pft->nind*CDEBT_PAYBACK_RATE;
  pft->bm_inc-=payback;
  tree->ind.debt-=payback/pft->nind;
} /* of 'reproduction_tree' */
