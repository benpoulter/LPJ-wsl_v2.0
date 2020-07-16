/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ a l l o m e t r y _ t r e e . c                    **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

/*
 *  ALLOMETRY
 *  Should be called to update allometry, FPC and FPC increment whenever 
 *  biomass values for a vegetation individual change.
 */

void allometry_tree(Pft *pft /* Pointer to tree pft */
                   )  
{
  Pfttree *tree;
  const Pfttreepar *treepar;
  Real stemdiam,sm_ind_temp; 

  tree=pft->data;
  treepar=getpftpar(pft,data);
  tree->height=(tree->ind.sapwood<=0.0 || tree->ind.leaf<=0.0) ? 0 : 
               k_latosa*tree->ind.sapwood/(tree->ind.leaf*pft->par->sla* wooddens);
      
  if(tree->height>height_max)
  {
    tree->height=height_max;
    sm_ind_temp=tree->ind.sapwood;
    tree->ind.sapwood=tree->ind.leaf*height_max*wooddens*pft->par->sla/
                      k_latosa;
    tree->ind.heartwood+=sm_ind_temp-tree->ind.sapwood;
  } 
  stemdiam=pow(tree->height/allom2,1.0/allom3);
  tree->crownarea=min(allom1*pow(stemdiam,reinickerp),treepar->crownarea_max);
} /* of allometry */ 
