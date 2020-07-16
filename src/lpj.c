/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  s r c / l p j . c                                            **/
/*******************************************************************/

#include <time.h>
#ifdef USE_UNAME
#include <sys/utsname.h>
#endif
#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "crop.h"

#define NTYPES 3 /* number of PFT types: grass, tree, crop */

int main(int argc,char **argv)
{
  FILE **output;
  char *progname;
  int npft,ncft,nsoil,year,ncountries,nregions;
  Pftpar *pftpar;
  Soilpar *soilpar;
  Countrypar *countrypar;
  Regionpar *regionpar;
  Cell *grid;
  Climate *climate;
  Landuse *landuse;
  time_t tstart,tend;

  #ifdef USE_UNAME
  struct utsname osname;
  #endif

  /* Create array of functions, uses the typedef of (*Fscanpftparfcn) in pft.h */
  Fscanpftparfcn scanfcn[NTYPES]={fscanpft_grass,fscanpft_tree,fscanpft_crop};
  Config config;

  #ifdef USE_UNAME
    uname(&osname);
    printf("**** %s C Version %s (" __DATE__ ") %s %s.%s ****\n\n",argv[0],
           LPJ_VERSION,osname.sysname,osname.version,osname.release);
  #else
    printf("**** %s C Version %s (" __DATE__ ") ****\n\n",argv[0],
           LPJ_VERSION);
  #endif

  //Scan configuration files
  //Read conf file from command line call to lpj
  progname=argv[0];

  if(fscanconfig(&config,&argc,&argv))
    fail("Error reading config");
  if((npft=fscanpftpar(&pftpar,config.pftpar_filename,scanfcn,NTYPES,&ncft))==0)
    fail("Error reading '%s'",config.pftpar_filename);
  if((nsoil=fscansoilpar(&soilpar,config.soilpar_filename))==0)
    fail("Error reading '%s'",config.soilpar_filename);
  if((ncountries=fscancountrypar(&countrypar,config.countrypar_filename))==0)
    fail("Error reading '%s',",config.countrypar_filename);
  if((nregions=fscanregionpar(&regionpar,config.regionpar_filename))==0)
    fail("Error reading '%s',",config.regionpar_filename);
  if(isreadrestart(config))
    printf("Starting from restart file '%s'.\n",config.restart_filename);

  //Initialize climate arrays
  #ifndef CLM_NCDF
    //binary input format
    if((climate=initclimate(&config))==NULL) 
      fail("Error initializing climate");
  #else
    //netcdf input format
    #ifdef MONTHLY
      if((climate=initclimate_ncdf(&config))==NULL)
      fail("Error initializing climate");
    #else
      //else read as daily
      if((climate=initclimate_daily_ncdf(&config))==NULL)
      fail("Error initializing climate");
    #endif
  #endif

  //Initialize land cover/use array
  #ifdef HYDE_BINARY
    if((landuse=initlanduse(&config))==NULL)
      fail("Error initializing landuse");
  #else
    //Landuse driver is NETCDF
    #if defined(GROSS_LUC) || defined(NETinGROSS)
        //Read as NCDF
        if((landuse=initlanduse_GROSS_ncdf(&config))==NULL)
          fail("Error initializing landuse");
    #else
        if((landuse=initlanduse_NET_ncdf(&config))==NULL)
          fail("Error initializing landuse");
    #endif
  #endif

  //Initialize grid attributes, and read in all of CO2 data from text file
  if((grid=newgrid(&config,pftpar,npft,ncft,soilpar,nsoil,countrypar,ncountries,regionpar,nregions,landuse))==NULL)
    fail("Error initializing grid");

  /* Initialize random seed */
  #ifdef USE_RAND48
  srand48((config.seed==RANDOM_SEED) ? time(NULL) : config.seed);  
  #else
  setseed((config.seed==RANDOM_SEED) ? time(NULL) : config.seed);  
  #endif

  output=fopenoutput(config);

  #ifdef RIVER_ROUTING
  if(initdrain(grid,&config))
    fail("Error initializing drainage");
  #endif

  printf("Simulation begins...\n");
  time(&tstart); /* Start timing */
  /* NTYPES is set to natural vegetation must be considered
   * in light and establishment 
   * crops must have last id-number */

  //Start the loops by year, then grid cell
  year=iterate(output,grid,climate,landuse,pftpar,npft,ncft,NTYPES,config);
  time(&tend); /* Stop timing */

  if(output[GRID]!=NULL)
    writecoords(output[GRID],grid,config.ngridcell);
  fcloseoutput(output,config.n_out);
  printf("Simulation ended.\n");

  //FREE MEMORY FOR CLIMATE AND LAND USE AND GRID
  #ifndef CLM_NCDF
    freeclimate(climate);
  #else
    #ifdef MONTHLY
      freeclimate_ncdf(climate);
    #endif

    #ifdef DAILY
      freeclimate_daily_ncdf(climate);
    #endif
  #endif

  #ifdef HYDE_BINARY
    freelanduse(landuse);
  #else
   //Landuse driver is NETCDF
   freelanduse_ncdf(landuse);
  #endif

  freegrid(grid,config.ngridcell,config.restart_filename!=NULL);

  //PRINT END MESSAGES
  printf( (year>config.lastyear) ? "%s" : "%s successfully",progname);
  printf(" terminated, %d grid cells processed.\n"
         "Wall clock time: %d sec, %.2g sec/cell/year.\n",
         config.ngridcell,(int)(tend-tstart),
         (double)(tend-tstart)/config.ngridcell/(year-config.firstyear+config.nspinup));
  return EXIT_SUCCESS;
} /* of 'main' */
