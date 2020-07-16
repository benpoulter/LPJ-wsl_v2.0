/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f r e a d p e r m a f r o s t . c                  **/
/*******************************************************************/
#include "lpj.h"

Bool freadpermafrost(FILE *file,Permafrost *permafrost,const Soilpar *soilpar,Bool swap)
{
  freadint1(&permafrost->snow_days_prev,swap,file);
  freadint1(&permafrost->snow_days,swap,file);
  freadreal1(&permafrost->swe,swap,file);
  freadreal1(&permafrost->snowdens,swap,file);
  freadreal1(&permafrost->snowdepth,swap,file);
  freadreal(permafrost->Dz,NLAYERS,swap,file);
  freadreal(permafrost->Di,NLAYERS,swap,file);
  freadreal(permafrost->pad_dz,PAD_LAYERS,swap,file);
  freadreal(permafrost->pad_temp,PAD_LAYERS,swap,file);

  freadreal1(&permafrost->thawdepth,swap,file);
  freadreal1(&permafrost->thaw_top,swap,file);

  freadreal(permafrost->Tsoil_365,NLAYERS,swap,file);
  freadreal(permafrost->Tsoil,NLAYERS,swap,file);
  freadreal(permafrost->T_old,NLAYERS,swap,file);
  freadreal(permafrost->Tsoil_prev,NLAYERS,swap,file);

  freadreal(permafrost->Fice,NLAYERS,swap,file);
  freadreal(permafrost->Fice_365,NLAYERS,swap,file);
  freadreal(permafrost->Fice_prev,NLAYERS,swap,file);
  freadreal(permafrost->ice_frac,NLAYERS,swap,file);
  
  freadreal(permafrost->Fwater,NLAYERS,swap,file);
  freadreal(permafrost->Fwater_365,NLAYERS,swap,file);
  freadreal(permafrost->Fwater_prev,NLAYERS,swap,file);
  //current permanent wilting point is constant value but it will be variable in future implementation
  freadreal(permafrost->Fpwp,NLAYERS,swap,file);
  freadreal(permafrost->Fair,NLAYERS,swap,file);
  freadreal(permafrost->wtot,NLAYERS,swap,file);


  freadreal1(&permafrost->ice_frac1,swap,file);
  freadreal1(&permafrost->ice_frac2,swap,file);
  freadint1(&permafrost->peathydr,swap,file);

  permafrost->Tsoil_0=permafrost->Tsoil_5=permafrost->Tsoil_10=permafrost->Tsoil_15      =0.0;
  permafrost->Tsoil_20=permafrost->Tsoil_25=permafrost->Tsoil_30=permafrost->Tsoil_40    =0.0;
  permafrost->Tsoil_50=permafrost->Tsoil_60=permafrost->Tsoil_70=permafrost->Tsoil_85    =0.0;
  permafrost->Tsoil_100=permafrost->Tsoil_125=permafrost->Tsoil_150=permafrost->Tsoil_175=0.0;
  permafrost->Tsoil_200=permafrost->permafrost_index=0.0;

  permafrost->day_count=permafrost->perma_count=0.0;
  permafrost->first=FALSE;
  permafrost->isstable=TRUE;

  return FALSE;
} /* of 'freadpermafrost' */
