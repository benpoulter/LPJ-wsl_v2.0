/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ i n i t _ a n n u a l . c                           **/
/*******************************************************************/
#include "lpj.h"

void init_annual(Cell *cell,int npft)
{
  int s,p;
  Pft *pft;
  Stand *stand;
  initgdd(cell->gdd,npft);
  init_climbuf(&cell->climbuf);
  cell->aprec=cell->nep=cell->awater_flux=0.0;
  if(!cell->skip)
    foreachstand(stand,s,cell->standlist){
      #ifdef DEBUG2
        printf("init npft=%d\n",stand->pftlist->n);
      #endif
      //update litter increment of previous year
      stand->soil.litter.ag_tree_pinc=stand->soil.litter.ag_tree_inc;
      stand->soil.litter.ag_grass_pinc=stand->soil.litter.ag_grass_inc;
      stand->soil.litter.bg_pinc=stand->soil.litter.bg_inc;
      //reset increment of litter of this year
      stand->soil.litter.ag_tree_inc=0.0;
      stand->soil.litter.ag_grass_inc=0.0;
      stand->soil.litter.bg_inc=0.0;

      stand->fire_sum=0;
      foreachpft(pft,p,stand->pftlist)
        init(pft);
    
    } /* of foreachstand */
} /* of 'init_annual' */
