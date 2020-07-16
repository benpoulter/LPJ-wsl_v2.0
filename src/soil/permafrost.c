/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ p e r m a f r o s t . c                            **/
/*******************************************************************/
#include "lpj.h"
//#include "tree.h"
//#include "grass.h"
#include <math.h>

#define PAD_DEPTH 8000  //padding depth [mm]
#define DT 1            //time step [day]
#define DZ_LITTER 100.0 //depth of each litter layer [mm], Bonan Ecol. CLim., p. 188 [J m-3 K-1]
#define CAIR 1200       //heat capacity of air
#define CWATER 4180000  //heat capacity of water
#define CICE 1700000    //heat capacity of ice
//#define CICE 3117800  //heat capacity of ice, from Bartlett et al. 2004
#define CORG 3117800    //heat capacity of organic matter, Bonan Ecol. CLim., p. 188 [J m-3 K-1]
#define CMIN 2380000    //heat capacity of mineral soil, Bonan Ecol. CLim., p. 188 [J m-3 K-1]
#define CPEAT 580000    //heat capacity of dry peat, Bonan Ecol. CLim., p. 188 [J m-3 K-1]

#define KAIR 0.025      //thermal conductivity of air [W m-1 K-1]
#define KWATER 0.57     //thermal conductivity of water [W m-1 K-1]
#define KICE 2.2        //thermal conductivity of ice [W m-1 K-1]
#define KORG 0.25       //thermal conductivity of organic matter [W m-1 K-1]
#define KMIN 2.0        //thermal conductivity of mineral soil [W m-1 K-1]
#define KPEAT 0.06      //thermal conductivity of dry peat [W m-1 K-1]

#define LHEAT 3.34E8 //latent heat of fusion (Granberg)


void soiltempnew(Permafrost *permafrost,const Soil *soil,const Pftlist pftlist,int npft,Real temp,Real stemp,int d,int m,int year)
{
 int i,j,l;             //index
 int IDX, SIDX;         //indexes for first active layer of soil, litter and snow 
 int MIDX, AIDX;        //index for the mixed (litter and
                        //standing water in peatlands) and air layer
 int layer0, layer;
 int snow_active;       //number of active snow layers
 int mixedl;            //number of active mixed layers (should be 1)
 Real uice_total,uwater_total,lice_total,lwater_total; //ZZ temporary variables
 //Pfttreepar *treepar;
 //Pftgrasspar *grasspar;
 //Real bulkpar=0.0;

 //Real Kair,Kwater,Kice,Korg,Kmin,Ksnow,Kpeat;  //thermal conductivity [W m-1 K-1]
 Real lKorg,lKpeat,lKmin,lKwater,lKice,lKair;  //log value for K (thermal conductivity)
 Real T[NLAYERS];           //soil temperature in each layer [deg. C]
// Real Tmin[NLAYERS];        //current minimum T [deg. C] used to find freezing conditions
 Real delta_T[NLAYERS];     //temperature difference (current - previous time step[deg C]
 Real litterdepth;          //total litter depth [m]
 Real litter_ag_total;      //total aboveground litter summed over PFTs
 //Real bulk_dens[npft];      //bulk density of litter [kgC m-3]
 Real bulk_dens_ave;        //average bulk density of litter [kgC m-3]
 Real Ksnow;                //thermal conductivity [W m-1 K-1]
 Real Csnow;                //heat capacity of snow (J m-3 K-1)
 Real Dsnow;                //Diffusivity of snow [mm2 d-1]
 //Real snowpor;              //[m3 m-3]

 Real lKfwater;             //log of therm. cond. for the water fraction
 Real lKfice;               //log of therm. cond. for the ice fraction
 Real lKforg;               //log of therm. cond. for the org. fraction
 Real lKfpeat;              //log of therm. cond. for the peat fraction
 Real lKfmin;               //log of therm. cond. for the min. fraction
 //Real lKfair;               //log of therm. cond. for the air fraction  
 Real Ktotal[NLAYERS];      //total therm. cond.
 Real Ctotal[NLAYERS];      //total heat capacity

 Real Ftot;                 //total of solid fraction in soil
 Real por[NLAYERS];         //porosity of each soil layer
 Real Forg[NLAYERS];        //org. soil fraction
 Real Fpeat[NLAYERS];       //peat fraction
 Real Fmin[NLAYERS];        //mineral soil fraction

 Bool Freezing;             //Indicates if conditions for freezing are right
 Real Ffreez;               //fraction of water which freezes
 Real Fthaw;                //fraction of ice which thaws
 Real energy;               //Energy per volume [J m-3]

 //Define constant array
 const Real Dz_soil[NSOILL]={100,100,100,200,200,300,500,500}; //Soil later thickness [mm]
 //const Real soil_middepth[NSOILL]={-50,-150,-250,-400,-600,-850,-1250,-1750}; // The depth of the midpoint of each layer [mm]
 
 lKorg = log(KORG);
 lKpeat = log(KPEAT);
 lKmin = log(KMIN);
 lKwater = log(KWATER);
 lKice = log(KICE);
 //lKair = log(KAIR);

 fornlayer(l){
     Tmin[l]  = 0.0;
     delta_T[l] = 0.0;
     Ctotal[l] = CWATER;
 }

 //SET count for permafrost index calculation
 if (d==1){
   permafrost->day_count=0.0;
   permafrost->perma_count=0.0;
 }

 //Allocate yesterday's temperature to today's
 fornlayer(l){
   if (d ==1){
     permafrost->Tsoil[l] = permafrost->Tsoil_365[l];
   }else{
     permafrost->Tsoil[l] = permafrost->Tsoil_prev[l];
   }
   
 }

 //     LAYER SETUP
 //     SNOW LAYER
 //     Use one snow layer with varying depth
 snow_active = 1;
 snowpor = 1 - permafrost->snowdens / 1000.0;
 // Csnow from Fukusako, it's the same as for ice
 // // ---------------------------------------------
 Csnow = (0.185 + 0.689 * (273 + temp) * 0.01) * 1000 * permafrost->snowdens;
 // From Sturm et al. 1997, and in Ling and Zhang, 2006
 if(permafrost->snowdens > 156){
   Ksnow = 0.138 - 1.01 * permafrost->snowdens / 1000 + pow(3.233 * (permafrost->snowdens/1000.0),2);
 }else {
   //Original equation for fresh snow, from Wania
   //Ksnow = 0.023 + 0.234 * permafrost->snowdens / 1000.0;
   //ZZ to let the Ksnow match with thermal conductivity calculation of old snow
   //we modify the equation for fresh snow, which increase parameter Ksnow to match with densed snow
   Ksnow = 0.198 + 0.234 * permafrost->snowdens / 1000.0;
 }

 //ZZ adjust thermal diffusivity of snow when snowdepth larger than 350 mm
 //to avoid abnormal values for differential equation of soil temperature
 Dsnow = Ksnow / Csnow * 86400E6;
 //Using linear equation from 1.5*Dsnow at 350 mm snowdepth to 10*Dsnow at 4000 mm snowdepth
 if(permafrost->snowdepth > 350) Dsnow= Ksnow / Csnow * 86400E6 * (8.5/3650*permafrost->snowdepth+0.685);
 //Dsnow= Ksnow / Csnow * 86400E6 * (7/3650*permafrost->snowdepth+2.329);
 // LAYERS
 //     -----------------------------------------------------------------
 //     After 20 years, calculate the litter depth.
 //     Multiply bulk_dens(pft) by 1000 to get from [kgC/m3] to [gC/m3]
 //     For peatlands, assume that the litter gets incorporated into the
 //     peat instead of accumulating on top of the soil
 //     -----------------------------------------------------------------i
 if (permafrost->peathydr){
   litterdepth = 50;       //initialise litterdepth at the beginning 
 }else{
   litter_ag_total = 0.0;
   bulk_dens_ave = 0.0;
   
   //ZZ currently we use average bulk density for calculating litter depth
   //instead of PFT-specified bulk density calculation in LPJWHYMe
   if(litter_ag_total < 0){
     litter_ag_total = 0.0;
   }else{
     litter_ag_total = soil->litter.ag_tree + soil->litter.ag_grass;
   }

   bulk_dens_ave += 20*soil->litter.ag_tree/litter_ag_total*BIOM_TO_C*1000;
   bulk_dens_ave += 2*soil->litter.ag_grass/litter_ag_total*BIOM_TO_C*1000;

   //ZZ Constrain the average bulk density not too low, which could be happened in high latitude when there is no tree growing.
   if(bulk_dens_ave < 5000){
     bulk_dens_ave = 5000;
   }

   litterdepth = litter_ag_total / bulk_dens_ave * 1000; //[mm]
 }
 
 //Avoid having no litterdepth at all as this will lead to problems
 //as the layer thickness will be set to that value
 if (isnan(litterdepth) || litterdepth < 50){
     litterdepth = 50;
 }
  
  //MIXED LAYER
  mixedl = 1;
  //    All of the layers exist all of the time, the snow layer is either
  //    filled with snow or air if there is no snow.

  //    AIDX = air layer index
  //    SIDX = snow layer index
  //    MIDX = mixed layer index, i.e. litter and water (in peatlands)
  //    IDX = soil layers index
  //    ZZ Note: index start from 1
  //     -----------------------------------------------------------------
  IDX = NLAYERS - NSOILL +1;
  MIDX = NLAYERS - NSOILL - mixedl +1;
  SIDX = NLAYERS - NSOILL - mixedl -snow_active + 1;
  AIDX = NLAYERS - NSOILL - mixedl - snow_active - 1 + 1;

  //Set soil parameters for each soil layer
  j = 0;
  for (i=IDX-1;i<NLAYERS;i++){
    permafrost->Dz[i] = Dz_soil[j];
    j+=1;
  }

  //ALLOCATE values to various variables.  
  if (permafrost->peathydr){
    //ACROTELM
    for (i=IDX-1;i<NLAYERS - NCATOTELM;i++){
            por[i]   = ACROTELM_POR - FGAS;
            Forg[i]  = 0.0;
            Fmin[i]  = soil->par->min_con; //is set to 0
            Fpeat[i] = 1 - ACROTELM_POR;
	    permafrost->Fpwp[i] = soil->par->wat_hld;  //ZZ added
    }
    // CATOTELM
    for (i=IDX+NACROTELM-1;i<NLAYERS;i++){
            por[i]   = soil->par->porosity - FGAS;
            Forg[i]  = 0.0;
            Fmin[i] = soil->par->min_con;
            Fpeat[i] = soil->par->org_con;
	    permafrost->Fpwp[i] = soil->par->wat_hld; //ZZ added
    }
         // MIXED LAYER
         // consists of litter and water and/or ice
         // ---------------------------------------
         permafrost->Dz[MIDX-1] = litterdepth + permafrost->Fwater[MIDX-1] * MAXH + permafrost->Fice[MIDX-1] * MAXH;
         //-------------------------------------------------
         //multiply by 86400 to get from m2 s-1 to m2 d-1
         //multiply by 1E6 to get from m2 d-1 to mm2 d-1
         //-------------------------------------------------
         // Note: Di(MIDX) should take the water and ice content into
         // account, but when I included it into the loop below where Di
         // gets updated, the model produced NaN in a few places.
         permafrost->Di[MIDX-1] = KORG / CORG * 86400.0 * 1E6;
         Forg[MIDX-1] = litterdepth / permafrost->Dz[MIDX-1];
         por[MIDX-1] = 0.0; // as it's completely filled with litter, water or ice
         Fmin[MIDX-1] = 0.0;
         Fpeat[MIDX-1] = 0.0;
         permafrost->Fpwp[MIDX-1] = 0.0;
    

  }else{
         // NON-WETLAND SOILS
         // -----------------
         for (i=IDX-1;i<NLAYERS;i++){
            por[i]  = soil->par->porosity;
            Forg[i] = soil->par->org_con;
            Fmin[i] = max(0.0, soil->par->min_con);
          permafrost->Fpwp[i] = soil->par->wat_hld;
            Fpeat[i] = 0.0;
         }
         // MIXED LAYER
         // -----------
         permafrost->Dz[MIDX-1] = litterdepth;
         //-------------------------------------------------
         //multiply by 86400 to get from m2 s-1 to m2 d-1
         //multiply by 1E6 to get from m2 d-1 to mm2 d-1
         //-------------------------------------------------
         permafrost->Di[MIDX-1] = KORG / CORG * 86400.0 * 1E6;
         Forg[MIDX-1] = 1;
         por[MIDX-1] = 1.0 - Forg[MIDX-1];
         Fmin[MIDX-1] = 0.0;
         Fpeat[MIDX-1] = 0.0;
         permafrost->Fpwp[MIDX-1] = 0.0;
         permafrost->Fwater[MIDX-1] = 0.0; 
         permafrost->Fice[MIDX-1] = 0.0;
  }

/*    -----------------------------------------------------------------
c     Set thickness of 'empty' snow layer and air layer to a high value
c     to avoid numerical instabilities
     -----------------------------------------------------------------
*/
      // SNOW LAYER
      // ----------
      if (permafrost->snowdepth > 0){
         permafrost->Di[SIDX-1] = Dsnow;
         permafrost->Dz[SIDX-1] = permafrost->snowdepth;
      }else{ //set snow layer to air values
         permafrost->Di[SIDX-1] = KAIR / CAIR * 86400 * 1E6;
         permafrost->Dz[SIDX-1] = 1000;
      }
      permafrost->Fwater[SIDX-1] = 0.0;

      // AIR LAYER
      // ---------
      permafrost->Dz[AIDX-1] = 1000;
      por[AIDX-1] = 1;
      Forg[AIDX-1] = 0.0;
      Fmin[AIDX-1] = 0.0;
      permafrost->Fpwp[AIDX-1] = 0.0;
      Fpeat[AIDX-1] = 0.0;
      permafrost->Di[AIDX-1] = KAIR / CAIR * 86400 * 1E6;
      permafrost->Fwater[AIDX-1] = 0.0;


      //-------------
      //PADDING SETUP
      //-------------
      // Set up padding - use calc_padding to calculate the thickness
      // of the padding layers, then initialise the padding layer
      // temperatures to zero.
      if (permafrost->first){
         calc_padding(PAD_DEPTH, PAD_LAYERS, permafrost->Dz[NLAYERS-1], permafrost->pad_dz);
         for (layer=0;layer<PAD_LAYERS;layer++){
      permafrost->pad_temp[layer] = 0.0;
         }
      }

      //------------------------------------------
      //Allocate surface temperature to air layer
      //------------------------------------------
      layer0 = AIDX-1;

      //--------------------------------------------------------------------
      //Do one timestep of the Crank-Nicholson method.
      //--------------------------------------------------------------------
      for(i=AIDX-1;i<NLAYERS;i++){
         T[i] = permafrost->Tsoil[i];
      }

      cnstep(layer0, T, permafrost->Di, permafrost->Dz, temp,permafrost->pad_dz,permafrost->pad_temp,d,year);

      for(i = AIDX-1;i<NLAYERS;i++){
        permafrost->Tsoil[i] = T[i];
       }

//     --------------------------------------------------------------------
//     Calculate WATER AND ICE fraction in each layer
//     --------------------------------------------------------------------
//     Note: To know how much ice is formed or melted at each time step,
//     I first calculate the temperature change from the previous to this
//     time step. If not all of the available water is frozen, then the
//     temperature will be held at 0 deg C until everything is frozen (and
//     vice versa for the melting). You need to find the amount of heat
//     released when freezing or absorbed when thawing. To do this you
//     multiply the total heat capacity of the soil times delta_T. Since
//     we are working in m-3 here, we don't need to multiply by the volume
//     or mass, because we want to get the fraction of water freezing or
//     ice thawing, not the absolute value. 
//     --------------------------------------------------------------------

      //Run through layers
      for(i=IDX-1;i<NLAYERS;i++){
         // Subtract the non-freezable (water that does not freeze until
         // the temperature drops below -10degC) water from Fwater
	     permafrost->Fwater[i] = max(0.0, permafrost->Fwater[i] - permafrost->Fpwp[i]);
         
	     if(permafrost->first)
	       permafrost->T_old[i] = permafrost->Tsoil[i];

         delta_T[i] = permafrost->Tsoil[i] - permafrost->T_old[i];

         //ZZ fix the thermal instability caused when warm air temperatures allow rain but the soil is still frozen
         //which leads to inconsistent delta_T (e.g. some layer showing increasing temperature but air temperature is dropping)
         //Temporal solution for detecting thermal instability
         // set up threshold of deltaT > 30 degC to indicate the occurance of oscilliation
         if(permafrost->first==FALSE&&fabs(delta_T[i]) > 30){
             permafrost->isstable=FALSE;
         }
         
         
         // Determine if conditions are right for freezing
         // ----------------------------------------------
         // -----------------------------------------------------------
         // FREEZING
         // Freezing is only allowed under the following conditions:
         // 1. Tsoil is < zero
         // 2. Tsoil is < permafrost->T_old to ensure falling temperature
         // 3. Freezing can happen only during the second half of the
         // year, this avoids problems with refreezing in spring, which
         // we can't account for  ZZ:Dont know to How to determine whether
	     // it is the second half of the year?
	     // Plus, the condition Fwater > 0.0 is 
         // -----------------------------------------------------------

         if(permafrost->Tsoil[i] < 0 && delta_T[i] < 0.0 && permafrost->Fwater[i] > 0.0){
            Freezing = TRUE;
            Tmin[i] = permafrost->Tsoil[i];
         }else{
            Freezing = FALSE;
	     }
         
	     if(Freezing){
	        Fthaw = 0.0;
            if(permafrost->Fwater[i] > 0.0){
               energy = CWATER * fabs(delta_T[i]); //energy in J m-3
               Ffreez = energy / LHEAT; //unitless fraction

               if(permafrost->Fwater[i] > Ffreez){
                  permafrost->Fice[i] += Ffreez;
                  permafrost->Fwater[i] -= Ffreez;
                  if(permafrost->Tsoil[i] < -0.5 ){
                  
                  }else{
                    permafrost->Tsoil[i] = 0.0;
                  }
                  if(permafrost->Fwater[i] < 0.000001) permafrost->Fwater[i] = 0.0;
                  if(permafrost->Fice[i] < 0.000001) permafrost->Fice[i] = 0.0;                  
               }else{
               // ------------------------------------------------
               // It is cold enough to freeze all the water and
               // decrease the temperature below 0, since the rest
               // of the water gets frozen now.
               // ------------------------------------------------
               //printf("All water has been freezed, Fice=%f, Fwater=%f\n",permafrost->Fice[i],permafrost->Fwater[i]);
               permafrost->Fice[i] += permafrost->Fwater[i];
               permafrost->Tsoil[i] = -(Ffreez - permafrost->Fwater[i]) * LHEAT / CWATER;
               if(permafrost->Fice[i] < 0.000001) permafrost->Fice[i] = 0.0;                  
               permafrost->Fwater[i] = 0.0;
             }
           }else{
              Ffreez = 0.0;
           }

         // --------
         // THAWING
         // --------
         }else if(permafrost->Tsoil[i] >= 0.0){
            Ffreez = 0.0;
            if(permafrost->Fice[i] > 0.0){
              energy = CWATER * fabs(delta_T[i]); //energy in J m-3
              Fthaw = energy / LHEAT; //unitless fraction

              //printf("Thawing but Fice > 0 Fice[%d]=%f, Fthaw[%d]=%f\n",i,permafrost->Fice[i],i,Fthaw);
              if(permafrost->Fice[i] > Fthaw){
                 permafrost->Fwater[i] += Fthaw;
                 permafrost->Fice[i] -= Fthaw;
                 permafrost->Tsoil[i] = 0.0;
                 if(permafrost->Fice[i] < 0.000001) permafrost->Fice[i] = 0.0;
                 if(permafrost->Fwater[i] < 0.000001) permafrost->Fwater[i] = 0.0;
              }else{
               // ------------------------------------------------
               // There is more energy available than required for
               // melting the rest of the ice. Therefore the
               // left-over energy will increase the temperature.
               // ------------------------------------------------
               //

               if(permafrost->Fice[i] > Fthaw){
                  permafrost->Fwater[i] += Fthaw;
                  permafrost->Fice[i] -= Fthaw;
                  permafrost->Tsoil[i] = 0.0;
                  if(permafrost->Fice[i] < 0.000001) permafrost->Fice[i] = 0.0;
                  if(permafrost->Fwater[i] < 0.000001) permafrost->Fwater[i] = 0.0;
               }else{
                // ------------------------------------------------
                // There is more energy available than required for
                // melting the rest of the ice. Therefore the
                // left-over energy will increase the temperature.
                // ------------------------------------------------
                  permafrost->Fwater[i] = permafrost->Fwater[i] + permafrost->Fice[i];
                  permafrost->Tsoil[i] = (Fthaw - permafrost->Fice[i]) * LHEAT / CWATER;
                  permafrost->Fice[i] = 0.0;
                  if(permafrost->Fwater[i] < 0.000001) permafrost->Fwater[i] = 0.0;
               }
            }else{
               Fthaw = 0.0;
               permafrost->Fice[i] = 0.0;
            }
         }//..end freezing or thawing        
         
         permafrost->T_old[i] = permafrost->Tsoil[i];
 
         //if(permafrost->Tsoil[i] >= 0.0) Tmin[i] = 0.0;

         // Add the non-freezable water to Fwater again
         //---------------------------------------------
         permafrost->Fwater[i] = permafrost->Fwater[i] + permafrost->Fpwp[i];
         //-------------------------------------------------------------------
         //Update HEAT CAPACITY for each layer
         //-------------------------------------------------------------------

         if(i == MIDX-1){
            permafrost->Fair[i] = 0.0;
         }else{
            permafrost->Fair[i] = max(0.0,FGAS + por[i] - permafrost->Fwater[i] - permafrost->Fice[i]);
         }

         Ctotal[i] = Fmin[i] * CMIN + Forg[i] * CORG + permafrost->Fice[i] * CICE+ permafrost->Fwater[i] * CWATER + Fpeat[i] * CPEAT + permafrost->Fair[i] * CAIR;

         //-------------------------------------------------------------------
         //Update the THERMAL CONDUCTIVITY K - which follows
         //Granberg et al. 1999, who refers to Farouki, 1986
         //-------------------------------------------------------------------
         Ftot = permafrost->Fwater[i] + permafrost->Fice[i] + Forg[i] + Fmin[i] + Fpeat[i];

         // Computational faster way to do the calculations of Ktotal:
         //------------------------------------------------------------
         lKforg = (Forg[i] / Ftot) * lKorg;
         lKfpeat = (Fpeat[i] / Ftot) * lKpeat;
         lKfmin = (Fmin[i] / Ftot) * lKmin;
         lKfwater = ((permafrost->Fwater[i] + permafrost->Fpwp[i])/ Ftot) * lKwater;
         lKfice = (permafrost->Fice[i] / Ftot) * lKice;
         lKfair = (permafrost->Fair[i] / Ftot) * lKair;
         Ktotal[i] = permafrost->Fair[i] * KAIR + Ftot * exp(lKfwater + lKfice + lKforg + lKfmin + lKfpeat);


         //-------------------------------------------------------------------
         //Update THERMAL DIFFUSIVITY for each SOIL layer
         //-------------------------------------------------------------------
         //multiply by 86400 to get from m2 s-1 to m2 d-1
         //multiply by 1E6 to get from m2 d-1 to mm2 d-1
         //-------------------------------------------------
         permafrost->Di[i] = Ktotal[i] / Ctotal[i] * 86400.0 * 1E6;

         //----------------------------------------------------------------
         //Ice_frac is for output purposes only
         //----------------------------------------------------------------
         if(permafrost->Fice[i] > 0.01){
            permafrost->ice_frac[i] = permafrost->Fice[i] / (permafrost->Fice[i] + permafrost->Fwater[i]);
            //ERROR message:
            //--------------
           if(permafrost->ice_frac[i] > 1.0){
              printf("ERROR: ice fraction %d, %f > 1", i, permafrost->ice_frac[i]);
              printf("year: %d,d: %d,Fice: %f,Fwater: %f",year, d, permafrost->Fice[i], permafrost->Fwater[i]);
            }
         }else{
            permafrost->ice_frac[i] = 0.0;
         }
         
      
      }

      if(permafrost->isstable ==FALSE){
          for(i=IDX-1;i<NLAYERS;i++){
              permafrost->Tsoil[i]=stemp;
              permafrost->T_old[i] = permafrost->Tsoil[i];
          }
          for(i=0;i<PAD_LAYERS;i++){
              permafrost->pad_temp[i] = stemp;
          }
      
      }
      
      uice_total = 0.0;
      lice_total = 0.0;
      uwater_total = 0.0;
      lwater_total = 0.0;
      
      //Calculate ice fraction for upper and lower layer, which is needed by TOPMODEL
      for(i=IDX-1;i<NLAYERS;i++){
	     if(i<7){
	         uice_total += permafrost->Fice[i];
	         uwater_total += permafrost->Fwater[i] - permafrost->Fpwp[i]; //exclude wilting point content
	     }else{
	         lice_total += permafrost->Fice[i];
	         lwater_total += permafrost->Fwater[i] - permafrost->Fpwp[i]; //exclude wilting point content
	     }
      }
      
      if(uice_total == 0.0){
        permafrost->ice_frac1 = 0.0;
      }else{
        permafrost->ice_frac1 = uice_total / (uice_total + uwater_total);
      }

      if(lice_total == 0.0){
        permafrost->ice_frac2 = 0.0;
      }else{
        permafrost->ice_frac2 = lice_total / (lice_total + lwater_total);
      }

         //----------------------------------------------------------------
         //SOIL TEMPERATURE for output purposes.
         //----------------------------------------------------------------
         permafrost->Tsoil_0   = permafrost->Tsoil[MIDX-1] * (permafrost->Dz[MIDX-1] / (permafrost->Dz[MIDX-1] + permafrost->Dz[IDX-1])) + permafrost->Tsoil[IDX-1] * (permafrost->Dz[IDX-1] / (permafrost->Dz[MIDX-1] + permafrost->Dz[IDX-1]));

         permafrost->Tsoil_5 = permafrost->Tsoil[IDX-1];

         permafrost->Tsoil_10  = permafrost->Tsoil[IDX-1] * (permafrost->Dz[IDX-1] / (permafrost->Dz[IDX-1] + permafrost->Dz[IDX]))+ permafrost->Tsoil[IDX] * (permafrost->Dz[IDX] / (permafrost->Dz[IDX-1] + permafrost->Dz[IDX]));

         permafrost->Tsoil_15 = permafrost->Tsoil[IDX];

         permafrost->Tsoil_20  = permafrost->Tsoil[IDX] * (permafrost->Dz[IDX]/(permafrost->Dz[IDX]+permafrost->Dz[IDX+1]))+ permafrost->Tsoil[IDX+1] * (permafrost->Dz[IDX+1]/(permafrost->Dz[IDX] + permafrost->Dz[IDX + 1]));

         permafrost->Tsoil_25  = permafrost->Tsoil[IDX+1];

         permafrost->Tsoil_30 = permafrost->Tsoil[IDX+1] * (permafrost->Dz[IDX+1]/(permafrost->Dz[IDX+1]+permafrost->Dz[IDX+2]))+ permafrost->Tsoil[IDX+2] * (permafrost->Dz[IDX+2]/       (permafrost->Dz[IDX+1] + permafrost->Dz[IDX + 2]));

         permafrost->Tsoil_40 = permafrost->Tsoil[IDX+2];

         permafrost->Tsoil_50  = permafrost->Tsoil[IDX+2]*(permafrost->Dz[IDX+2]/(permafrost->Dz[IDX+2]+permafrost->Dz[IDX+3]))+ permafrost->Tsoil[IDX+3]*(permafrost->Dz[IDX+3]/(permafrost->Dz[IDX+2]+permafrost->Dz[IDX+3]));

         permafrost->Tsoil_60 = permafrost->Tsoil[IDX+3];

         permafrost->Tsoil_70 = permafrost->Tsoil[IDX+3]*(permafrost->Dz[IDX+3]/(permafrost->Dz[IDX+3]+permafrost->Dz[IDX+4]))+ permafrost->Tsoil[IDX+4]*(permafrost->Dz[IDX+4]/     (permafrost->Dz[IDX+3]+permafrost->Dz[IDX+4]));

         permafrost->Tsoil_85 = permafrost->Tsoil[IDX+4];

         permafrost->Tsoil_100 = permafrost->Tsoil[IDX+4]*(permafrost->Dz[IDX+4]/(permafrost->Dz[IDX+4]+permafrost->Dz[IDX+5]))+ permafrost->Tsoil[IDX+5]*(permafrost->Dz[IDX+5]/(permafrost->Dz[IDX+4]+permafrost->Dz[IDX+5]));

         permafrost->Tsoil_125 = permafrost->Tsoil[IDX+5];

         permafrost->Tsoil_150 = permafrost->Tsoil[IDX+5]*(permafrost->Dz[IDX+5]/(permafrost->Dz[IDX+5]+permafrost->Dz[IDX+6]))+ permafrost->Tsoil[IDX+6]*(permafrost->Dz[IDX+6]/(permafrost->Dz[IDX+5]+permafrost->Dz[IDX+6]));

         permafrost->Tsoil_175 = permafrost->Tsoil[IDX+6];

         permafrost->Tsoil_200 = permafrost->Tsoil[IDX+6]*(permafrost->Dz[IDX+6]/(permafrost->Dz[IDX+6]+permafrost->pad_dz[0]))+permafrost->pad_temp[0]*(permafrost->pad_dz[0]/(permafrost->Dz[IDX+6]+permafrost->pad_dz[0]));

         permafrost->Tsoil_top = permafrost->Tsoil[IDX-1] * (permafrost->Dz[IDX-1]/ (permafrost->Dz[IDX-1] + permafrost->Dz[IDX] + permafrost->Dz[IDX+1] + permafrost->Dz[IDX+2])) + permafrost->Tsoil[IDX] * (permafrost->Dz[IDX]/ (permafrost->Dz[IDX-1] + permafrost->Dz[IDX] + permafrost->Dz[IDX+1] + permafrost->Dz[IDX+2])) + permafrost->Tsoil[IDX+1] * (permafrost->Dz[IDX+1]/ (permafrost->Dz[IDX-1] + permafrost->Dz[IDX] + permafrost->Dz[IDX+1] + permafrost->Dz[IDX+2])) + permafrost->Tsoil[IDX+2] * (permafrost->Dz[IDX+2]/ (permafrost->Dz[IDX-1] + permafrost->Dz[IDX] + permafrost->Dz[IDX+1] + permafrost->Dz[IDX+2]));

         permafrost->Tsoil_bot = permafrost->Tsoil[IDX+3] * (permafrost->Dz[IDX+3]/ (permafrost->Dz[IDX+3] + permafrost->Dz[IDX+4] + permafrost->Dz[IDX+5] + permafrost->Dz[IDX+6])) + permafrost->Tsoil[IDX+4] * (permafrost->Dz[IDX+4]/ (permafrost->Dz[IDX+3] + permafrost->Dz[IDX+4] + permafrost->Dz[IDX+5] + permafrost->Dz[IDX+6])) + permafrost->Tsoil[IDX+5] * (permafrost->Dz[IDX+5]/ (permafrost->Dz[IDX+3] + permafrost->Dz[IDX+4] + permafrost->Dz[IDX+5] + permafrost->Dz[IDX+6])) + permafrost->Tsoil[IDX+6] * (permafrost->Dz[IDX+6]/ (permafrost->Dz[IDX+3] + permafrost->Dz[IDX+4] + permafrost->Dz[IDX+5] + permafrost->Dz[IDX+6]));

         //------------------------------------------------------------------
         //THAW DEPTH: Define the thaw depth as the 0degC isotherm (Muller
         //1947, Burn 1998), which equals the depth, down to which liquid
         //water exists continuously from the top down.
         //The thaw depth is also defined as the 0degC isotherm.
         //------------------------------------------------------------------
         permafrost->thawdepth = 0.0;
         i = IDX-1; 
         do{
             permafrost->thawdepth = permafrost->thawdepth + permafrost->Dz[i];
             i += 1;
         }
         while(i < NLAYERS && permafrost->Tsoil[i] >= 0.0);
         permafrost->thawdepth = min(2000.0, permafrost->thawdepth);

         //     Calculate also at which depth there is no ice anymore. This could
         //     be used in the waterbalance SR to estimate drainage or
         //     runoff. (Not implemented at the moment.)

         permafrost->thaw_top = 0.0;
         i = IDX-1;
         do{
             permafrost->thaw_top += permafrost->Dz[i];
             i = i + 1;
         }
         while(i < NLAYERS && permafrost->Fice[i] < 0.0001);

         //     Check if each soil layer thaws at least for one day of the
         //     year. This is needed to know if roots will occur in this layer or
         //     not. This information is used in the methane SR.
         for(i=IDX-1;i<NLAYERS;i++){
             if(permafrost->Fwater[i] <= permafrost->Fpwp[i] && permafrost->Fice[i] > 0.001){
                 permafrost->frozen_day[i] = TRUE;
             }else{
                 permafrost->frozen_day[i] = FALSE;
             }
         }

         if(d == 365){
             for(i=0;i<NLAYERS;i++){
                 permafrost->Tsoil_365[i] = permafrost->Tsoil[i];
                 permafrost->Fice_365[i] = permafrost->Fice[i];
                 permafrost->Fwater_365[i] = permafrost->Fwater[i];
             }
         }else{
             for(i=0;i<NLAYERS;i++){
                 permafrost->Tsoil_prev[i] = permafrost->Tsoil[i];
                 permafrost->Fice_prev[i] = permafrost->Fice[i];
                 permafrost->Fwater_prev[i] = permafrost->Fwater[i];
             }
         }

      permafrost->first = FALSE;
 
  return;

} /* of 'soiltemp' */


void calc_padding(Real pad_depth,int pad_layers,Real dz,Real pad_dz[PAD_LAYERS])
{
  Real func,dfunc;  // Function, derivative and parameter for geometric factor calcualtion
  Real k_guess,k_last;  // Iterative improvements of geometric factor
  int layer;  //Layer counter

  //Initial state - don't choose 1.0 as the initial value for l_guess, since it's a trivial solution!
  k_guess = 10.0;
  k_last = 0.0;

  //Do Newton-Raphson solve to find geometric sequence factor.
  do{
    //Statement functions for function and derivative for Newton solver for
    //geometric sequence factor
    func = pow(k_guess, PAD_LAYERS+1) - k_guess * (1.0 + pad_depth / dz) + pad_depth/ dz;
    dfunc = (PAD_LAYERS +1) * pow(k_guess, PAD_LAYERS) - (1.0 + PAD_DEPTH /dz);
    k_last = k_guess;
    k_guess = k_guess - func/dfunc;
  }while(fabs(k_guess - k_last) > 0.01);

  for(layer=0;layer<pad_layers;layer++){
    pad_dz[layer] = dz * k_guess * (layer+1);
  }

  return;

} /* of calc_padding*/

//Crank-Nicholson timestepper for temperature diffusion equation.
void cnstep( int layer0, Real temp[NLAYERS],Real Di[NLAYERS],Real dz[NLAYERS],Real surf_temp,Real pad_dz[PAD_LAYERS],Real pad_temp[PAD_LAYERS],int d,int year)
{
  int layer, lidx, active_layers;
  //Diffusion constants averaged over adjacent layers.
  Real dplus, dminus;
  //Layer-dependent weighting factors in Crank-Nicholson scheme.
  Real dz_factor, Cplus, Cminus;
  Real dzhere, dzminus, dzplus;
  Real there, tminus, tplus;

  //Leading diagonal, left and right subdiagonals for Crank-Nicholson matrix.
  Real diag[NLAYERS - layer0 +PAD_LAYERS];
  Real left[NLAYERS - layer0 + PAD_LAYERS];
  Real right[NLAYERS - layer0 + PAD_LAYERS];

  //Right hand side vector for Crank-Nicholson scheme equations.
  Real rhs[NLAYERS - layer0 + PAD_LAYERS];

  //Solution vector for Crank-Nicholson scheme equations.
  Real solution[NLAYERS - layer0 +PAD_LAYERS];

  // --- CODE STARTS HERE ---
  // Not all layers in the input arrays are actually used.  Calculate
  // how many layers we have to deal with here.
  
  //Initialize vectors
  for(lidx=0;lidx < (NLAYERS+PAD_LAYERS);lidx++){
    diag[lidx]=left[lidx]=right[lidx]=rhs[lidx]=solution[lidx]=0.0;
  }
  

  active_layers = NLAYERS - layer0 + PAD_LAYERS;
  //BUILD TRIDIAGONAL MATRIX AND KNOWN RIGHT HAND SIDE
  //End members for off-diagonal elements.
  left[0] = 0.0;
  right[active_layers - 1] = 0.0;

  //Process the active layers.
  for(lidx =0;lidx<active_layers;lidx++){
    layer = lidx + layer0 -1 +1; //ZZ

    //Calculate diffusion constants averaged over adjacent layers.
    //The diffusion constant at the bottom layer is clamped to zero
    //to enforce the no heat flow boundary condition there.  Diffusion
    //constants in the padding layers are calculated in a sensible way.
    if(layer == (NLAYERS+PAD_LAYERS -1)){
      dplus = 0.0;
    }else if(layer >= NLAYERS - 1){
      dplus = Di[NLAYERS - 1];
    }else{
      dplus = 0.5 * (Di[layer] + Di[layer+1]);
    }

    if(layer == layer0){
      dminus = Di[layer];
    }else if(layer > (NLAYERS-1)){
      dminus = Di[NLAYERS-1];
    }else{
      dminus = 0.5 * (Di[layer] + Di[layer-1]);
    }

    // Extract sensible values to use for temperature and diffusivity
    // of the current layer, the layer above and the layer below,
    // taking account of padding layers and end cases.
    if(layer <= NLAYERS-1){
      dzhere = dz[layer];
      there = temp[layer];
    }else{
      dzhere = pad_dz[layer - NLAYERS];
      there = pad_temp[layer - NLAYERS];
    }

    if(layer <= (NLAYERS-1-1)){
      dzplus = dz[layer + 1];
      tplus = temp[layer + 1];
    }else if (layer < (NLAYERS + PAD_LAYERS-1)){
      dzplus = pad_dz[layer - NLAYERS+1]; 
      tplus = pad_temp[layer - NLAYERS+1]; 
    }else{
      dzplus = pad_dz[PAD_LAYERS-1];
      tplus = pad_temp[PAD_LAYERS-1];
    }

    if(layer == layer0){ 
      dzminus = dz[layer];
      tminus = temp[layer];
    }else if(layer <= NLAYERS){
      dzminus = dz[layer-1];
      tminus = temp[layer-1];
    }else{
      dzminus = pad_dz[layer - NLAYERS - 1];
      tminus = pad_temp[layer - NLAYERS - 1];
    }

    //Calculate layer-dependent weighting factors.
    dz_factor = 0.25 * (dzplus+2*dzhere+dzminus);
    Cplus = dplus * DT / dz_factor/(dzplus+dzhere);
    Cminus = dminus * DT / dz_factor / (dzhere +dzminus);

    //Fill in matrix diagonal and off-diagonal elements.
    if(lidx == 0){
      diag[lidx] = 1.0;
    }else{
      diag[lidx] = 1.0 + Cplus + Cminus;
    }

    if(lidx > 0){
      left[lidx] = -Cminus;
    }

    if(lidx < active_layers-1){
      if(lidx > 0){
        right[lidx] = -Cplus;
      }else{
        right[lidx] = 0.0;
      }
    }

    //Calculate right hand side vector values.
    if(lidx ==0){
      rhs[lidx] = surf_temp;
    }else if(lidx == (active_layers-1)){
      rhs[lidx] = (1.0 - Cminus)* there + Cminus *tminus;
    }else{
      rhs[lidx] = (1.0 - Cplus - Cminus) * there + Cplus * tplus + Cminus * tminus;
    }
  }


  //------------------------------------------------------------
  //SOLVE TRIDIAGONAL SYSTEM
  tridag(active_layers,left,diag,right,rhs,solution);
  
  for(lidx =0;lidx < NLAYERS;lidx++){
    if(lidx < layer0){
      temp[lidx] = MISSING_VALUE;
    }else{
      temp[lidx] = solution[lidx - layer0 ];
    }
  }
  
  for(lidx =0;lidx < PAD_LAYERS;lidx++){
    pad_temp[lidx] = solution[lidx - layer0 + NLAYERS]; //ZZ
  }
  
  return;

}/* cnstep*/

void tridag(int n,Real a[],Real b[],Real c[],Real r[],Real u[])
{
  Real gam[n];
  Real bet;
  int j;

  if(b[0] ==0.0){
    printf("ERROR in tridag: degenerate system!");
    return;
  }

  bet = b[0];
  u[0] = r[0] / bet;
  for (j = 1;j< n;j++){
    gam[j] = c[j-1] / bet;
    bet = b[j] - a[j] * gam[j];
    if(bet == 0.0){
      printf("ERROR in tridag: zero pivot!");
      return;
    }
    u[j] = (r[j] - a[j] * u[j - 1]) / bet;
  }

  for(j = n-1-1; j>0;j--){
    u[j] = u[j] - gam[j+1] * u[j+1];
  }
  return;

}/* end of tridag*/
