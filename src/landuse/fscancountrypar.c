/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ f s c a n c o u n t r y p a r . c               **/
/*******************************************************************/
#include "lpj.h"

#define fscanreal2(file,var,fcn,name)\
  if(fscanreal(file,var))\
  {\
    readrealerr(fcn,name);\
    break;\
  }

int fscancountrypar(Countrypar **countrypar, /* Pointer to countrypar array */
                   const char *filename      /* filename */
                  )                          /* returns number of elements in array */
{
  int ncountries,n,id;
  char *cmd;
  FILE *file;
  String s;
  Countrypar *country;
#ifdef USE_CPP
  cmd=malloc(strlen(filename)+strlen(cpp_cmd)+1);
  strcat(strcpy(cmd,cpp_cmd),filename);
  file=pt_popen(cmd,"r");
  free(cmd);
#else
  file=fopen(filename,"r");
#endif
  if(file==NULL){
    printfopenerr("fscancountrypar",filename);
    return 0;
  }

  if(fscanf(file,"%d",&ncountries)!=1){
    readinterr(filename,"ncountries");
    pt_pclose(file);
    return 0;
  }
  *countrypar=newvec(Countrypar,ncountries);
  check(*countrypar);
  for(n=0;n<ncountries;n++){
    if(fscanf(file,"%d",&id)!=1){
      readinterr(filename,"countrynumber");
      break;
    } 
    if(id<0 || id>=ncountries){
      fprintf(stderr,"Error in '%s': invalid range of 'countrypar'.\n",filename);
      break;
    }
    country=(*countrypar)+id;
   
    if(fscanstring(file,s)){
      readstringerr(filename,"name");
      break;
    }
    country->name=strdup(s);
    country->id=id;

    fscanreal2(file,&country->laimax_tempcer,filename,"laimax_tempcer");
    fscanreal2(file,&country->laimax_maize,filename,"laimax_maize");

    fscanreal2(file,&country->ep,filename,"project efficiency");
    fscanreal2(file,&country->ec,filename,"irrigation conveyance efficiency");
    #ifdef NIREQUALGIR
    /*NIR=GIR*/
    country->ep=country->ec=1.0;
    #endif
  }
  pt_pclose(file);
  return n;
} /* of 'fscancountrypar' */

