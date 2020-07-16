/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ n e w _ c r o p . c                                **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

#define FROOTMAX 0.4

void new_crop(Pft *pft)
{
  Pftcrop *crop;
  Pftcroppar *par;
  crop=new(Pftcrop);
  pft->data=crop;
  par=pft->par->data;

  pft->fpc=1;
  pft->nind=1;

  crop->growingdays=0;

  crop->vdsum=crop->husum=crop->fphu=0.0;
  crop->ind.so=0.0;
  pft->bm_inc=20.0;
  pft->monopool=0.0;
  crop->laimax_adjusted=1; 
  crop->flaimax=0.000083; 
  crop->demandsum=0;
  crop->supplysum=0;

  crop->lai=crop->lai000=crop->flaimax*par->laimax;
  crop->lai_nppdeficit=0.0;
  pft->phen=crop->lai/par->laimax;
  crop->senescence=FALSE;

  allocation_daily_crop(pft,0,-9); 
} /* of 'new_crop' */

/*
- new_crop() is called when a new crop is added to a stand (see newpft.c)
- sets fpc 1 for crops
- provides memory place for specific crop variables
  -> initialisation of specific crop variables (see crop.h)
*/
