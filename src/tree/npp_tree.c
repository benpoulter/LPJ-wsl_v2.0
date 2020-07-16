/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ n p p _ t r e e . c                                **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

#define k 0.0548 

Real npp_tree(Pft *pft,Real gtemp_air,Real gtemp_soil,Real assim,Bool *negbm,Real wdf, Real *bgaresp) 
{
  Pfttree *tree;
  const Pfttreepar *par;
  Treephys resp;
  Real mresp,gresp,npp; 
  tree=pft->data;
  par=pft->par->data;

  //-----------------------------
  //   plant respiration 
  //-----------------------------
  resp.sapwood=pft->par->respcoeff*k*tree->ind.sapwood*pft->nind/
               par->cn_ratio.sapwood*gtemp_air;
  resp.root=pft->par->respcoeff*k*tree->ind.root*pft->nind/par->cn_ratio.root*
            gtemp_soil*pft->phen;
  gresp=(assim-resp.sapwood-resp.root)*0.25;
  if (gresp<0.0) gresp=0.0;
  mresp=resp.sapwood+resp.root;
  *bgaresp=resp.root+resp.sapwood*0.3;

  #ifdef DEBUG3
    printf("mresp=%g, gresp=%g\n",mresp,gresp);
  #endif

  //-----------------------------
  //   net primary production 
  //-----------------------------
  npp=assim-mresp-gresp;
  pft->bm_inc+=npp;

  return npp;
} /* of 'npp_tree' */
