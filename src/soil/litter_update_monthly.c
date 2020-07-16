/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ l i t t e r _ u p d a t e _ m o n t h l y . c      **/
/*******************************************************************/
#include "lpj.h"

void litter_update_monthly(Cell *cell){
  int s;
  //Pft *pft;
  Stand *stand;
  
  foreachstand(stand,s,cell->standlist){
    stand->soil.litter.bg+=stand->soil.litter.bg_pinc/12;
    stand->soil.litter.ag_tree+=stand->soil.litter.ag_tree_pinc/12;
    stand->soil.litter.ag_grass+=stand->soil.litter.ag_grass_pinc/12;
  } 

}
