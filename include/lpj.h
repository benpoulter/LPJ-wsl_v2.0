/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / l p j . h                                   **/
/*******************************************************************/

#ifndef LPJ_H /* Already included? */
#define LPJ_H

#define LPJ_VERSION  "1.0.0"  /*  Poulter github version */

/* Necessary header files */

/* Standard C header files */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>

/*  Defined header files for LPJ */

#include "list.h"
#include "types.h"
#include "popen.h"
#include "swap.h"
#include "coord.h"
#include "config.h"
#include "buffer.h"
#include "date.h"
#include "header.h"
#include "climate.h"
#include "soil.h"
#include "manage.h"
#include "pft.h"
#include "errmsg.h"
#include "cropdates.h"
#include "crop.h"
#include "pftlist.h"
#include "numeric.h"
#include "units.h"
#include "conf.h"
#include "landuse.h"
#include "output.h"
#include "stand.h"
#include "discharge.h"
#include "topmodel.h"
#include "permafrost.h"
#include "cell.h"
#include "input.h"

extern Bool iffire;

/* Declaration of functions */

extern Cell *newgrid(Config *,const Pftpar [],int,int,const Soilpar [],int,const Countrypar [],int,const Regionpar [],int,Landuse *);
extern Bool fwriterestart(const char *,Cell *,int,int,int,int,int,const Pftpar*,Bool);

/* Definition of macros */

#define israndomprec(wet_filename) (wet_filename!=NULL)
#define iswriterestart(config) (config.write_restart_filename!=NULL)
#define isreadrestart(config) (config.restart_filename!=NULL)

#endif
