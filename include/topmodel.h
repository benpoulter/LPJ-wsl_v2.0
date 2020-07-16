/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / t o p m o d e l . h                         **/
/*******************************************************************/

#ifndef TOPMODEL_H /* Already included? */
#define TOPMODEL_H

/* #define v 8640 velocity m/d (0.1m/s) */

/* Definition of datatypes */

typedef struct
{
  Real cs;
  Real fmax;
  Real cti;
  Real tj;   // temperature JAN
  Real tm;   // mean temperature
} Topmodel;

/* Declaration of functions */

extern Real wtd(Real mswc[], const Soil *soil);
extern Real wetdfrac(const Topmodel *topmodel, const Soil *soil, Real wtd_temp, Real ice_frac[]);
extern Real ch4e(const Topmodel *topmodel, const Coord *coord,Real soiltemp,Real rh,Real wetfrac);
extern Real ch4o(const Topmodel *topmodel, const Coord *coord,Real soiltemp,Real rh,Real wetfrac);
#endif
