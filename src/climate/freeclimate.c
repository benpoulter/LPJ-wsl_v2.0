/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ f r e e c l i m a t e . c                       **/
/*******************************************************************/

#include "lpj.h"

void freeclimate(Climate *climate)
{
  fclose(climate->file_temp);
  fclose(climate->file_prec);
  fclose(climate->file_cloud);
  free(climate->co2);
  if(climate->file_wet!=NULL)
  {
    fclose(climate->file_wet);
    free(climate->wet);
  }
  free(climate);
} /* of 'freeclimate' */
