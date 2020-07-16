/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ s o i l t e m p . c                                **/
/*******************************************************************/
#include "lpj.h"

#define LAG_CONV (NDAYYEAR*0.5*M_1_PI)  /* conversion factor for oscillation 
                                lag from angular units to days (=365/(2*PI))*/

Real soiltemp(const Soil *soil,const Climbuf *climbuf)
{
  Real a,b,temp_lag;
  if(soil->w[0]==0)
    return climbuf->temp[NDAYS-1];
  linreg(&a,&b,climbuf->temp,NDAYS);
  temp_lag=a+b*(NDAYS-1-soil->alag*LAG_CONV);
  return climbuf->atemp_mean+soil->amp*(temp_lag-climbuf->atemp_mean);
} /* of 'soiltemp' */
