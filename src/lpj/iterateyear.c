/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ i t e r a t e y e a r . c                           **/
/*******************************************************************/
#include "lpj.h"


void iterateyear(FILE **output,
     Cell *grid,
     Climate *climate,
     Landuse *landuse,
     Real co2,
     const Pftpar *pftpar,
     int npft,
     int ncft,
     int ntypes,
     Config config,
     int year)
{
  Real temp,prec,sun,swdown,lwdown; 
  Real ozone=0.0; 
  Real minund=0.0;                  //inundation
  int month,dayofmonth,day;
  int cell;
  Real delta_totc;
  Real mtemp[config.ngridcell];
  
  #ifdef DAILY
  int month_mtemp, dayofmonth_mtemp; //needed for phenology mtemp
  month_mtemp=dayofmonth_mtemp=0;
  #endif
  
  #ifdef OZONE
  int o,ozoneYear,nOzoneYear, endYear, baseYear; 
  Real ozone1, ozone2, ozoneRate,*ozoneTmp, ozone1all[12], ozone2all[12]; 
  #endif

  for(cell=0;cell<config.ngridcell;cell++){
    initoutput_annual(&((grid+cell)->output),npft,ncft);
    init_annual(grid+cell,npft);
    if(!(grid+cell)->skip){
      laimax_manage((grid+cell)->laimax,(grid+cell)->manage.par->laimax_tempcer,(grid+cell)->manage.par->laimax_maize,pftpar,npft,ncft,year);  
      (grid+cell)->woodproducts_temp=0.0; //reset each year, for carbon balance of woodharvest

      #if defined(AGECLASS_SECFOREST)
        //set bool to false
        //..gets updated in LUC routine if secforest ageclasses transition
        //..secforest ageclasses required to transition if LUC occurs to satisfy mixing rules
        //..bool flag avoids transitioning ageclasses twice in a year
        (grid+cell)->transitioned_ageclass_secforest=FALSE;
      #endif

      //-------------------------------------------//
      //       L A N D - U S E  C H A N G E        //
      //-------------------------------------------//
      #if defined(GROSS_LUC) || defined(NETinGROSS)
        #if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
          //call LUC with ageclasses
          landusechange_AGECLASS_GROSS(grid+cell,pftpar,npft,ncft,ntypes,cell,landuse,year,config);
        #else
          //for use with gross LUC & netHybrid (reduced landuse transitions)
          landusechange_GROSS(grid+cell,pftpar,npft,ncft,ntypes,cell,landuse,year,config);
        #endif
      #else
        //landusechange() is deprecated
        //.. use landusechange_NET for 'original' formulation
        landusechange_NET(grid+cell,pftpar,npft,ncft,ntypes,cell,landuse,year,config);   
      #endif//end of GROSS_LUC..

      //-------------------------------------------//
      //  A G E C L A S S - T R A N S I T I O N S  //
      //-------------------------------------------//
      //     WORKHORSE FOR THE TILING APPROACH     //
      //-------------------------------------------//
      //..ageclasses created by LUC & fire
      //..
      //..transition the ageclass if they exists;
      //.. .. (below) accounts for ageclass creation due to LUC and Fire
      //..skips if no primary or secforset stand to transition
      //..skips also if ageclasses transitions in the LUC routine (only secforest)
      //.. ..transition update is via (gridd+cell)->transitioned_ageclass_secforest==TRUE/FALSE

      #ifdef AGECLASS_PRIMARY
        //.......................................//
        //          P R I M A R Y               //
        //.......................................//
        //sorts stand list by standtype (forest first) then by PRIMARY or SECFOREST
        //..then reverse standAge, oldest first (stand->ageclass_standid)
        //..simplifies the search and merge loop
        sort_ForestTile((grid+cell)->standlist, AGESTAND, FOREST, PRIMARY);

        //transition stand fractions based on ageclass_standid
        ageclass_transition((grid+cell), pftpar, npft, PRIMARY);
      #endif

      #ifdef AGECLASS_SECFOREST
        //.......................................//
        //         S E C F O R E S T             //
        //.......................................//
        //NOTE: only transition if ageclass transitions were not already done in LUC routine
        if((grid+cell)->transitioned_ageclass_secforest==FALSE){
          //sorts stand list by standtype (forest first) then by PRIMARY or SECFOREST
          //..then reverse standAge, oldest first (stand->ageclass_standid)
          //..simplifies the search and merge loop
          sort_ForestTile((grid+cell)->standlist, AGESTAND, FOREST, SECFOREST);

          //transition stand fractions based on ageclass_standid
          ageclass_transition((grid+cell), pftpar, npft, SECFOREST);
        }
      #endif
    }
  }

  day=1;
  foreachmonth(month){
    for(cell=0;cell<config.ngridcell;cell++){
      initoutput_monthly(&((grid+cell)->output),npft);
      (grid+cell)->discharge.mfin=(grid+cell)->discharge.mfout=0.0;
      
      #ifdef MONTHLYLITTER
        if(!(grid+cell)->skip){
          //update litter pools monthly
          litter_update_monthly(grid+cell);
        }
      #endif
      
      #ifdef MONTHLY
         if(!(grid+cell)->skip){
           if(israndomprec(config.wet_filename)){
              prdaily(&((grid+cell)->climbuf),ndaymonth[month],
                (getcellprec(climate,cell))[month],
                (getcellwet(climate,cell))[month]);
           }
         }
      #endif
      
    } //..end update for each gridcell

    //continue for days of month
    foreachdayofmonth(dayofmonth,month){
      irrig_amount(grid,config);
      for(cell=0;cell<config.ngridcell;cell++){
        if(!(grid+cell)->skip){
          initoutput_daily(&((grid+cell)->output),dayofmonth);
              
          #ifdef DAILYLITTER  //update litter daily
            litter_update_daily(grid+cell);
          #endif

          #ifdef DAILY //calculate monthly temperature to use in update_daily to get phenology
            foreachdayofmonth(dayofmonth_mtemp,month_mtemp){
              mtemp[cell]+=(getcelltempDay(climate,cell))[dayofmonth_mtemp];
            }
            mtemp[cell]/=ndaymonth[month_mtemp];
          #endif

          #ifdef MONTHLY
            temp  =interpolate(getcelltemp(climate,cell),month,dayofmonth);
            sun   =interpolate(getcellsun(climate,cell),month,dayofmonth);
            swdown=0.;
            lwdown=0.;
            #ifdef SWLWRAD
              swdown=interpolate(getcellsw(climate,cell),month,dayofmonth);
              lwdown=interpolate(getcelllw(climate,cell),month,dayofmonth);
            #endif

            prec=  (israndomprec(config.wet_filename)) ? getprec(*(grid+cell),dayofmonth) :
                     interpolate(getcellprec(climate,cell),month,dayofmonth)*ndaymonth1[month];
          #endif

          #ifdef DAILY
            temp  =(getcelltempDay(climate,cell))[day-1]; /* GLOBCARBON input daily*/
            sun   =0;
            swdown=0.;
            lwdown=0.;
            #ifdef SWLWRAD
              swdown=(getcellswDay(climate,cell))[day-1]; /* GLOBCARBON input daily*/
              lwdown=(getcelllwDay(climate,cell))[day-1]; /* GLOBCARBON input daily*/
            #endif
             prec=(getcellprecDay(climate,cell))[day-1]; /* GLOBCARBON input daily*/
          #endif

          ozone=0.;
          #ifdef OZONE
            //ozone interpolation
            endYear = 0;
            baseYear = (year < 2000) ? 1850 : 2000;
            ozoneYear = (year < 2000) ? 1 : 2;
            //Get 36 (12months*3 scenario) O3 values for each cell
            ozoneTmp = (getcellozone(climate,cell));

            //Split to two groups (baseline and future)
            for(o=0;o<12;o++){
              ozone1all[o]=ozoneTmp[o+12*(ozoneYear-1)];
              ozone2all[o]=ozoneTmp[o+12*(ozoneYear)];
            }

            //Calculate pseudo daily O3 value for baseline and future
            ozone1=interpolate(ozone1all,month,dayofmonth);
            ozone2=interpolate(ozone2all,month,dayofmonth);

            if(strstr(config.ozone_filename,"2030cle")!=NULL)
               endYear = (year < 2000) ? 1999 : 2030;
            else if(strstr(config.ozone_filename,"2050iiasa")!=NULL)
               endYear = (year < 2000) ? 1999 : 2050;
            else if(strstr(config.ozone_filename,"2100cle")!=NULL)
               endYear = (year < 2000) ? 1999 : 2100;

            //Calculate present day ozone value with linear interpolation
            ozoneRate  = (ozone2-ozone1)/(endYear-baseYear);
            nOzoneYear = (year-baseYear <= 0) ? 0 : year-baseYear;
            ozone      = ozone1 + (ozoneRate*nOzoneYear);
          #endif
   
          #ifdef SWETINPUT
            if(!(grid+cell)->skip){
              //update monthly inundation
              minund=(getcellinund(climate,cell))[month];
              if(minund <0) minund=0.0;
            }
          #endif

          prec= prec>0.000001 ? prec : 0.0;
          updategdd(grid[cell].gdd,pftpar,npft,ncft,temp);
          (grid+cell)->aprec+= prec;

          #ifdef CO2GRID
            co2=interpolate(getcellco2(climate,cell),month,dayofmonth);
          #endif
          //printf("Year: %d\t CO2: %f\n",year,co2);
          //printf("%f\t %f\t %f\t %f\t %f\t %f\t %f\n",co2, temp, prec, sun, swdown, lwdown, mtemp);
          update_daily(output,grid+cell,config,co2,temp,prec,sun,swdown,lwdown,day,month,year,pftpar,npft,ncft,ntypes,
                              &landuse->landfrac[cell*landuse->nbands],landuse->intercrop,cell,ozone,
                              ndaymonth[month],mtemp[cell],minund);
        }//..end skip
      } //..end cells

      #ifdef RIVER_ROUTING
        drain(grid,config);
      #endif
      day++;
    } //..end days of month

    for(cell=0;cell<config.ngridcell;cell++){
      if((grid+cell)->output.mdischarge<0) (grid+cell)->output.mdischarge=0;

      if(!(grid+cell)->skip ){
        #ifdef MONTHLY
          update_monthly(output,grid+cell,(getcelltemp(climate,cell))[month],month,year,config,npft);
        #endif

        #ifdef DAILY
          update_monthly(output,grid+cell,mtemp[cell],month,year,config,npft);
        #endif
      }
    }//..end gridcells
  } /* of 'foreachmonth */

  for(cell=0;cell<config.ngridcell;cell++){
    if(!(grid+cell)->skip ){

      delta_totc=update_annual(output,grid+cell,pftpar,npft,ncft,ntypes,year,config,landuse->intercrop);

      //-----------------------------------------------------------------
      //    C A R B O N   &   W A T E R   B A L A N C E   C H E C K
      //-----------------------------------------------------------------
      check_fluxes(grid+cell,config,year,cell,delta_totc);

      #ifdef DEBUG
        if(year>config.firstyear){
          printf("year=%d\n",year);
          printf("cell=%d\n",cell);
          printcell(grid+cell,1);
        }
      #endif
      if(config.nspinup>soil_equil_year && year==config.firstyear-config.nspinup+soil_equil_year)
        equilsom(grid+cell);
    }
  }

  //--------------------------------------------------
  //               write to output
  //--------------------------------------------------
  if((config.firstyear!=config.lastyear) && (year>=(config.firstyear-config.nspinup))){
    fwriteoutput_pft(output,grid,config,npft,ncft,year);
  }
} /* of 'iterateyear' */
