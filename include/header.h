/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / h e a d e r . h                             **/
/*******************************************************************/

#ifndef HEADER_H /* Already included? */
#define HEADER_H

/*Definition of constants */

#define RESTART_HEADER "LPJRESTART"
#define RESTART_VERSION 4
#define LPJ_CLIMATE_HEADER "LPJCLIMATE"
#define LPJ_CLIMATE_VERSION 1
#define LPJ_LANDUSE_HEADER "LPJLANDUSE"
#define LPJ_LANDUSE_VERSION 1
#define LPJGRID_HEADER "LPJGRID"
#define LPJGRID_VERSION 1
#define LPJDRAIN_HEADER "LPJDRAIN"
#define LPJDRAIN_VERSION 1
#define LPJ_COUNTRY_HEADER "LPJ_COUNTRY"
#define LPJ_COUNTRY_VERSION 1
#define LPJNEIGHB_IRRIG_HEADER "LPJNEIGHBIRRIG"
#define LPJNEIGHB_IRRIG_VERSION 1

#define CELLYEAR 1 
#define YEARCELL 2

/* Definition of datatypes */

typedef struct
{
  int order,firstyear,nyear,firstcell,ncell,nbands;
} Header;

/* Declaration of functions */

extern Bool fwriteheader(FILE *,Header, const char *,int);
extern Bool freadheader(FILE *,Header *,Bool *,const char *,int);
extern Bool freadheader_ncdf(int,Header *,Bool *,const char *,int); 
extern int  headersize(char *);

#endif
