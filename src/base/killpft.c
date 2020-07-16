/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ k i l l p f t . c                                  **/
/*******************************************************************/

#include "lpj.h"

Bool killpft(Litter *litter,Pft *pft,const Climbuf *climbuf)
{
  Real rharvest;
  if(!survive(pft->par,climbuf)){
    litter_update(litter,pft,pft->nind,&rharvest,FALSE,FALSE);
    return TRUE; 
  }else{
    return FALSE;
  }
} /* of 'killpft' */
