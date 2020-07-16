/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ i n i t p e r m a f r o s t . c                    **/
/*******************************************************************/
#include "lpj.h"
#include "permafrost.h"
void initpermafrost(Permafrost *permafrost,
                    Soil *soil,     
                    float oc_top, //organic matter content
                    float tsl     //soil temperature
        )
{
  int l;
  //Determine if it is a peatland
  if(oc_top < 4){
    permafrost->peathydr = FALSE;
  }else{
    permafrost->peathydr = TRUE;
  }
  //Currently set to FALSE to off peatland module
  permafrost->peathydr = FALSE;

  permafrost->thawdepth  = 0.0;
  permafrost->thaw_top   = 0.0;
  permafrost->snowdepth  = 0.0;
  permafrost->first      = TRUE;
  permafrost->swe        = 0.0;
  permafrost->snow_days  = 1;
  permafrost->snowdepth  = 0.0;
  permafrost->all_melted = FALSE;
  permafrost->isstable   = TRUE;
  fornlayer(l){
    permafrost->Tsoil_365[l]   = tsl;
    permafrost->Tsoil[l]       = tsl;
    permafrost->T_old[l]       = 0.0;
    permafrost->Tsoil_prev[l]  = tsl;
    permafrost->Fice[l]        = 0.0;
    permafrost->Fice_365[l]    = 0.0;
    permafrost->Fice_prev[l]   = 0.0;
    permafrost->Fwater[l]      = soil->par->wat_hld;
    permafrost->Fwater_365[l]  = 0.0;
    permafrost->Fwater_prev[l] = 0.0;
    permafrost->Fpwp[l]        = soil->par->wat_hld;
    permafrost->Dz[l]          = 100.0;
    permafrost->Di[l]          = 0.04 * 1E6;
    permafrost->ice_frac[l]    = 0.0;
    permafrost->wtot[l]        = 0.0;
  }
  for(l=0;l<PAD_LAYERS;l++){
    permafrost->pad_dz[l]=0.0;
    permafrost->pad_temp[l]=0.0;
  }
  permafrost->ice_frac1=permafrost->ice_frac2=0.0;
  
  return;
} /* of 'initpermafrost' */
