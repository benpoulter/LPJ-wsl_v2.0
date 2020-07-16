/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f r e a d s o i l . c                              **/
/*******************************************************************/
#include "lpj.h"

Bool freadsoil(FILE *file,Soil *soil,const Soilpar *soilpar,Bool swap)
{
  soil->par=soilpar;
  freadreal((Real *)&soil->cpool,sizeof(Pool)/sizeof(Real),swap,file);
  freadreal((Real *)&soil->litter,sizeof(Litter)/sizeof(Real),swap,file);
  freadreal(soil->w,NSOILLAYER,swap,file);
  freadreal1(&soil->w_evap,swap,file);
  freadreal1(&soil->snowpack,swap,file);
  freadreal(soil->soildepth,NSOILLAYER,swap,file); 
  freadreal(soil->whcs,NSOILLAYER,swap,file);     
  freadreal1(&soil->alag,swap,file);
  freadreal1(&soil->amp,swap,file);
  soil->k_mean.fast=soil->k_mean.slow=soil->decomp_litter_mean=0;
  return (freadreal1(&soil->meanw1,swap,file)!=1);
} /* of 'freadsoil' */
