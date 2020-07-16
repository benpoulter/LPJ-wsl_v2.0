/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ i n i t s o i l . c                                **/
/*******************************************************************/
#include "lpj.h"

void initsoil(Soil *soil,const Soilpar *soilpar, Real soildepth[2]) 
{
  int l;
  soil->par=soilpar;
  soil->soildepth[0]=soildepth[0]; 
  soil->soildepth[1]=soildepth[1]; 
  soil->cpool.fast=soil->cpool.slow=soil->litter.ag_tree=soil->litter.bg=
                   soil->litter.ag_grass=soil->k_mean.fast=soil->k_mean.slow=0.0;
  soil->litter.ag_tree_inc=soil->litter.ag_grass_inc=soil->litter.bg_inc=0.0;
  soil->litter.ag_tree_pinc=soil->litter.ag_grass_pinc=soil->litter.bg_pinc=0.0;
  soil->alag=soil->amp=soil->meanw1=soil->decomp_litter_mean=0.0;
  soil->snowpack=0.0;
  for (l=0;l<NSOILLAYER;l++)
    soil->w[l]=0.0;
  for (l=0;l<NSOILLAYER;l++) //SOILDEPTH - calculate whcs
    soil->whcs[l]=soil->par->whc[l]*soil->soildepth[l];

  soil->w_evap=0.0;
} /* of 'initsoil' */
