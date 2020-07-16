/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f p c _ s u m . c                                  **/
/*******************************************************************/

#include "lpj.h"

Real fpc_sum(Real fpc_type[],int ntype,const Pftlist pftlist)
{
  const Pft *pft;
  int p,i;
  Real fpc;
  for(i=0;i<ntype;i++)
    fpc_type[i]=0; 
  fpc=0;
  foreachpft(pft,p,pftlist)
  {
    fpc+=pft->fpc;   
    fpc_type[pft->par->type]+=pft->fpc; 
  }
  return fpc;
} /* of 'fpc_sum' */
