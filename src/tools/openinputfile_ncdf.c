/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ o p e n i n p u t f i l e _ n c d f . c           **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

int openinputfile_ncdf(Header *header,Bool *swap,const char *filename,
                      Config *config, const char *headername, int version)
{
  //DEVQ: HAVE METADATA READ FROM ATTRIBUTES
  int retval, file, varid_lon, varid_lat;
  size_t lenLon, lenLat;

  printf("Opening %s \n", filename);

  // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file
  if ((retval = nc_open(filename, NC_NOWRITE, &file)))
     ERR(retval);

  // Read the lon lat ids
  if ((retval = nc_inq_dimid(file, "lon", &varid_lon)))
     ERR(retval);
  // Read the variable name from last dimension
  if ((retval = nc_inq_dimid(file, "lat", &varid_lat)))
     ERR(retval);

  // Get the lengths of lon, lat
  if ((retval = nc_inq_dimlen(file, varid_lon, &lenLon)))
     ERR(retval);
  if ((retval = nc_inq_dimlen(file, varid_lat, &lenLat)))
     ERR(retval);

  header->firstyear=1901;        //Must be changed for different climate inputs
  header->nyear=0;               //Must be changed
  #ifdef MONTHLY
    header->nbands=12;     
  #endif

  #ifdef DAILY
    header->nbands=365;          //365 for daily inputs - no leap day
  #endif

  //Always constant
  header->order=CELLYEAR;        //WAS USED IN initclimate.c, but now deleted, and not used
  header->ncell=lenLat*lenLon;
  //Force ncell to be equal to 67420
  //..a full global grid will be 720*360 dimensions, but coordinates file will be 67420
  if(header->ncell == (720*360))
    header->ncell=67420;      

  header->firstcell=0;
  //headername = headername;
  //version = version;

  //Then use regular LPJ code
  if(header->firstyear>config->firstyear)
    fprintf(stderr,"Warning: first year in '%s'=%d greater than  %d.\n",
            filename,header->firstyear,config->firstyear);

  if(config->totalgridcell!=header->ncell && config->totalgridcell>0)
    fprintf(stderr,"Warning: number of gridcells in '%s' distinct from %d.\n",
            filename,config->totalgridcell);
  else{ 
    config->totalgridcell=header->ncell;
    if(config->ngridcell==0)
      config->ngridcell=config->totalgridcell;
  }
  return file;
} /* of 'openinputfile_ncdf' */
