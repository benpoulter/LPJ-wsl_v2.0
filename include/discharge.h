/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / d i s c h a r g e . h                       **/
/*******************************************************************/

#ifndef DISCHARGE_H /* Already included? */
#define DISCHARGE_H

#define v 86400             /* velocity m/d (1m/s) */
/* #define v 8640 velocity m/d (0.1m/s) */
#define kr 0.001            /* lake outflow coefficient 1/d */
#define H 5                 /* maximum active lake storage depth m */
#define IRRIGATION_PAUSE 0

/* Definition of datatypes */

typedef struct
{
  Real dmass_new;
  Real dmass_lake;
  Real dmass_lake_max;
  Real dfout_lake;
  Real fin;
  Real mfin;
  Real mfout;
  Real drunoff;
  long int l;
  short d[25];
  int count;
  Real gir;
  Real act_irrig_amount;
  short neighb_irrig;
  float waterdeficit;
} Discharge;

#endif
