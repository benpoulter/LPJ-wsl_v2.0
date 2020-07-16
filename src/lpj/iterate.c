/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ i t e r a t e . c                                   **/
/*******************************************************************/
#include "lpj.h"

#ifdef ERA
    #define nspinyear 30 // cycle length during spinup (yr)
    #define startspinupyear 1980 //define the start year for spin up 
#endif

#ifdef MERRA2
    #define nspinyear 30 // cycle length during spinup (yr)
    #define startspinupyear 1980 //define the start year for spin up 
#endif

#ifdef JRA55
    #define nspinyear 30 // cycle length during spinup (yr)
    #define startspinupyear 1980 //define the start year for spin up 
#endif

#ifdef CRU
    #define nspinyear 30 /* cycle length during spinup (yr) */
    #define startspinupyear 1901 //define the start year for spin up 
#endif

#ifdef CRUNCEP
    #define nspinyear 30 /* cycle length during spinup (yr) */
    #define startspinupyear 1901 //define the start year for spin up 
#endif

#ifdef AR4
    #define nspinyear 30 /* cycle length during spinup (yr) */
    #define startspinupyear 1901 //define the start year for spin up
#endif

#ifdef CMIP5
    #define nspinyear 30 /* cycle length during spinup (yr) */
    #define startspinupyear 1901 //define the start year for spin up
#endif

#ifdef SWETINPUT
    #define startYear 2000  //startyear of inundation
    #define endYear 2017    //endyear of inundation
#endif

int iterate(FILE **output,
            Cell *grid,
            Climate *climate,
            Landuse *landuse,
            const Pftpar *pftpar,
            int npft,
            int ncft,
            int ntypes,
            Config config)
{
  Real co2;
  int year;
  int j;
  int random_year; //random_year for spinup

  for(year=config.firstyear-config.nspinup;year<=config.lastyear;year++){
    //Create vector of latitudes and longitudes for NCDF handling
    Real latIn[config.ngridcell], lonIn[config.ngridcell];
    for(j=0; j<config.ngridcell; j++){
       latIn[j]=grid[j].coord.lat;
       lonIn[j]=grid[j].coord.lon;
       //printf("%f\t%f\n",latIn[j],lonIn[j]);
    }

    //Get CO2 data from data file or ncdf for all cells for 1 year
    co2=0;
    #ifndef CO2GRID
      co2=getco2(climate,year);
      printf("Year: %d\t CO2: %f\n",year,co2);
    #else
      getco2_ncdf(climate,year,latIn,lonIn,config.ngridcell);
      //Must set CO2 to zero, because now CO2 is read from the climate data structure
      co2=0;
      printf("Year: %d CO2 is gridded..\n",year);
    #endif

    //Get climate from CLM data for all cells for 1 year
    #ifndef CLM_NCDF
      #ifdef DAILY //no daily CLM data (yet)
       printf("ERROR: Can not run daily climate with non-ncdf drivers\n");
      #else //Get monthly
        if(getclimate(climate,(year<config.firstyear) ? (year-config.firstyear+config.nspinup)
              % nspinyear + config.firstyear : year))
        {
           fprintf(stderr,"Simulation stopped because of CLM-MONTHLY-CLM error.\n");
           return year;
        }
      #endif
    #endif

    //Get climate data from NCDF data for all cells for 1 year
    #ifdef CLM_NCDF
      //Read in randomly selected year
      random_year=(rand()%nspinyear) + startspinupyear;
      #ifdef DAILY 
        if(getclimate_daily_ncdf(climate,(year<startspinupyear) ? random_year : year, latIn, lonIn, config.ngridcell)){
          fprintf(stderr,"Simulation stopped because of CLM-DAILY-NCDF error.\n");
          return year;
        }
      #else //get Monthly
        //use the IF statement below to have random years for entire run (i.e., for TRENDY runs S0, S1, S4)
        //if(getclimate_ncdf(climate,random_year, latIn, lonIn, config.ngridcell))
        if(getclimate_ncdf(climate,(year<startspinupyear) ? random_year : year, latIn, lonIn, config.ngridcell)){
          fprintf(stderr,"Simulation stopped because of CLM-MONTHLY-NCDF error.\n");
          return year;
        }
      #endif
    #endif

    //Get correct land use data for all cells for 1 year
    #ifdef HYDE_BINARY
      if(getlanduse(landuse,(year<config.firstyear) ? config.firstyear : year,config.ngridcell,ncft)){
        fprintf(stderr,"Simulation stopped because of LUC-CLM error.\n");
        return year;
      }
    #else
      //In getlanduse_ncdf, the year of the land cover is adjusted by the ncdf land cover start year
      //    .. getlanduse_ncdf is deprecated
      //    .. use getlanduse_NET_ncdf for 'original' landusechange routine
      #if defined(GROSS_LUC) || defined(NETinGROSS)
        if(getlanduse_GROSS_ncdf(config,landuse,year,config.ngridcell,ncft, latIn, lonIn)){
          fprintf(stderr,"Simulation stopped because of LUC-NCDF error.\n");
          return year;
        }
      #else //NET LUC
        if(getlanduse_NET_ncdf(landuse,year,config.ngridcell,ncft, latIn, lonIn)){
          fprintf(stderr,"Simulation stopped because of LUC-NCDF error.\n");
          return year;
        }
      #endif
    #endif

    //get satellate inundation for all cells for 1 year
    #ifdef SWETINPUT
      //for years that doesn't have observations we use random year
      random_year=(rand()%(endYear-startYear));
      if(getinund_ncdf(climate,(year < startYear || year > endYear) ? random_year : (year-startYear), latIn, lonIn, config.ngridcell)){
          fprintf(stderr,"Simulation stopped because of INUND-NCDF error.\n");
          return year;
      }
    #endif
    
    //Iterate through the days and cells in the year
    iterateyear(output,grid,climate,landuse,co2,pftpar,npft,ncft,ntypes,config,year); 

    //Write message if end of spinup
    if(iswriterestart(config) && year==config.lastyear){
      printf("Write restart file '%s'.\n",config.write_restart_filename);
      fwriterestart(config.write_restart_filename,
                    grid,config.startgrid,config.ngridcell,
                    npft,ncft,year,pftpar,landuse->intercrop); 
    }
  } /* of 'for(year=...)' */
  return year;
} /* of 'iterate' */
