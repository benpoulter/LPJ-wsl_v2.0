/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ l i g h t _ t r e e . c                            **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

void light_tree(Litter *litter,Pft *pft,Real excess,Bool subannual)
{
  Real rharvest;
  Real nind_kill;  /* reduction in individual density to reduce tree FPC to
                      permitted maximum (indiv/m2)*/

#ifdef DEBUG3
  printf("light: %g %g %s %g\n",pft->fpc,pft->nind,pft->par->name,excess);
#endif
  nind_kill=(excess<1e-20) ? 0 : pft->nind*(excess/pft->fpc); 
  pft->nind-=nind_kill;
  litter_update_tree(litter,pft,nind_kill,&rharvest,FALSE,subannual);
  fpc_tree(pft);

} /* of 'light_tree' */
