/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ f s c a n r e g i o n p a r . c                 **/
/*******************************************************************/
#include "lpj.h"

#define fscanreal2(file,var,fcn,name)\
  if(fscanreal(file,var))\
  {\
    readrealerr(fcn,name);\
    break;\
  }

int fscanregionpar(Regionpar **regionpar,  /* Pointer to regionpar array */
                   const char *filename    /* filename */
                  )                        /* returns number of elements in array */
{
  int nregions,n,id;
  char *cmd;
  FILE *file;
  String s;
  Regionpar *region;
#ifdef USE_CPP
  cmd=malloc(strlen(filename)+strlen(cpp_cmd)+1);
  strcat(strcpy(cmd,cpp_cmd),filename);
  file=pt_popen(cmd,"r");
  free(cmd);
#else
  file=fopen(filename,"r");
#endif
  if(file==NULL){
    printfopenerr("fscanregionpar",filename);
    return 0;
  }

  if(fscanf(file,"%d",&nregions)!=1){
    readinterr(filename,"nregions");
    pt_pclose(file);
    return 0;
  }

  *regionpar=newvec(Regionpar,nregions);
  check(*regionpar);

  for(n=0;n<nregions;n++){
    if(fscanf(file,"%d",&id)!=1){
      readinterr(filename,"regionnumber");        
      break;
    } 
    if(id<0 || id>=nregions){
      fprintf(stderr,"Error in '%s': invalid range of 'regionpar'.\n",filename);
      break;
    }
    region=(*regionpar)+id;
   
    if(fscanstring(file,s)){
      readstringerr(filename,"name");
      break;
    }
    region->name=strdup(s);
    region->id=id;
  }
  pt_pclose(file);
  return n;
} /* of 'fscanregionpar' */
