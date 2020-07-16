/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ l i t t e r _ u p d a t e _ d e f o r e s t . c **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"
#include "grass.h"

Real litter_update_deforest(Litter *litter, Pft *pft,Real frac,Real *harvestdef,Real *deforestProduct, Bool subannual)    /*frac...pft->nind*/
{
  Pfttree *tree;
  Pftgrass *grass;

  //DEVQ:  probably not correct to have ALL NIND go into the soil because the actual harvest flux is scaled by fraction later on
  //DEVQ:  original assumption was that 15% of sapwood was below ground and that this stayed on site, and that everything else went off
  if(subannual){
    if(istree(pft)){
      tree=pft->data;

      #ifdef DEFORESTPRODUCT
        //update carbon in product pools remaining (output and reset annually)
        //..gets passed to product pools in landusechange routine and multiplied by deforest fraction
        //..37.5% of wood to immediate combustion, 37.5% of wood to product pools (2yr), 25% of wood for product pool (25yr) 
        //..100% leaf,root to litter
        *deforestProduct += 0.625*(tree->ind.sapwood+tree->ind.heartwood-tree->ind.debt)*frac;
        *harvestdef += 0.375*(tree->ind.sapwood+tree->ind.heartwood-tree->ind.debt)*frac;

        litter->bg+=tree->ind.root*frac;
        litter->ag_tree+=tree->ind.leaf*frac;
      #else
        litter->bg+=tree->ind.root*frac;
        litter->ag_tree+=(0.5*tree->ind.sapwood+tree->ind.leaf)*frac;
        *harvestdef+=(0.5*tree->ind.sapwood+tree->ind.heartwood-tree->ind.debt)*frac; 
      #endif
    }else if(isgrass(pft)){
       //grass leaf biomass goes to ag_grass rather than is harvest
       grass=pft->data;
       
       litter->bg+=grass->ind.root*frac;
       litter->ag_grass+=grass->ind.leaf*frac;
       *harvestdef+=0;
    }
  }else{
    if(istree(pft)){
      tree=pft->data;

      litter->bg_inc+=tree->ind.root*frac;
      litter->ag_tree_inc+=(0.5*tree->ind.sapwood+tree->ind.leaf)*frac;
      *harvestdef+=(0.5*tree->ind.sapwood+tree->ind.heartwood-tree->ind.debt)*frac; 
    }else if(isgrass(pft)){
      //grass leaf biomass goes to ag_grass rather than is harvest
      grass=pft->data;

      litter->bg_inc+=grass->ind.root*frac;
      litter->ag_grass_inc+=grass->ind.leaf*frac;
      *harvestdef+=0;
    }
  }

  return 0;
} /* of 'litter_update_deforest' */
