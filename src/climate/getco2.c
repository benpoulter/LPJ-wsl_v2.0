/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ g e t c o 2 . c                                 **/
/*******************************************************************/
#include "lpj.h"

Real getco2(const Climate *climate,int year)
{
#ifndef CO2MEAN

  //July 2013: The 'year' variable becomes a counter from zero through nyears-1 of the simulation
  //The counter is used to access the CO2 value in the annual CO2 vector
  return (year-climate->firstco2year<0) ? CO2_P : climate->co2[year-climate->firstco2year];

#else //USE PRE-INDUSTRIAL CO2, defined in include/climate.h
  return CO2_P;
#endif
} /* of 'getco2' */
