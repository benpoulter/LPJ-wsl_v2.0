/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ t u r n o v e r _ g r a s s . c                   **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

/*
 *
 *  Function turnover
 *
 *  Turnover of PFT-specific fraction from each living C pool
 *  Leaf and root C transferred to litter, sapwood C to heartwood
 *
 */

Real turnover_grass(Litter *litter,Pft *pft) /* returns turnover */
{
  Pftgrass *grass;
  const Pftgrasspar *grasspar;
  Grassphys gturn;
  grass=pft->data;
  grasspar=getpftpar(pft,data);
  gturn.leaf=grass->ind.leaf/grasspar->turnover.leaf; 
  gturn.root=grass->ind.root/grasspar->turnover.root;
  grass->ind.leaf-= gturn.leaf;
  grass->ind.root-= gturn.root;
  litter->ag_grass_inc+=gturn.leaf*pft->nind; 
  litter->bg_inc+=gturn.root*pft->nind;
  return gturn.leaf+gturn.root;
} /* of 'turnover_grass' */
