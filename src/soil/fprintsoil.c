/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f p r i n t s o i l . c                            **/
/*******************************************************************/
#include "lpj.h"

void fprintsoil(FILE *file,const Soil *soil)
{
  int l;
  fprintf(file,"Soil type: %s\n",soil->par->name);
  fprintf(file,"Cpool: ");
  fprintpool(file,soil->cpool);
  fprintf(file,"\nLitter: ");
  fprintlitter(file,soil->litter);
  fprintf(file,"\nWater: ");
  foreachsoillayer(l)
    fprintf(file,"%5.2g ",soil->w[l]);
  fprintf(file,"\nSnowpack: %g\n",soil->snowpack);
} /* of 'fprintsoil' */
