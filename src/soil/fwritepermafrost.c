/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f w r i t e p e r m a f r o s t . c                **/
/*******************************************************************/
#include "lpj.h"

Bool fwritepermafrost(FILE *file,const Permafrost *permafrost,Bool full)
{
  fwrite1(&permafrost->snow_days_prev,sizeof(int),file);
  fwrite1(&permafrost->snow_days,sizeof(int),file);
  fwrite1(&permafrost->swe,sizeof(Real),file);
  fwrite1(&permafrost->snowdens,sizeof(Real),file);
  fwrite1(&permafrost->snowdepth,sizeof(Real),file);
  fwriten(permafrost->Dz,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Di,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->pad_dz,sizeof(Real),PAD_LAYERS,file);
  fwriten(permafrost->pad_temp,sizeof(Real),PAD_LAYERS,file);

  fwrite1(&permafrost->thawdepth,sizeof(Real),file);
  fwrite1(&permafrost->thaw_top,sizeof(Real),file);

  fwriten(permafrost->Tsoil_365,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Tsoil,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->T_old,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Tsoil_prev,sizeof(Real),NLAYERS,file);
  
  fwriten(permafrost->Fice,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Fice_365,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Fice_prev,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->ice_frac,sizeof(Real),NLAYERS,file);

  fwriten(permafrost->Fwater,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Fwater_365,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Fwater_prev,sizeof(Real),NLAYERS,file);
  //currently permanent wilting point is constant value but it will be variable in future implementation
  fwriten(permafrost->Fpwp,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->Fair,sizeof(Real),NLAYERS,file);
  fwriten(permafrost->wtot,sizeof(Real),NLAYERS,file);

  fwrite1(&permafrost->ice_frac1,sizeof(Real),file);
  fwrite1(&permafrost->ice_frac2,sizeof(Real),file);
  fwrite1(&permafrost->peathydr,sizeof(Bool),file);

  return FALSE;
} /* of 'fwritepermafrost' */
