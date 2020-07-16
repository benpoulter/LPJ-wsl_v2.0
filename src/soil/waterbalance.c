/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ w a t e r b a l a n c e . c                        **/
/*******************************************************************/
#include "lpj.h"
#include "permafrost.h"

#define SOILDEPTH_EVAP 200.0  /* depth of sublayer at top of upper soil layer,
                                 from which evaporation is possible*/
#define BASEFLOW_FRAC 0.5     /* Fraction of standard percolation amount from
                                 lower soil layer*/
#define K_DEPTH 0.4 
#define K_AET 0.52            /* Fraction of total (vegetation) AET from upper
                                 soil layer*/
#define PRIESTLEY_TAYLOR 1.32

//below define parameter for permafrost module
#define pt 1.32        //Priestley-Taylor coefficient
#define alpham 1.391   //Priestley-Taylor coefficient (Demand)
#define gm 3.26        //empirical parameter in demand function
#define Dz_sph 10      //depth of each sphagnum layer [mm]
#define surfw 0.34     //surface water content when WTD is at 20 mm
#define minvtot 0.25   //minimum water content at surface in mm3/mm3 look up Haywood and Clymo 1982
#define zmin 100.0     //limit where water content at surface = minuw1 (mm)
#define grad_lim 50.0  //gradient limit of upper gradient (mm)


Real waterbalance(Soil *soil,                /* Pointer to soil */
                  Real aet_stand[NSOILLAYER],
                  Real *evap,
                  Real rainmelt,             /* rainfall + melting water + irrigation water (mm) */
                  Real pet,
                  Real cover,
                  Real lat,
                  Real lon,
                  int landusetype
                 )                           /* returns water runoff */
{
  Real runoff;
  Real perc,perc_frac,perc_baseflow;
  Real aet_evap;
  Real influx;
  Real SOILDEPTH_UPPER, K_AET_DEPTH; //SOILDEPTH
  int l;

  SOILDEPTH_UPPER=soil->soildepth[0]; //SOILDEPTH - calculate dynamically
  K_AET_DEPTH=((SOILDEPTH_UPPER/SOILDEPTH_EVAP-1.0)*(K_AET/K_DEPTH-1.0)/(1.0/K_DEPTH-1.0)+1.0); //soildepth - calculate dynamically
  /* Weighting coefficient for AET flux from evaporation layer, assuming active
  *   root density decreases with soil depth
  *   Equates to 1.3 given SOILDEPTH_EVAP=200 mm, SOILDEPTH_UPPER=500 mm,
  *   K_DEPTH=0.4, K_AET=0.52 
  */
  //printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\n",soil->par->k1, soil->par->k2, soil->par->tdiff_0, soil->par->tdiff_15, soil->par->tdiff_100, soil->par->whc[0],soil->par->whc[1]);

  runoff=0;
  if (cover>=1.0) cover=0.999;
  /* aet_evap=(aet_stand[0]*SOILDEPTH_EVAP*K_AET_DEPTH/SOILDEPTH_UPPER); */
  aet_evap=(aet_stand[0]*K_AET_DEPTH/SOILDEPTH_UPPER);
  *evap=pet*PRIESTLEY_TAYLOR*soil->w_evap*soil->w_evap*(1-cover);
  //soil->w[0]+=(rainmelt-aet_stand[0]-*evap)/soil->par->whcs[0];
  //printf("%f\t%f\n",soil->w[0],soil->w_evap);
  
  soil->w[0]+=(rainmelt-aet_stand[0]-*evap)/soil->whcs[0];//SOILDEPTH  - change soil structure for whcs
  if(soil->w[0]<0.0){
    soil->w[0]=0.0;
    printf("WARNING:%f %f negative soil->w %f\n",lat,lon,soil->w[0]); 
  }
  /*printf("evap=%g cover=%g pet=%g w=%g\n",*evap,cover,pet,soil->w_evap);*/
  #ifdef SAFE
    if (soil->w[0]<0.0){
      printf("rainmelt= %3.2f aet= %3.2f evap=  %3.2f cover=  %3.2f \n",rainmelt,aet_stand[0],*evap,cover);
      fail("Pixel: %f %f Soil-moisture 1 negative: %g, lutype %d\n",lat,lon,soil->w[0],landusetype);
    }
  #endif

  /*printf("rainmelt= %3.2f aet= %3.2f evap= %3.2f cover= %3.2f aet_evap= %3.2f w_evap= %3.2f w1= %3.2f\n",
   rainmelt,aet_stand[0],*evap,cover,aet_evap,soil->w_evap,soil->w[0]);*/

  if (soil->w[0]>1.0) 
  {
    //Surface runoff
    runoff=(soil->w[0]-1.0)*soil->whcs[0]; 
    //runoff=(soil->w[0]-1.0)*soil->par->whcs[0];
    soil->w[0]=1.0;
  }

  influx=rainmelt;
  

  /* Update water content in evaporation layer for tomorrow */
  soil->w_evap+=((rainmelt-*evap-aet_evap)/(soil->par->whc[0]*SOILDEPTH_EVAP));
  
  if (soil->w_evap>1.0) soil->w_evap=1.0;
  if (soil->w_evap<0.0) soil->w_evap=0.0;

  /* Percolation from evaporation layer */
  if (influx>0.1){
    perc=SOILDEPTH_EVAP/soil->soildepth[0]*soil->par->k1* 
         pow(soil->w_evap,soil->par->k2);
    if(perc>influx)
      perc=influx;
    soil->w_evap-=perc/(soil->par->whc[0]*SOILDEPTH_EVAP);
  }

  /* Percolation and fluxes to and from lower soil layer(s)*/
  for (l=1;l<NSOILLAYER;l++) 
  {
    soil->w[l]-=aet_stand[l]/soil->whcs[l];
    #ifdef SAFE 
      if (soil->w[l]<0.0)       
       fail("Soil-moisture negative:%f %f %5.2f Soillayer: %4.3f\n",lat,lon,soil->w[l],l);
    #endif
   
    /* Percolation: Allow only on days with rain or snowmelt */
    if (influx>=0.1){
      perc=soil->par->k1*pow(soil->w[l-1],soil->par->k2);
      if(perc>influx)
        perc=influx;
      perc_frac=perc/soil->whcs[l-1];
      if(perc_frac>soil->w[l-1])
        perc_frac=soil->w[l-1];
      soil->w[l-1]-=perc_frac;
      soil->w[l]+=perc_frac*soil->whcs[l-1]/soil->whcs[l];
      influx-=perc;
    }
 
    if (soil->w[l]>1.0) 
    {
      runoff+=(soil->w[l]-1.0)*soil->whcs[l]; 
      soil->w[l]=1.0;
    }

  }   

  /* Baseflow runoff (rain or snowmelt days only) */
  if (influx>=0.1) {
    perc_baseflow=BASEFLOW_FRAC*soil->par->k1*pow(soil->w[BOTTOMLAYER],
                  soil->par->k2);
    if (perc_baseflow>influx) 
      perc_baseflow=influx;
    perc_frac=perc_baseflow/soil->whcs[BOTTOMLAYER];
    if(perc_frac>soil->w[BOTTOMLAYER])
      perc_frac=soil->w[BOTTOMLAYER];
    soil->w[BOTTOMLAYER]-=perc_frac;
    runoff+=perc_frac*soil->whcs[BOTTOMLAYER];
    //printf("baseflow runoff:mswc2=%f,influx=%f,perc_frac=%f\n",soil->w[1],influx,perc_frac);
  }
  soil->meanw1+=soil->w[0];

  if ((soil->w_evap*soil->par->whc[0]*SOILDEPTH_EVAP)>(soil->w[0]*soil->whcs[0])) 
    soil->w_evap=min(1.0,soil->w[0]*soil->soildepth[0]/SOILDEPTH_EVAP);
  if (soil->w_evap<0)
    soil->w_evap=0;

  return runoff;
  
} /* of 'waterbalance' */

//##############################################################################################################
//new waterbalance subroutine for 8 layers
Real waterbalancenew(
                  Permafrost *permafrost,
              Soil *soil,                /* Pointer to soil */
              const Pftlist pftlist,
              Real aet_stand[NSOILLAYER],
                  Real *evap,
                  Real rainmelt,             /* rainfall + melting water + irrigation water (mm) */
                  Real pet,
                  Real cover,
                  Real lat,
                  Real lon,
              int npft,
              int year, 
              int d,  //day
                  int landusetype
                 )                           /* returns water runoff */
{
  Real runoff;
  Real perc,perc_frac,perc_baseflow;
  Real aet_evap;
  Real influx;
  Real SOILDEPTH_UPPER, K_AET_DEPTH; 
  int  i,l;
  int IDX,LIDX;
  Pft *pft;

  //New variables for permafrost below
  Real wtot_lo;    //Total water content in lower soil layer [mm]
  Real wtot_up;    //Total water content in upper soil layer [mm]
  Real ice_lo;     //Total ice content in lower soil layer   [mm]
  Real ice_up;     //Total ice content in upper soil layer   [mm]
  Real awhc[NLAYERS];  // Actual WHC for all the layers [mm]
  Real rfroz;      //runoff caused by frozen ice in upper 200 mm soil
  Real fpc_moss_total;   //needed to calculate daep from mosses
  Real fpc_grid[npft];  //gridcell foliar projective cover (FPC)

  //LAYER SETUP
  //IDX = top of soil layers
  //MIDX = a mixed layer on top of soil layer that consists of litter
  //       air and in peatlands also of water.
  //LIDX = top of lower soil layers (500-1500mm)  
  IDX = NLAYERS - NSOILL +1 - 1; //index = 3, the first soil layer
  //MIDX = IDX - 1 ;
  LIDX = IDX + 4 ;

  //Initialization local variables
  wtot_lo=wtot_up=0.0;
  ice_lo=ice_up=0.0;
  rfroz=fpc_moss_total=0.0;

  //Every time step need to reset AWHC to zero
  soil->awhcs[0]=soil->awhcs[1]=0.0;
  
  //Allocate the ice content according to the changes from the Soiltemp SR
  //Wtot is the water content excluding ice fraction Unit:mm
  for(i=IDX;i<NLAYERS;i++){
    //the order in LPJ-WSL and LPJ-WHYME is different, we calculate soiltemp first,then hydroloigcal processes
      permafrost->wtot[i] = (permafrost->Fwater[i]- permafrost->Fpwp[i]) * permafrost->Dz[i];
  }
  
  //Calculate the total water content in upper and lower layer
  for(i=IDX;i<LIDX;i++){
    wtot_up+=permafrost->wtot[i];
    ice_up+=permafrost->Fice[i] * permafrost->Dz[i];
  }
  for(i=LIDX;i<NLAYERS;i++){
    wtot_lo+=permafrost->wtot[i];
    ice_lo+=permafrost->Fice[i] * permafrost->Dz[i];
  }

  //Reduce WHC for each layers by ice fraction AWHC unit: mm
  for(i=IDX;i<NLAYERS;i++){
    if(i<LIDX){
      awhc[i] = soil->whcs[0]/soil->soildepth[0] * permafrost->Dz[i] - permafrost->Fice[i] * permafrost->Dz[i];    
    }else{
      awhc[i] = soil->whcs[1]/soil->soildepth[1] * permafrost->Dz[i] - permafrost->Fice[i] * permafrost->Dz[i];
    }
    if(awhc[i] < 0.0001) awhc[i] =0.0;
  }
    
  //Calculate Actual WHC for upper and lower layers
  for(i=IDX;i<LIDX;i++){
    soil->awhcs[0]+=awhc[i];
  }

  for(i=LIDX;i<NLAYERS;i++){
    soil->awhcs[1]+=awhc[i];
  }
    
  runoff=0;

  if (cover>=1.0) cover=0.999;

  //reserved for peatland implementation
  if(permafrost->peathydr){
    //=======================================================================
    //HYDROLOGY SEPARATION ---starts here---
    //Peatland soils:  
    //Leave space for peatland hydrology module
    //======================================================================= 
   
    }else{
    //=======================================================================
    //HYDROLOGY SEPARATION ---starts here---
    //Mineral soils:   
    //=======================================================================    
    
    // Calculate SOIL WATER components
    //pre-calcuation
    SOILDEPTH_UPPER=soil->soildepth[0]; //calculate dynamically
    K_AET_DEPTH=((SOILDEPTH_UPPER/SOILDEPTH_EVAP-1.0)*(K_AET/K_DEPTH-1.0)/(1.0/K_DEPTH-1.0)+1.0); //soildepth - calculate dynamically 
    
    //aet_evap = ETup, transpiration from the upper 200mm soil layer
    aet_evap=(aet_stand[0]*K_AET_DEPTH/SOILDEPTH_UPPER);
    
    //*evap is the daily bare soil evaporation
    *evap=pet*PRIESTLEY_TAYLOR*soil->w_evap*soil->w_evap*(1-cover);
    
    //consider increase runoff from the reduction of liquid water holding capacity with the top 100 mm
    //of soil caused by presence of ice in the soil by scaling the potential infiltration
    //So that surface runoff will occur even though soil moisture does not reach the maximum (>=1)
    //allow water infiltration happen even when ice fraction reaches maximum (1) 
    //Ringeval et al., 2012 & Poutou et al., 2004
    rfroz = rainmelt * ice_up/soil->whcs[0];
    rainmelt -= rfroz;
    runoff   += rfroz;
    
    //soil->w is daily soil moisture
    soil->w[0]+=(rainmelt-aet_stand[0]-*evap)/soil->whcs[0];
  
    if(soil->w[0]<0.0){
    soil->w[0]=0.0;
    printf("WARNING:lat:%f lon:%f negative soil->w %f\n",lat,lon,soil->w[0]); 
    }

#ifdef SAFE
 if (soil->w[0]<0.0){
   printf("rainmelt= %3.2f aet= %3.2f evap=  %3.2f cover=  %3.2f \n",rainmelt,aet_stand[0],*evap,cover);
   fail("Pixel: %f %f Soil-moisture 1 negative: %g, lutype %d\n",lat,lon,soil->w[0],landusetype);
 }
#endif

    if (soil->w[0]>1.0) 
    {
      /* Surface runoff */
      runoff+=(soil->w[0]-1.0)*soil->whcs[0];
      soil->w[0]=1.0;
    }
    
    //rainmelt equals preciptation + melt
    influx=rainmelt;
  
     /* Update water content in evaporation layer for tomorrow */
     //Unitless
    soil->w_evap+=((rainmelt-*evap-aet_evap)/(soil->par->whc[0]*SOILDEPTH_EVAP));

    if (soil->w_evap>1.0) soil->w_evap=1.0;
    if (soil->w_evap<0.0) soil->w_evap=0.0;

     /* Percolation from evaporation layer */
    if (influx>0.1)
    {
      //Calculate percolation Rprec
      perc=SOILDEPTH_EVAP/soil->soildepth[0]*soil->par->k1*  
           pow(soil->w_evap,soil->par->k2);
      if(perc>influx)
        perc=influx;
      soil->w_evap-=perc/(soil->par->whc[0]*SOILDEPTH_EVAP);
    }


    /* Percolation and fluxes to and from lower soil layer(s)*/
    for (l=1;l<NSOILLAYER;l++) 
    {
      soil->w[l]-=aet_stand[l]/soil->whcs[l];
  #ifdef SAFE 
      if (soil->w[l]<0.0)       
       fail("Soil-moisture negative:%f %f %5.2f Soillayer: %4.3f\n",lat,lon,soil->w[l],l);
  #endif
   
      /* Percolation: Allow only on days with rain or snowmelt */
      if (influx>=0.1)
      {
        perc=soil->par->k1*pow(soil->w[l-1],soil->par->k2);
        if(perc>influx) perc=influx;
        perc_frac=perc/soil->whcs[l-1];
        if(perc_frac>soil->w[l-1]) perc_frac=soil->w[l-1];
        soil->w[l-1]-=perc_frac;
        soil->w[l]+=perc_frac*soil->whcs[l-1]/soil->whcs[l];
        influx-=perc;
      }
 
      if (soil->w[l]>1.0) 
      {
        runoff+=(soil->w[l]-1.0)*soil->whcs[l];
        soil->w[l]=1.0;
      }
    }

    /* Baseflow runoff (rain or snowmelt days only) */
    if (influx>=0.1) 
    {
      perc_baseflow=BASEFLOW_FRAC*soil->par->k1*pow(soil->w[BOTTOMLAYER],
                  soil->par->k2);
      if (perc_baseflow>influx) 
        perc_baseflow=influx;
      perc_frac=perc_baseflow/soil->whcs[BOTTOMLAYER];
      if(perc_frac>soil->w[BOTTOMLAYER])
        perc_frac=soil->w[BOTTOMLAYER];
      soil->w[BOTTOMLAYER]-=perc_frac;
      runoff+=perc_frac*soil->whcs[BOTTOMLAYER];
    }
    soil->meanw1+=soil->w[0];

    if ((soil->w_evap*soil->par->whc[0]*SOILDEPTH_EVAP)>(soil->w[0]*soil->whcs[0]))
      soil->w_evap=min(1.0,soil->w[0]*soil->soildepth[0]/SOILDEPTH_EVAP);
    if (soil->w_evap<0)
      soil->w_evap=0;
  }
  
  //Update water content for upper and lower layer
  wtot_up = soil->w[0] * soil->par->whc[0] * DEPTH_U - ice_up;
  wtot_lo = soil->w[1] * soil->par->whc[1] * DEPTH_L- ice_lo;

  //Allocate water content for each permafrost layers
  //the current strategy is allocating water content equally
  for(i=IDX;i<LIDX;i++){
     permafrost->wtot[i] = wtot_up * permafrost->Dz[i] / DEPTH_U * soil->par->whc[0]; 
     if(permafrost->wtot[i] < 0.0){
       permafrost->wtot[i] = 0.0;
     }
      permafrost->Fwater[i] = (permafrost->wtot[i] / (permafrost->Dz[i]/DEPTH_U * soil->par->whc[0])) + permafrost->Fpwp[i];
  }   
  for(i=LIDX;i<NLAYERS;i++){
     permafrost->wtot[i] = wtot_lo * permafrost->Dz[i] / DEPTH_L * soil->par->whc[1]; 
     if(permafrost->wtot[i] < 0.0){
       permafrost->wtot[i] = 0.0;
     }
      permafrost->Fwater[i] = (permafrost->wtot[i] / (permafrost->Dz[i]/DEPTH_L * soil->par->whc[1])) + permafrost->Fpwp[i];
  }

  return runoff;
  
} /* of 'waterbalancenew' */
