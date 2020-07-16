/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / d a t e . h                                 **/
/*******************************************************************/

#ifndef DATE_H /* Already included? */
#define DATE_H

#define NMONTH 12     /* number of months in a year         */
#define NDAYYEAR 365  /* number of days in a year (no leap) */

typedef Real MReal[NMONTH];

extern int sdaymonth[NMONTH];  /* first day of month on time scale year */
extern int ndaymonth[NMONTH];  /* number of days in each month          */
extern MReal ndaymonth1;       /* 1/(number of days in each month)      */
extern int midday[NMONTH+1];
extern MReal diffday;

/* Declaration of functions */

extern Real interpolate(const MReal,int,int);

/* Definition of macros */

#define foreachmonth(month) for(month=0;month<NMONTH;month++)
#ifdef MONTHLY
  #define foreachdayofmonth(day,month) for(day=0;day<ndaymonth[month];day++)
#endif

#ifdef DAILY
   #define foreachdayofmonth(day,month) for(day=sdaymonth[month];day<sdaymonth[month]+ndaymonth[month];day++) //BP DAILY
#endif

#endif
