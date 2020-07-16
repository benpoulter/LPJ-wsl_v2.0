/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ f r e e c l i m a t e _ n c d f . c             **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

void freeclimate_ncdf(Climate *climate)
{
 int retval;
  if ((retval = nc_close(climate->ncid_temp)))
  ERR(retval);
  if ((retval = nc_close(climate->ncid_prec)))
  ERR(retval);
  if ((retval = nc_close(climate->ncid_cloud)))
  ERR(retval);
  #ifdef RAD
  if ((retval = nc_close(climate->ncid_sw)))
  ERR(retval);
  if ((retval = nc_close(climate->ncid_lw)))
  ERR(retval);
  #endif

  #ifdef OZONE
  if ((retval = nc_close(climate->ncid_ozone)))
  ERR(retval);
  #endif

  //Need to free CO2 data also (not a ncdf)
  free(climate->co2);
  if(climate->ncid_wet!=9999)
  {
    if((retval = nc_close(climate->ncid_wet)))
      ERR(retval);
    free(climate->wet);
  }
  free(climate);
} /* of 'freeclimate' */
