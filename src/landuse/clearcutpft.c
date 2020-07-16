/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ c l e a r c u t p f t . c                       **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

void clearcutpft(Litter *litter, Pftlist pftlist, Real *harvestflux)
{
  int p;
  Pft *pft;
  Pfttree *tree;

  //Loop through all PFTs in the stand
  foreachpft(pft,p,pftlist){
    //If it is a tree, then clearcut the PFT
    //..can be modified to do species (but could also change here to do fraction, nind, etc...)
    if(istree(pft)){
      //Get the PFT data
      tree=pft->data;

      //Remove all aboveground biomass (85% sapwood and 100% heartwood + leaves offsite, 15% to litter)
      *harvestflux+=(tree->ind.sapwood*0.85+tree->ind.heartwood-tree->ind.debt)*pft->nind+tree->ind.leaf*pft->nind;
      //All dead roots go belowground
      litter->bg_inc+=tree->ind.root*pft->nind;
      //Remaining sapwood goes aboveground
      litter->ag_tree_inc+=tree->ind.sapwood*0.15*pft->nind;

      //Delete the PFT
      delpft(pftlist,p);
    }
  }
} /* of 'cleacutpft' */

/*
Sept2012: This routine is similar to litter_update_deforest.c. 
Can modify nind to do selective or partial logging. Can modify PFT to do species specific logging.
*/
