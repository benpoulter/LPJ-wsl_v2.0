/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ s n o w . c                                        **/
/*******************************************************************/
#include "lpj.h"

#ifdef PERMAFROST
  #define snowdens_start 150 //snow density at start of snow season [kg m-3]
  #define snowdens_end 500   //snow density at end of snow season [kg m-3]
  #define tsnow -2.0
  #define km 3.0
  #define maxsnowpack 20000.0
#else
  #define tsnow 0.0
  #define km 3.0
  #define maxsnowpack 20000.0
#endif

/*    
 *    Function snow
 *
 *    Adjust daily precipitation by snowmelt and accumulation in snowpack
 *    Ref: Haxeltine & Prentice 1996
 *
 */

Real snow(Real *snowpack, /* snowpack depth (mm) */
          Real *prec,     /* Precipitation (mm) */
          Real *snowmelt, /* snowmelt */
          Real temp       /* temperature */
         )
{
  Real runoff=0;
  
  if(temp<tsnow){
    *snowpack+=*prec;
    if(*snowpack>maxsnowpack){
      runoff=(*snowpack)-maxsnowpack;
      *snowpack=maxsnowpack;
    }
    *snowmelt=*prec=0.0;
  }else{
    /* *snowmelt=km*(temp-tsnow);*/
    /* following Gerten et al. 2004 */
    *snowmelt=(1.5+0.007**prec)*(temp-tsnow);
    if(*snowmelt>*snowpack) 
      *snowmelt=*snowpack;
    *snowpack-=*snowmelt;
  }
  return runoff;
} /* of 'snow' */

#ifdef PERMAFROST
/*    
 *    Function snownew 
 *
 *    Calculate snow density uses a varying snow density instead of a fixed one
 *    Adjust daily precipitation by snowmelt and accumulation in snowpack
 *    Ref: Haxeltine & Prentice 1996
 *
 */
void snownew(
       Real dtemp,              /* temperature */
        Real dprec,             /* Precipitation (mm) set to pointer if want to keep change */
       Real snowmelt,           /* snowmelt set to pointer if want to keep change */
       Permafrost *permafrost, 
       int year,
       int day
            )
{
  Real newsnow;      
  
  //Calculate snow melt and new snow for today
  if(dtemp < tsnow){
    newsnow = dprec;
    snowmelt = 0.0;
    permafrost->all_melted = FALSE;
  }else{
    newsnow = 0.0;
    snowmelt=(1.5+0.007*dprec)*(dtemp-tsnow);
  }

  //Set maximum snowmelt to size of swe
  if(snowmelt > permafrost->swe){
    snowmelt= permafrost->swe;
    if(permafrost->swe > 0.0) permafrost->all_melted = TRUE;
      permafrost->swe = 0.0;
  }
  if(permafrost->all_melted){
    permafrost->snow_days_prev = permafrost->snow_days + 1;
    permafrost->snow_days = 1;
    permafrost->all_melted = FALSE;  
  }
  permafrost->swe=max(0.0,permafrost->swe+newsnow-snowmelt);
  if(permafrost->swe > 0.01){
    permafrost->snow_days+=1;
  }

  //set maximum swe = 10m swe unit = mm
  permafrost->swe = min(permafrost->swe,10000.0);

  //set maximum permafrost.snow_days to 365 (in some areas you can have permanent snowcover)
  permafrost->snow_days = min(permafrost->snow_days,365);

  //Monthly permafrost.swe [m]
  //mswe = mswe +swe/ndaysmonth
  
  /*    Calculate SNOWDENS - use a fixed value for the first year, then
  c     count the days on which you have a snowcover and use that estimate
  c     for the next year. For the first 3/4 (or half, if desired) of the
  c     snow cover duration, the snow density stays fixed, for the last
  c     1/4 (or second half), the density increases linearly.
  c     ------------------------------------------------------------------*/
  if(year < 1902){
    permafrost->snowdens = snowdens_start;
  }else if(permafrost->snow_days < permafrost->snow_days_prev * 0.75){
    permafrost->snowdens = snowdens_start;
  }else if(permafrost->snow_days < permafrost->snow_days_prev){
    permafrost->snowdens= snowdens_start+
                          (snowdens_end - snowdens_start)/(permafrost->snow_days_prev*0.25)*(permafrost->snow_days - permafrost->snow_days_prev*0.75);
  }else{
    permafrost->snowdens = snowdens_end;
  }

  /*    Calculate effective water supply (as daily values in mm). swe
  c     units are either mm or kg m-2. Hence you divide kg m-2 by kg m-3
  c     and you get m.  Multiply by 1000 to get from m to mm*/
  permafrost->snowdepth = permafrost->swe / permafrost->snowdens *1000;
  dprec = dprec - newsnow;

  return;
}
#endif
