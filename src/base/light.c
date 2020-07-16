/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ l i g h t . c                                      **/
/*******************************************************************/

#include "lpj.h"
#include "tree.h"

void light(Litter *litter,
           Pftlist pftlist,
           int ntypes,
           const Real fpc_inc[],
           Bool subannual)

{
  int p;
  Real ntree;         /*no of tree PFTs currently present*/
  Real *fpc_total;    /*total grid FPC for PFTs     */ 
  Real fpc_inc_tree;  /*this years total FPC increment for tree PFTs*/
  Real excess;        /*tree FPC or grass cover to be reduced*/
  Pft *pft;
/*    Calculate total woody FPC, FPC increment and grass cover (= crown area)*/
 
  fpc_inc_tree=0.0;
  ntree=0;
  fpc_total=newvec(Real,ntypes);
  check(fpc_total);
  fpc_sum(fpc_total,ntypes,pftlist);

  foreachpft(pft,p,pftlist){
    if(istree(pft)){
      fpc_inc_tree+=fpc_inc[p];
      ntree++;
    }
  }

  foreachpft(pft,p,pftlist){
    switch(getpftpar(pft,type))
    {
      case TREE:
        if(fpc_total[TREE]>FPC_TREE_MAX){ 
          excess =(fpc_inc_tree>0.0) ? 
                  (fpc_total[TREE]-FPC_TREE_MAX)*(fpc_inc[p]/fpc_inc_tree) :
                  (fpc_total[TREE]-FPC_TREE_MAX)/ntree;

          /*  Reduce individual density (and thereby gridcell-level biomass)*/
          /*  so that total tree FPC reduced to 'fpc_tree_max'*/
          /* changed by Werner von Bloh to avoid FPE */
          
          pft->par->light(litter,pft,excess,subannual);
        }
        break;
                              
      case GRASS:
        if(fpc_total[GRASS]>(1.0-min(fpc_total[TREE],FPC_TREE_MAX))){
          excess=(min(fpc_total[TREE],FPC_TREE_MAX)+fpc_total[GRASS]-1.0)*
                 (pft->fpc/fpc_total[GRASS]);
          pft->par->light(litter,pft,excess,subannual);
        }
        break;
    }/* of 'switch' */
  }/* of 'foreachpft' */
  free(fpc_total);
} /* of 'light' */
