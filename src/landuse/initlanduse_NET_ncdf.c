/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ i n i t l a n d u s e _ N E T _ n c d f . c     **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

/*
 *     Function initlanduse
 *
 *     Initializes landuse datatype 
 *
 */

Landuse *initlanduse_NET_ncdf(Config *config)
{
  int i;
  Header header;
  Landuse *landuse;
  landuse=new(Landuse);
  
  if((landuse->ncid_luc=openinputfile_ncdf(&header,&landuse->swap,config->landuse_filename,config,
                 LPJ_LANDUSE_HEADER,LPJ_LANDUSE_VERSION))==0)
  {
    free(landuse);
    return NULL;
  }

#ifdef BONDEAU
  landuse->firstyear=1901;
#endif

#ifdef SYNMAP
  landuse->firstyear=1700;
#endif

#ifdef HURTT
  landuse->firstyear=1500; //For Hurtt input, with primary, crop, pasture, secondary
#endif

#ifdef HYDE
  landuse->firstyear=1600; //HYDE version TRENDY3
#endif
  //DEVQ: consider change nbands to include CROPgeneral/PASTURE LUtypes for deforest routine in SecForest module
  landuse->nbands=24;  //ADDED THIS to fix to keep to full 24 bands - importnat in get land use and land use module
  landuse->size=config->totalgridcell*landuse->nbands*sizeof(short);
  landuse->n=config->ngridcell*landuse->nbands;
  landuse->offset=0;
  landuse->intercrop=TRUE;
   
  if((landuse->landfrac=newvec(Real,landuse->n))==NULL)
  {
    printallocerr("initlanduse","nfrac");
    free(landuse);
    return NULL;
  }
  ////////////////////////////////

  //initialize vector to zero
  for(i=0; i < landuse->n; i++){landuse->landfrac[i]=0.0;}

  return landuse;
} /* of 'initlanduse' */

/*
- called in lpj()
- opens the landuse input file (see also cfts26_lu2clm.c)
- sets the landuse variables (see also landuse.h)
*/
