/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ n e w _ g r a s s . c                             **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

void new_grass(Pft *pft /* Parameter of pft */
              )        
{
  Pftgrass *grass;
  grass=new(Pftgrass);
  pft->data=grass;
  pft->bm_inc=pft->wscal_mean=pft->phen=0;
  pft->nind=1;
  grass->ind.leaf=grass->ind.root=0;
} /* of 'new_grass' */
