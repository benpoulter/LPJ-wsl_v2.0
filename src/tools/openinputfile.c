/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ o p e n i n p u t f i l e . c                     **/
/*******************************************************************/
#include "lpj.h"

FILE *openinputfile(Header *header,Bool *swap,const char *filename,
                      Config *config, const char *headername, int version)
{
  FILE *file;
  if((file=fopen(filename,"rb"))==NULL){
    printfopenerr("init",filename);
    return NULL;
  }
  if(freadheader(file,header,swap,headername,version)){
    fprintf(stderr,"Invalid header in '%s'.\n",filename);
    fclose(file);
    return NULL;
  }
  if(header->firstyear>config->firstyear)
    fprintf(stderr,"Warning: first year in '%s'=%d greater than  %d.\n",
            filename,header->firstyear,config->firstyear);

  if(config->totalgridcell!=header->ncell && config->totalgridcell>0)
    fprintf(stderr,"Warning: number of gridcells in '%s' distinct from %d.\n",
            filename,config->totalgridcell);
  else{ 
    config->totalgridcell=header->ncell;
    if(config->ngridcell==0)
      config->ngridcell=config->totalgridcell;
  }
  return file;
} /* of 'openinputfile' */
