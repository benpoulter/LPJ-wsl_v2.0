/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / p e r m a f r o s t . h                     **/
/*******************************************************************/

#ifndef PERMAFROST_H /* Already included? */
#define PERMAFROST_H

#include "soil.h"
#include "pft.h"
/* Definition of constants */

#define NPFTPAR 43       //number of PFT parameter (count from 0)
#define NSOILL 8         //Soil layer thickness [mm]
#define NLAYER1 4
#define NLAYER2 4
#define DEPTH_U 500.0    //depth of upper buckets (mm)
#define DEPTH_L 1500.0   //depth of lower buckets (mm)
#define DEPTH_EV 200.0   //depth of evaporation soil layer (mm)
#define NLAYER_EV 2      //number of layers in evaporation zone
#define PAD_LAYERS 4     //number of padding layers

//For peatland
#define Z_ACRO 300.0     //max depth of acrotelm, taken from Granberg
#define Z_CATO 1700.0    //max depth of catotelm, taken from Granberg
#define NLAYERS 11       //number of layers incl. soil and snow
#define MAXH 11          //max height of standing water (mm)

#define NACROTELM 3      //number of acrotelm layers
#define DZ_ACRO 100.0    //depth of each acrotelm layer (mm)
#define ACROTELM_POR 0.9 //acrotelm porosity
#define NCATOTELM 5      //number of catotelm layers
#define DZ_CATO 100.0    //depth of each catotelm layer (mm)
#define FGAS 0.08        //gas fraction in peat
#define BIOM_TO_C 0.45   //convert g biomass to g C

#define MISSING_VALUE -99999;

//ZZheng below have not been updated
/* Definition of datatypes */
/*typedef struct
{
  Bool frozen[NLAYERS];  //annual indicator for pahse of water
  Real uw1; //water content in soil layer 1 [mm]
  Real uw2; //water content in soil layer 2 [mm]
  Real uw_ep; //daily water content in evap layer [mm]
  Real theta; //fractional water content of top layer for comparison with observations [-]
  Real Total_water[365][NLAYERS]; //water + ice content [m3] used in SR Methane
  Real w[2]; //soil layer 1 and 2 water content(fraction of available water holding capacity)
  Real wtp; //daily water table position [mm]
  Real mw1[12]; //monthly values w[1], fraction avail. water
  Real mw2[12]; //monthly values w[2], fraction avail. water
  Real ice1; //ice content in upper soil layer [mm]
  Real ice2; //ice content in lower soil layer [mm]
  Real mice_25[12]; //monthly fraction of H2O in from of ice in 25 cm depth
  Real mice_50[12]; //monthly fraction of H2O in form of ice in 50 cm depth
  Real mice_100[12]; //monthly fraction of H2O in form of ice in 100 cm depth
  Real mice_200[12]; //monthly fraction of H2O in form of ice in 100 cm depth
  Real arunoff;
  Real mrunoff[12]; //total monthly runoff [mm]
  Real drunoff_surf; //daily surface runoff [mm]
  Real drunoff_drain; //daily drainage runoff [mm]

  Real dmelt; //daily snowmelt [mm]
  Real aaet; //annual actual evapotranspiration [mm/year]
  Real aaep; //annual actual evaporation [mm/year]
  Real aaept; //annual actual evapotranspiration [mm/year]- in wetlands, the evap. and tranp. is calculated together
  Real maep[12]; //monthly actual evaporation [mm]
  Real maet[12]; //monthly actual transpiration [mm]
  Real maept[12]; //monthly actual evapotranspiration [mm/year] in wetlands, the evap. and trans. is calculated together
  Real daep; //daily evaporation [mm]
  Real daet; //daily transpiration [mm]
  Real daept; //daily evapotranspiration [mm] - in wetlands, the evap. and trans. is calcualted together
  Real intercp_tot;

  Real inund_height[NPFT]; //max WTD toerated
  Real inund_dura[NPFT]; //max inundation duration tolerated
} Hydrovars;

typedef struct
{
  Real mtemp_max; //warmest-month temperature [deg C]
  Real mtemp_max20; //20-year average maximum monthly
  Real mtemp_min20; //20-year average minimum monthly temp. [deg C]
  Real mtemp_old[12]; //last year's monthly temp. [deg C]
  Real atsoil_25; //annual temperature in 25 cm depth [deg C]

}Thermvars;
*/

typedef struct
{
  Bool all_melted;           //indicator if there is still some snow
  int snow_days_prev;        //number of days with snow cover for last year
  int snow_days;             //number of days with snow cover
  Real swe;                  //snow water quivalent [mm = kg m -2]
  Real snowdens;             //daily snow density in [kg m-3]
  Real snowdepth;            //daily snow depth in [mm]
  Bool frozen_day[NLAYERS];  //daily indicator for phase of water
  Real Dz[NLAYERS];          //  [mm] 
  Real Di[NLAYERS];          //  [mm2 / day]
  Real pad_dz[PAD_LAYERS];   //  [mm]
  Real pad_temp[PAD_LAYERS]; //  [deg C]

  Real thawdepth;            //daily thaw depth [mm]
  Real thaw_top;             //ZZhang ?
  int day_count;             //count the years for the permafrost index
  int perma_count;           //count the years, where soil is not fully thawed
  
  Real Tsoil_365[NLAYERS];   // soil temperatures on last day [deg. C] 
  Real Tsoil[NLAYERS];       //daily soil Tsoil [deg C]
  Real T_old[NLAYERS];       //soil temperature from previous time step [deg C]
  Real Tsoil_prev[NLAYERS];  //yesterday soil Tsoil [deg C]
  
  Real Fice[NLAYERS];        //daily ice fraction in each layer
  Real Fice_365[NLAYERS];    //ice fraction in each layer on last day of year
  Real Fice_prev[NLAYERS];   //ice fraction in each layer on yesterday
  Real ice_frac[NLAYERS];    //fraction of ice in each layer OUTPUT
  Real ice_frac1;            //fraction of ice in upper layer
  Real ice_frac2;            //fraction of ice in lower layer

  Real Fwater[NLAYERS];      //daily water fraction in each layer
  Real Fwater_365[NLAYERS];  //water fraction in each layer on last day of year
  Real Fwater_prev[NLAYERS]; //yesterday soil water fraction
  Real Fpwp[NLAYERS];        //water held below the wilting point,this is kept liquild down to -10 deg C
  Real Fair[NLAYERS];        //air fraction

  Real Tsoil_0;              //daily soil temperature in 0 cm depth
  Real Tsoil_5;              //daily soil temperature in 5 cm depth
  Real Tsoil_10;             //daily soil temperature in 10 cm depth
  Real Tsoil_15;             //daily soil temperature in 15 cm depth
  Real Tsoil_20;             //daily soil temperature in 20 cm depth
  Real Tsoil_25;             //daily soil temperature in 25 cm depth
  Real Tsoil_30;             //daily soil temperature in 30 cm depth
  Real Tsoil_40;             //daily soil temperature in 40 cm depth
  Real Tsoil_50;             //daily soil temperature in 50 cm depth
  Real Tsoil_60;             //daily soil temperature in 60 cm depth
  Real Tsoil_70;             //daily soil temperature in 70 cm depth
  Real Tsoil_85;             //daily soil temperature in 85 cm depth
  Real Tsoil_100;            //daily soil temperature in 100 cm depth
  Real Tsoil_125;            //daily soil temperature in 125 cm depth
  Real Tsoil_150;            //daily soil temperature in 150 cm depth
  Real Tsoil_175;            //daily soil temperature in 175 cm depth
  Real Tsoil_200;            //daily soil temperature in 200 cm depth

  Real Tsoil_top;            //daily soil temperature in 0-50 cm depth (weighted averge)
  Real Tsoil_bot;            //daily soil temperature in 50-200 cm depth (weighted averge)

  Real wtot[NLAYERS];        //Total water content in each layer [mm]
  
  Bool peathydr;
  Real permafrost_index;
  Bool first;                //needed for standing water initialisation
  Bool isstable;             //for detecting whether the thermal state is stable
} Permafrost;

extern void soiltempnew(Permafrost *permafrost,const Soil *soil,const Pftlist pftlist,int npft,Real temp,Real stemp,int d, int m, int year);
extern void calc_padding(Real pad_depth,int pad_layers,Real dz,Real pad_dz[PAD_LAYERS]);
extern void cnstep(int layer0,Real T[NLAYERS],Real Di[NLAYERS],Real Dz[NLAYERS],Real surf_temp,Real pad_dz[PAD_LAYERS],Real pad_temp[PAD_LAYERS],int d,int year);
extern void tridag(int n,Real a[],Real b[],Real c[],Real r[],Real u[]);
extern void snownew(Real dtemp, Real dprec, Real snowmelt, Permafrost *permafrost, int year, int day);
extern void initpermafrost(Permafrost *permafrost, Soil *soil, float oc_top,float tsl);
extern Bool freadpermafrost(FILE *file,Permafrost *permafrost,const Soilpar *soilpar,Bool swap);
extern Bool fwritepermafrost(FILE *file,const Permafrost *permafrost,Bool full);
extern Real waterbalancenew(Permafrost *permafrost,Soil *,const Pftlist pftlist,Real [NSOILLAYER],Real *,Real,Real,Real,Real,Real,int,int,int,int);
/* Definition of macros */
#define fornlayer(l) for(l=0;l<NLAYERS;l++)

#endif
