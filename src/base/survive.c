/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ s u r v i v e . c                                  **/
/*******************************************************************/

#include "lpj.h"

Bool survive(const Pftpar *pftpar,const Climbuf *climbuf)
{
  Real temp_min20,temp_max20;
  
  temp_min20=climbuf->min->avg/climbuf->min->n;
  temp_max20=climbuf->max->avg/climbuf->max->n;
  return (temp_min20>=pftpar->temp.low) || 
         (temp_max20-temp_min20>=pftpar->min_temprange);

} /* of 'survive' */
