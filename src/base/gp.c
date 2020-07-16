/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ g p . c                                            **/
/*******************************************************************/

#include "lpj.h"

#define lambda_opt 0.8

Real gp(Pft  *pft,             /* Pft list */
        Real co2,              /* atmospheric CO2 concentration (ppm) */
        Real temp,             /* temperature (deg C) */
        Real par,              /* photosynthetic active radiation flux */
        Real daylength,        /* daylength (h) */
        Real *gp_stand_leafon, /* pot. canopy conduct.at full leaf cover */
        Real *fpc              /* total fpc of all Pfts */
       )
{
  Real agd,adtmm,gp,rd;
  *gp_stand_leafon=gp=*fpc=0;
  if(daylength==0)
    return 0;
  adtmm=photosynthesis(&agd,&rd,pft->par->path,lambda_opt,
                         temp_stress(pft->par,temp,daylength),ppm2Pa(co2),
                         temp,par,pft->fpc,daylength,pft->par->alphaa);
  gp=1.6*adtmm/(ppm2bar(co2)*(1.0-lambda_opt)*hour2sec(daylength));
  *fpc=pft->fpc;

  *gp_stand_leafon= (gp<1e-20 || *fpc<1e-20) ? 0 : gp;
  return (gp<1e-20 || *fpc<1e-20) ? 0 : gp;
} /* of 'gp_sum' */
