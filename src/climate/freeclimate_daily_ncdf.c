/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ f r e e c l i m a t e _ d a i l y _ n c d f . c **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

void freeclimate_daily_ncdf(Climate *climate)
{
 int retval;
  if ((retval = nc_close(climate->ncid_temp)))
  ERR(retval);
  if ((retval = nc_close(climate->ncid_prec)))
  ERR(retval);
  if ((retval = nc_close(climate->ncid_lw)))
  ERR(retval);
  if ((retval = nc_close(climate->ncid_sw)))
  ERR(retval);

  free(climate->co2);
  free(climate);
} /* of 'freeclimate' */
