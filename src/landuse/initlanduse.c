/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ i n i t l a n d u s e . c                       **/
/*******************************************************************/
#include "lpj.h"

/*
 *     Function initlanduse
 *
 *     Initializes landuse datatype 
 *
 */

Landuse *initlanduse(Config *config)
{
  Header header;
  Landuse *landuse;
  landuse=new(Landuse);
  
  if((landuse->file_landuse=openinputfile(&header,&landuse->swap,config->landuse_filename,config,
                 LPJ_LANDUSE_HEADER,LPJ_LANDUSE_VERSION))==NULL)
  {
    free(landuse);
    return NULL;
  }

  landuse->firstyear=header.firstyear;
  landuse->size=config->totalgridcell*header.nbands*sizeof(short);
  landuse->n=config->ngridcell*header.nbands;
  landuse->offset=config->startgrid*header.nbands*sizeof(short)+headersize(LPJ_LANDUSE_HEADER);
  landuse->nbands=header.nbands;
  landuse->intercrop=TRUE;
   
  if((landuse->landfrac=newvec(Real,landuse->n))==NULL)
  {
    printallocerr("initlanduse","nfrac");
    free(landuse);
    return NULL;
  }
  return landuse;
} /* of 'initlanduse' */

/*
- called in lpj()
- opens the landuse input file (see also cfts26_lu2clm.c)
- sets the landuse variables (see also landuse.h)
*/
