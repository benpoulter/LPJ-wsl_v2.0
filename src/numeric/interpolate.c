/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ i n t e r p o l a t e . c                       **/
/*******************************************************************/
#include <stdio.h>
#include "types.h"
#include "date.h"

/*
 *  Function interpol
 *
 *  Daily interpolation of monthly data
 *
 */

Real interpolate(const MReal mval, /* monthly values to interpolate       */
                 int month,        /* month (0..11)                       */
                 int dm            /* day of month (1 ..ndaymonth[month]) */
                )                  /* returns interpolated value          */
{
  int nextmonth;
  if(dm>ndaymonth[month]/2)
  {
    dm-=ndaymonth[month]/2;
    nextmonth=(month<NMONTH-1) ? month+1 : 0;
  }
  else
  {
    nextmonth=month;
    if(month==0)
      month=NMONTH-1;
    else
      month--; 
    dm+=ndaymonth[month]/2; 
  }
#ifdef DEBUG3
  printf("mval[%d]=%g,mval[%d]=%g,dm=%d,diffday=%g,%d\n",month,mval[month],
         nextmonth,mval[nextmonth],dm,diffday[month],ndaymonth[month]);
  printf("res=%g\n",mval[month]+dm*(mval[nextmonth]-mval[month])*diffday[month]);
#endif

  return mval[month]+dm*(mval[nextmonth]-mval[month])*diffday[month];
} /* of 'interpolate' */
