/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ e q u i l s o i l . c                              **/
/*******************************************************************/
#include "lpj.h"

/*
 *  DESCRIPTION
 *
 *  Analytically solves differential flux equations for fast and slow SOM 
 *  pools assuming annual litter inputs close to long term equilibrium
 *
 */

void equilsoil(Soil *soil) 
{
  
  soil->cpool.fast=soil->k_mean.fast>0 ? soilfrac*fastfrac*soil->decomp_litter_mean/
                   soil->k_mean.fast : 0;
  soil->cpool.slow=soil->k_mean.slow>0 ? soilfrac*(1.0-fastfrac)*soil->decomp_litter_mean/
                   soil->k_mean.slow : 0;
} /* of 'equilsoil' */
