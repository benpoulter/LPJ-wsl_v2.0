/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f w r i t e s o i l . c                            **/
/*******************************************************************/
#include "lpj.h"

Bool fwritesoil(FILE *file,const Soil *soil,Bool full)
{
  fwrite1(&soil->cpool,sizeof(Pool),file);
  fwrite1(&soil->litter,sizeof(Litter),file);
  fwriten(soil->w,sizeof(Real),NSOILLAYER,file);
  fwrite1(&soil->w_evap,sizeof(Real),file);
  fwrite1(&soil->snowpack,sizeof(Real),file);
  fwriten(&soil->soildepth,sizeof(Real),NSOILLAYER,file); 
  fwriten(&soil->whcs,sizeof(Real),NSOILLAYER,file);     
  if(full){
    fwrite1(&soil->alag,sizeof(Real),file);
    fwrite1(&soil->amp,sizeof(Real),file);
    fwrite1(&soil->meanw1,sizeof(Real),file);
  }
  return FALSE;
} /* of 'fwritesoil' */
