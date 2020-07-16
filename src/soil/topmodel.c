/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ t o p m o d e l . c                                **/
/*******************************************************************/
#include <math.h>
#include "lpj.h"

#ifdef PERMAFROST
  #define W_THRES   0.0285  //threshold for determining wetland,means when mean sm= 0.95, WTD=0
  #define SOILDEPTH 2.0     //defined soil depth
  #define Z_BOT     2.0     //the bottom depth of soil column

  #ifdef TOPMODEL
    #ifdef ERA
      #define K_TROPICS 0.0378 //Parameter K for tropics
      #define K_TEMPERATE 0.0147 //Parameter K for temperate
    #endif
    #ifdef MERRA2
      #define K_TROPICS 0.0391 //Parameter K for tropics
      #define K_TEMPERATE 0.0153 //Parameter K for temperate
    #endif
    #ifdef JRA55
      #define K_TROPICS 0.0436 //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRUNCEP
      #define K_TROPICS 0.0436 //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRU
      #define K_TROPICS 0.0848  //Parameter K for tropics
      #define K_TEMPERATE 0.0106 //Parameter K for temperate
    #endif
    #ifdef CMIP5
      #define K_TROPICS 0.0341 //Parameter K for tropics
      #define K_TEMPERATE 0.0133 //Parameter K for temperate
    #endif
  #else
    //SWAMPS parameters
    #ifdef ERA
      #define K_TROPICS 0.0378 //Parameter K for tropics
      #define K_TEMPERATE 0.0147 //Parameter K for temperate
    #endif
    #ifdef MERRA2
      #define K_TROPICS 0.0391 //Parameter K for tropics
      #define K_TEMPERATE 0.0153 //Parameter K for temperate
    #endif
    #ifdef JRA55
      #define K_TROPICS 0.0436 //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRUNCEP
      #define K_TROPICS 0.0436 //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRU
      #define K_TROPICS 0.1376  //Parameter K for tropics
      #define K_TEMPERATE 0.00783 //Parameter K for temperate
    #endif
    #ifdef CMIP5
      #define K_TROPICS 0.0341 //Parameter K for tropics
      #define K_TEMPERATE 0.0133 //Parameter K for temperate
    #endif
  #endif

#else
  #define W_THRES   0.1  //minimum threshold to form wetland
  #define SOILDEPTH 1.5  //defined soil depth
  #define Z_BOT     1.5  //the bottom depth of soil column

  #ifdef TOPMODEL
    //SWAMPS parameters
    #ifdef ERA
      #define K_TROPICS 0.0378   //Parameter K for tropics
      #define K_TEMPERATE 0.0147 //Parameter K for temperate
    #endif
    #ifdef MERRA2
      #define K_TROPICS 0.0391   //Parameter K for tropics
      #define K_TEMPERATE 0.0153 //Parameter K for temperate
    #endif
    #ifdef JRA55
      #define K_TROPICS 0.0436   //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRUNCEP
      #define K_TROPICS 0.0436   //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRU
      #define K_TROPICS 0.0115  //Parameter K for tropics
      #define K_TEMPERATE 0.01  //Parameter K for temperate
    #endif
    #ifdef CMIP5
      #define K_TROPICS 0.0341   //Parameter K for tropics
      #define K_TEMPERATE 0.0133 //Parameter K for temperate
    #endif
  #else
    //SWAMPS parameters
    #ifdef ERA
      #define K_TROPICS 0.0378   //Parameter K for tropics
      #define K_TEMPERATE 0.0147 //Parameter K for temperate
    #endif
    #ifdef MERRA2
      #define K_TROPICS 0.0391   //Parameter K for tropics
      #define K_TEMPERATE 0.0153 //Parameter K for temperate
    #endif
    #ifdef JRA55
      #define K_TROPICS 0.0436   //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRUNCEP
      #define K_TROPICS 0.0436   //Parameter K for tropics
      #define K_TEMPERATE 0.0170 //Parameter K for temperate
    #endif
    #ifdef CRU
      #define K_TROPICS 0.0144   //Parameter K for tropics
      #define K_TEMPERATE 0.0120 //Parameter K for temperate
    #endif
    #ifdef CMIP5
      #define K_TROPICS 0.0341   //Parameter K for tropics
      #define K_TEMPERATE 0.0133 //Parameter K for temperate
    #endif
  #endif
#endif


//keep parameters below future implementation
#define P_TROPICS 0.1252   //Parameter P for tropics CH4 production
#define P_TEMPERATE 0.0407 //Parameter P for temperate CH4 production

Real wtd(Real mswc[], const Soil *soil)
{
 Real sm_m, wtd; //mean soil moisture
 sm_m=0.0;
 sm_m= (mswc[0] * soil->soildepth[0] + mswc[1]* soil->soildepth[1]) / (soil->soildepth[0] + soil->soildepth[1]);
 
 //wtd unit: m
 wtd = Z_BOT-(sm_m + W_THRES)/(1-W_THRES)*SOILDEPTH;
 if(wtd > SOILDEPTH)
    wtd = SOILDEPTH;
 return wtd;

} /* of water table depth calculation*/

Real wetdfrac(const Topmodel *topmodel,
              const Soil *soil,
        Real temp_wtd,
        Real ice_frac[]  //two layers
        )
{
 Real cs,fmax,cti,tj,f;
 Real ice_scl=0.0;;
 Real wetfrac;
 cs=topmodel->cs;
 fmax=topmodel->fmax;
 cti=topmodel->cti;
 tj=topmodel->tj;
 f=soil->par->f;
 if(tj < -250){
  f*=0.7;
 }else if(tj >= -250 && tj < -50){
  f*= 1.075 + 0.015*tj/10;   //unit of tj is 10*deg
 }

 // consider effect of ice fraction (unitless 0~1)
 //only consider the ice fraction in upper layer
 ice_scl = 1 - ice_frac[0];
 //equation below considers the ice fraction in both upper and lower layer
 //ice_scl = ((1-ice_frac[0]) * soil->whcs[0] + (1- ice_frac[1]) * soil->whcs[1]) / (soil->whcs[0] + soil->whcs[1]);

#ifdef PERMAFROST
 if(temp_wtd <= 0){  //make sure wetlanf fraction not exceed the maximum Fmax 
   temp_wtd = 0.0;
 }  
   wetfrac=fmax * ice_scl * exp(-1*cs*f*temp_wtd*1.5/2); //Scale wtd from 2 m in permafrost module to match the 1.5 m of origional LPJ soil depth
#else
 wetfrac=fmax * ice_scl * exp(-1*cs*f*temp_wtd);
#endif
 if(wetfrac < 0.0) wetfrac=0.0;
 
 return wetfrac;
}

Real ch4e(const Topmodel *topmodel,const Coord *coord,Real soiltemp,Real rh,Real wetfrac)
{
 Real emit_tropic,emit_temper;
 Real area;
 Real tropicness,mch4;
 tropicness=mch4=0.0;
   
 if(rh <=0 ) rh=0;
   
 //tropicness=exp((topmodel->tm - 30.2)/8);
 tropicness=exp((soiltemp-30.2)/8);
 area=111.13295*111.13295*0.5*0.5*cos(coord->lat*(M_PI/180))*(1000*1000);
 emit_tropic=wetfrac*area*rh*K_TROPICS;
 emit_temper=wetfrac*area*rh*K_TEMPERATE;
 mch4 =(emit_tropic*tropicness) + (emit_temper*(1-tropicness));
 if(mch4 <0) mch4 = 0.0;   //For pixels has Nodata wetfrac

 return mch4/area;
}



Real ch4o(const Topmodel *topmodel,const Coord *coord,Real soiltemp,Real rh,Real wetfrac)
{
  Real emit_tropic,emit_temper;
  Real tropicness,pch4;
  Real area;
  //if soilltemp = 0 ,there is no CH4 emission
  if(topmodel->tm <= -90 || soiltemp < 0)
    pch4=0.0;
  else{
    if(rh <=0 )
 
    rh=0;
    
    tropicness=exp((topmodel->tm - 30.2)/8);
 
    area=111.13295*111.13295*0.5*0.5*cos(coord->lat*(M_PI/180))*(1000*1000);
    emit_tropic=wetfrac*area*rh*1000*P_TROPICS*1e-12;
    emit_temper=wetfrac*area*rh*1000*P_TEMPERATE*1e-12;
    pch4 =(emit_tropic*tropicness) + (emit_temper*(1-tropicness));
  }
 return pch4;
}
