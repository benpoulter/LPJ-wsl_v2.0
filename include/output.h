/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / o u t p u t . h                             **/
/*******************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

/* Definition of datatypes */

typedef struct
{
  Real mnpp;            // monthly net primary productivity NPP, (g C/m2/month), total for all tiles   
  Real mrh;             // monthly heterotrophic respiration, (g C/m2/month), total for all tiles 
  Real mtransp;         // monthly transpiration, (mm/month), total for all tiles 
  Real mrunoff;         // monthly runoff, (mm/month), total for all tiles 
  Real mdischarge;      // monthly discharge, (mm/month), total for all tiles  
  Real mevap;           // monthly evaporation, (mm/month), total for all tiles 
  Real minterc;         // monthly interception, (mm/month), total for all tiles 
  Real mswc[NSOILLAYER];// monthly soil water content, (fraction of water holding capacity)
  Real firec;           // annual fire carbon emissions, (g C/m2/yr), total for all tiles
  Real firef;           // annual fire fraction, (fraction of grid cell)
  Real flux_estab;      // annual establishment flux, (g C/m2/yr), total for all tiles 
  Real flux_harvest;    // annual harvest flux, (g C/m2/yr), total for all tiles 
  Real mirrig_wd;       // monthly irrigation water withdrawal, (mm/month), total for all tiles
  Real mirrig;          // monthly irrigation water reaching the field, (mm/month), total for all tiles
  Real aevap_lake;      // annual evaporation from water surfaces of lakes and reservoirs, (mm/yr), total for all tiles 
  int *sdate;           // sowing date, (days index from first day at 0 )       
  Real *pft_npp;        // annual pft specific NPP, (g C/m2/yr) 
  Real *pft_harvest;    // cft specific harvest of storage organs, (g C/m2/yr) 
  Real *pft_rharvest;   // cft specific harvest of plant residues, (g C/m2/yr) 
  Real *fpc;            // foliar projective cover FPC, (fraction) 
  Real *pft_gcgp;       // annual ratio of actual to potential stomatal conductance x pft, (mol H20/m2/yr)
  Real *gcgp_count;     // water stress ([0,1])
  Real *pft_fo3uptake;  // annual uptake of ozone x pft (mmol O3/m2/yr)
  Real *pft_transp;     // pft specific annual transpiration, (mm/yr) 
  Real *pft_gc;         // annual pft stomatal conductance (mol H2O/m2/yr)
  Real *pft_lai;        // pft specific leaf area index LAI (m2/m2)
  Real *pft_gpp;        // annual pft specific gross primary productivity GPP, (g C/m2/yr) 
  Real *pft_vegc;       // annual pft specific vegitation carbon, (g C/m2/yr)
  Real *pft_nind;       // density of PFT individuals (number/m2)
  Real *pft_mort;       // pft specific mortality, (number/m2) 
  Real *pft_maxphenday; // pft day of maximum phenology (day of year)
  Real *pft_bimonfpar;  // ?
  Real mgpp;            // monthly gross primary productivity GPP, (g C/m2/month) 
  Real msoiltemp;       // monthly soil temperature, (deg C/month)
  Real mpar;            // monthly PAR (J/m2/day)
  Real mpet;            // monthly potential evapotranspiration PET, (mm/month) 
  Real mra;             // monthly autotrophic respiration (g C/m2/month)
  Real flux_luc;        // annual flux from land use conversion, (g C/m2/month) 
  Real msnowpack;       // monthly snowpack depth, (mm/month)
  Real *mpft_lai;       // pft specific monthly LAI (m2/m2)
  Real *mpft_gc;        // monthly pft stomatal conductance (mol H2O/m2/yr)
  Real *mpft_ci;        // monthly pft internal CO2 concentration (ppmv)
  Real *mpft_transp;    // pft specific monthly transpiration, (g C/m2/month)
  Real *mpft_gpp;       // pft specific monthly GPP, (g C/m2/month) 

  //TOPMODEL outputs    
  Real wtd;             // water table depth, (m)
  Real wetfrac;         // wetland areal fraction, (fraction of total grid cell)
  Real mch4e;           // monthly CH4 emissions from wetlands, (g CH4/m2/month)
  Real ch4o;            // CH4 sinks by soils, (g CH4/m2/month)

  //PERMAFROST outputs
  Real mtsoil_0;        // monthly soil temperature in 0 cm depth, (deg C/month)
  Real mtsoil_10;       // monthly soil temperature in 10 cm depth, (deg C/month)
  Real mtsoil_20;       // monthly soil temperature in 20 cm depth, (deg C/month)
  Real mtsoil_25;       // monthly soil temperature in 25 cm depth, (deg C/month)
  Real mtsoil_50;       // monthly soil temperature in 50 cm depth, (deg C/month)
  Real mtsoil_70;       // monthly soil temperature in 70 cm depth, (deg C/month)
  Real mtsoil_100;      // monthly soil temperature in 100 cm depth, (deg C/month)
  Real mtsoil_150;      // monthly soil temperature in 150 cm depth, (deg C/month)
  Real mtsoil_200;      // monthly soil temperature in 200 cm depth, (deg C/month)
  Real mtsoil_top;      // monthly soil temperature in upper layer [0-50 cm], (deg C/month)
  Real mthaw_depth;     // monthly thaw depth, (mm/month)
  Real mFwater_10;      // monthly water fraction in layer [0-10 cm] (fraction/month)
  Real mFwater_20;      // monthly water fraction in layer [10-20 cm] (fraction/month)
  Real mFwater_30;      // monthly water fraction in layer [20-30 cm] (fraction/month)
  Real mFwater_50;      // monthly water fraction in layer [30-50 cm] (fraction/month)
  Real mFwater_70;      // monthly water fraction in layer [50-70 cm] (fraction/month)
  Real mFwater_100;     // monthly water fraction in layer [70-100 cm] (fraction/month)
  Real mFwater_150;     // monthly water fraction in layer [100-150 cm] (fraction/month)
  Real mFwater_200;     // monthly water fraction in layer [150-200 cm] (fraction/month)
  Real mFice_10;        // monthly ice fraction in layer [0-10 cm] (fraction/month)
  Real mFice_20;         // monthly ice fraction in layer [10-20 cm] (fraction/month)
  Real mFice_30;         // monthly ice fraction in layer [20-30 cm] (fraction/month)
  Real mFice_50;         // monthly ice fraction in layer [30-50 cm] (fraction/month)
  Real mFice_70;         // monthly ice fraction in layer [50-70 cm] (fraction/month)
  Real mFice_100;        // monthly ice fraction in layer [70-100 cm] (fraction/month)
  Real mFice_150;        // monthly ice fraction in layer [100-150 cm] (fraction/month)
  Real mFice_200;        // monthly ice fraction in layer [150-200 cm] (fraction/month)
  Real frozen_days;     // monthly frozen_days for upper layer [0-50 cm] (days/month)

  //BVOC variables
  Real misopr;          //monthly Isoprene Production 
  Real mmonopr;         //monthly Monoterpene Porduction

  //climate variables
  Real mtair;           // monthly mean air temperature, (deg C/month)
  Real mppt;            // monthly precipitation, (mm/month)
  Real mwetdays;        // monthly wetdays, (days/month))
  Real mswdown;         // monthly mean downward shortwave radiation, (W/m2/s)
  Real mlwdown;         // monthly mean downward longwave radiation, (W/m2/s)
  Real mcld;            // monthly mean cloud cover, (percent/month)
  Real mco2conc;        // monthly mean CO2 concentration, (ppmv/month)
  
  //daily output
  Real dwet_frac;       // daily wetland fraction (fraction)
  Real dch4e;           // daily CH4 emissions from wetlands, (g CH4/m2/day)
  Real dgpp;            // daily GPP, (g C/m2/day) 
  Real dnpp;            // daily NPP, (g C/m2/day) 
  Real drh;             // daily heterotrophic respiration, (g C/m2/day) 
  Real dFwater_10;      // daily water fraction in layer [0-10 cm] (fraction/day)
  Real dFwater_20;      // daily water fraction in layer [10-20 cm] (fraction/day)
  Real dFwater_30;      // daily water fraction in layer [20-30 cm] (fraction/day)
  Real dFwater_50;      // daily water fraction in layer [30-50 cm] (fraction/day)
  Real dFwater_70;      // daily water fraction in layer [50-70 cm] (fraction/day)
  Real dFwater_100;     // daily water fraction in layer [70-100 cm] (fraction/day)
  Real dFwater_150;     // daily water fraction in layer [100-150 cm] (fraction/day)
  Real dFwater_200;     // daily water fraction in layer [150-200 cm] (fraction/day)
  Real dFice_10;        // daily ice fraction in layer [0-10 cm] (fraction/day)
  Real dFice_20;        // daily ice fraction in layer [10-20 cm] (fraction/day)
  Real dFice_30;        // daily ice fraction in layer [20-30 cm] (fraction/day)
  Real dFice_50;        // daily ice fraction in layer [30-50 cm] (fraction/day)
  Real dFice_70;        // daily ice fraction in layer [50-70 cm] (fraction/day)
  Real dFice_100;       // daily ice fraction in layer [70-100 cm] (fraction/day)
  Real dFice_150;       // daily ice fraction in layer [100-150 cm] (fraction/day)
  Real dFice_200;       // daily ice fraction in layer [150-200 cm] (fraction/day)
  Real dtair;           //daily air temperature (degC)
  Real dppt;            //daily total precipitation (mm/day)
  Real dswdown;         //daily income shortwave radiation (W/m2)
  Real dlwdown;         //daily downward longwave raidiation (W/m2)
  Real dcld;            //daily cloud cover (%)

  //tracking primary,secondary vegitation variables
  Real mnpp_primary;                          // primary veg monthly NPP, (g C/m2/month)  
  Real mrh_primary;                           // primary veg monthly heterotrophic respiration, (g C/m2/month) 
  Real mnpp_secforest;                        // secondary veg monthly NPP, (g C/m2/month)
  Real mrh_secforest;                         // secondary veg monthly heterotrophic respiration, (g C/m2/month)
  Real mgpp_primary;                          // primary veg monthly GPP, (g C/m2/month)
  Real mgpp_secforest;                        // secondary veg montly GPP, (g C/m2/month)
  Real flux_luc_primary;                      // deforestation flux, (g C/m2/yr)
  Real flux_luc_secforest;                    // secondary forests land use change flux, (g C/m2/yr)

  //tracking wood harvest products, emitted and remaining, (g C/m2)
  //..60% wood to product pool 40% wood to litter, all root and leaf biomass to litter
  //..wood product pools distributed into 1,10,100 year pools based on LUH_V2 (spatially-explicit; ie, by grid-cell)
  Real woodharvest_1yr;                       // flux of carbon from 1 year wood harvest product pool (g C/m2/yr)
  Real woodharvest_10yr;                      // flux of carbon from 10 year wood harvest product pool (g C/m2/yr)
  Real woodharvest_100yr;                     // flux of carbon from 100 year wood harvest product pool (g C/m2/yr)
  Real woodharvest_1yr_remain;                // carbon remaining in 1 year wood harvest product pool (g C/m2/yr)
  Real woodharvest_10yr_remain;               // carbon remaining in 10 year wood harvest product pool (g C/m2/yr)
  Real woodharvest_100yr_remain;              // carbon remaining in 100 year wood harvest product pool (g C/m2/yr)
  //tracking deforestation products, emitted and remaining, (g C/m2)
  //..62.5% of deforest wood mass goes into product pool 37.5% of wood goes to immediate atmospheric flux
  //..all root and leaf biomass to litter
  //..based on LPX implementation
  //..of wood going to product pools, 60% into 2yr pool, 40% in 25yr pool
  Real deforestProduct_25yr;                  // flux of carbon from 25 year deforestation product pool (g C/m2/yr), ? % assumption,
  Real deforestProduct_2yr;                   // flux of carbon from 2 year deforestation product pool (g C/m2/yr), ? % assumption,
  Real deforestProduct_25yr_remain;           // carbon remaining in 25 year deforestation product pool (g C/m2/yr), ? % assumption,
  Real deforestProduct_2yr_remain;            // carbon remaining in 2 year deforestation product pool (g C/m2/yr), ? % assumption,

 #ifdef OUTPUT_BYSTAND
  Real frac_primary;                          // stand area as fraction of grid-cell area (fraction [0,1])  
  Real *fpc_primary;                           // foliar projective cover FPC per pft, corrected for stand area (fraction [0,1])
  Real flux_estab_primary;                    // establishment flux of primary vegetation, (g C/m2/yr)
  Real firec_primary;                         // primary vegitation loss from fire, (g C/m2/yr)

  //tracking wood harvest products from primary vegitation, emitted and remaining, (g C/m2)
  Real woodharvest_1yr_primary;               // see above
  Real woodharvest_10yr_primary;              // see above
  Real woodharvest_100yr_primary;             // see above
  Real woodharvest_1yr_remain_primary;        // see above
  Real woodharvest_10yr_remain_primary;       // see above
  Real woodharvest_100yr_remain_primary;      // see above
  //tracking deforest  products from primary vegitation, emitted and remaining, (g C/m2)
  Real deforestProduct_25yr_primary;          // see above
  Real deforestProduct_2yr_primary;           // see above
  Real deforestProduct_25yr_remain_primary;   // see above
  Real deforestProduct_2yr_remain_primary;    // see above

  Real frac_secforest;                        // stand area as fraction of grid-cell area (fraction [0,1])  
  Real *fpc_secforest;                        // foliar projective cover FPC per pft, corrected for stand area (fraction [0,1])
  Real flux_estab_secforest;                  // establishment flux of secondarty vegitation, (g C/m2/yr)
  Real firec_secforest;                       // secondary vegitation loss from fire, (g C/m2/yr)
  
  //tracking wood harvest and deforestation products from secondary vegetation, emitted and remaining, (g C/m2)
  Real woodharvest_1yr_secforest;             // see above
  Real woodharvest_10yr_secforest;            // see above
  Real woodharvest_100yr_secforest;           // see above
  Real woodharvest_1yr_remain_secforest;      // see above
  Real woodharvest_10yr_remain_secforest;     // see above
  Real woodharvest_100yr_remain_secforest;    // see above
  Real deforestProduct_25yr_secforest;        // see above
  Real deforestProduct_2yr_secforest;         // see above
  Real deforestProduct_25yr_remain_secforest; // see above
  Real deforestProduct_2yr_remain_secforest;  // see above
 #endif

 #ifdef OUTPUT_BYAGECLASS
  //primary vegitation outputs
  Real ageclass_pri_firec[12];                // veg loss from fire, (g C/m2/yr) 
  Real ageclass_pri_mnpp[12];                 // monthly NPP, (g C/m2/month)
  Real ageclass_pri_mrh[12];                  // monthly hetotrophic respiration, (g C/m2/month) 
  Real ageclass_pri_fluxestab[12];            // establishment flux, (g C/m2/yr)
  //written in output.c: 
  //..ageclass_pri_pftfpc[(NPFT+1)*12];       // foliar projective cover FPC per pft by ageclass (fraction [0,1])
  //..ageclass_pri_frac[12] = {0};            // stand area as fraction of grid-cell area by ageclass (fraction [0,1]) 

  //secondary vegitation outputs  
  Real ageclass_sec_firec[12];                // veg loss from fire, (g C/m2/yr)      
  Real ageclass_sec_mnpp[12];                 // monthly NPP, (g C/m2/month)       
  Real ageclass_sec_mrh[12];                  // monthly heterotrophic respiration, (g C/m2/month)    
  Real ageclass_sec_fluxestab[12];            // establishment flux, (g C/m2/yr) 
  //written in output.c: 
  //..ageclass_sec_pftfpc[(NPFT+1)*12];       // foliar projective cover FPC per pft by ageclass, (fraction [0,1])
  //..ageclass_sec_frac[12] = {0};            // stand area as fraction of grid-cell area by ageclass (fraction [0,1]) 
 #endif

 #ifdef OUTPUT_AGRICULTURE_MPFT
  //crop pfts only
  //..memory allocation in newgrid.c
  Real *mpft_ag_npp;                          // CFT and managed grass lands specific monthly NPP, (g C/m2/month)
  Real *mpft_ag_gpp;                          // CFT and managed grass lands specific monthly GPP, (g C/m2/month)        
  Real *mpft_ag_lai;                          // CFT and managed grass lands specific monthly LAI, (m2/m2)
  Real *mpft_ag_transp;                       // CFT and managed grass lands specific monthly transpiration, (g C/m2/month)
 #endif  
} Output;

/* Declaration of functions */

extern FILE **fopenoutput(Config);
extern void fcloseoutput(FILE **,int);
extern void initoutput_annual(Output *,int,int);
extern void initoutput_monthly(Output *, int); 
extern void initoutput_daily(Output *, int); 
extern void freeoutput(Output *);

#endif

