/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ n p p _ g r a s s . c                             **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

#define k 0.0548 

Real npp_grass(Pft *pft,Real gtemp_air,Real gtemp_soil,Real assim,Bool *negbm,Real wdf, Real *bgaresp) 
{
  Pftgrass *grass;
  const Pftgrasspar *par;
  Real npp,mresp,gresp;
  grass=pft->data;
  par=pft->par->data;
  mresp=pft->par->respcoeff*k*grass->ind.root*
       pft->nind/par->cn_ratio.root*gtemp_soil*pft->phen;
  gresp=(assim-mresp)*0.25;
  *bgaresp=mresp; //BP new output
  if (gresp<0.0) gresp=0.0;
#ifdef DEBUG3
  printf("mresp=%g gresp=%g assim=%g bminc=%g\n",mresp,gresp,assim,pft->bm_inc);
#endif
  npp=assim-mresp-gresp;
  pft->bm_inc+=npp;
  return npp;
} /* of 'npp_grass' */
