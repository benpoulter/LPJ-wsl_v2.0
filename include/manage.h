/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / m a n a g e . h                             **/
/*******************************************************************/

#ifndef MANAGE_H /* Already included? */
#define MANAGE_H

#include "managepar.h"

/* Definition of constants */

/* Declaration of datatypes */

typedef struct
{
  int id;               /* country id (0-196)*/
  char *name;           /* country name */
  Real ep;              /* project efficiency */
  Real ec;              /* irrigation conveyance efficiency */
  Real laimax_tempcer;  /*laimax for temperate cereals*/
  Real laimax_maize;    /*laimax for maize*/

} Countrypar;


typedef struct
{
  int id;               /* region id (0-432)*/
  char *name;           /* region name */
} Regionpar;


typedef struct
{
  const Countrypar *par;
  const Regionpar *regpar;

} Manage;



/* Declaration of functions */

extern int fscancountrypar(Countrypar **,const char *);
extern int fscanregionpar(Regionpar **,const char *);
extern void initmanage(Manage *, const Countrypar *, const Regionpar *);

/* Definitions of macros */

#endif
