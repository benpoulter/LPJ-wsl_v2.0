/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ p e t p a r R A D . c                           **/
/*******************************************************************/
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "date.h"
#include "numeric.h"
#include "units.h"

#define qoo 1360.0
#define sigma 5.6704e-8   /* stefan-boltzmann constant (W m-2 K-4) */
#define beta 0.17         /* albedo                                */
#define abszero -273.15   /* abs zero (deg C)                      */
#define dayseconds 86400  /* seconds per day                       */
#define a 107.0           /* (a in W/m2; b unitless)               */
#define b 0.2
#define c 0.25
#define d 0.5
#define k 13750.98708

#define gamma(temp) (65.05+temp*0.064)
#define lambda(temp) (2.495e6-temp*2380)

Real petparRAD(Real *daylength,   /* daylength                      */
    Real *par,                    /* par flux                       */
    Real *pet,                    /* pet                            */
    Real lat,                     /* lat (deg)                      */
    Real day,                     /* day (1...365)                  */
    Real temp,                    /* temp (deg C)                   */
    Real swdown,                  /* shortwave downward flux (Wm-2) */
    Real lwdown                   /* longwave downward flux (Wm-2)  */
    )

{
  Real lup,knet,lnet;
  Real delta,u,v,hh,s;
  #ifdef SWRAD
    Real uu,vv,w,hn,rl,sinehh,qo;
      qo=qoo*(1.0+2.0*0.01675*cos(2*M_PI*(day+0.5)/NDAYYEAR));
  #endif
  
  delta=deg2rad(-23.4*cos(2*M_PI*(day+10.0)/NDAYYEAR));
  u=sin(deg2rad(lat))*sin(delta);
  v=cos(deg2rad(lat))*cos(delta);
  
  //COPIED FROM LPJ GUESS
  if (u>=v)
    hh=M_PI; // polar day
  else if (u<=-v)
    hh=0.0; // polar night
  else hh=acos(-u/v); // Eqn 11
  
  #ifdef SWRAD
    sinehh=sin(hh);
  #endif
  
  // Calculate daylength in hours from hh
  *daylength=24.0*hh/M_PI;
  
  lup=sigma*pow((temp-abszero),4);  /* longwave upward flux (W m-2)*/
  lnet=lwdown-lup;                  /* longwave net flux, downward positive (W m-2)*/
  knet=(1-beta)*swdown;             /* shortwave net flux, downward positive (W m-2)*/
  
  *par=dayseconds*knet*0.48;        /* better to use 0.48 instead of 0.50 */
  
  s=2.503e6*exp(17.269*temp/(237.3+temp))/((237.3+temp)*(237.3+temp));
  
  #ifndef SWRAD
    /* calculation of PET respects only longwave flux during daylight */
    *pet=dayseconds*(s/(s+gamma(temp))/lambda(temp))*(knet+lnet*(*daylength)/24);
  #endif
  
  #ifdef SWRAD //From LPJ-GUESS
    //Calculate actual radiation
    if(hh == 0) //Polar night (ifelse not in LPJ guess) - causes hh==0 and then sinhh==0 and then inf
      w=0;
    else
      w=(*par/0.48)/2.0/(u*hh+v*sinehh)/k;
  
    //Calculate longwave radiation
    rl=(b+(1.0-b)*(w/qo/(1.0-beta)-c)/d)*(a-temp);
  
    uu=w*u-rl;
    vv=w*v;
  
    //  Calculate half-period with positive net radiation, hn. In Eqn (25), hn defined for uu in range -vv to vv
    //  For uu >= vv, hn = pi (12 hours, i.e. polar day). For uu <= -vv, hn = 0 (i.e. polar night)
    if (uu>=vv)
      hn=M_PI; // polar day
    else if (uu<=-vv)
      hn=0.0; // polar night
    else
      hn=acos(-uu/vv);
  
    //  Calculate total EET for this day
    *pet=2.0*(s/(s+gamma(temp))/lambda(temp))*(uu*hn+vv*sin(hn))*k;
    //printf("%f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",w, *pet, *par,u, hh, v, sinehh, k);
  #endif
  
  if (*pet<0) *pet=0;
  
  return(knet); //return net SW in W/m2
} /* of petpar */
