/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f s c a n s o i l p a r . c                        **/
/*******************************************************************/
#include "lpj.h"

#define fscanreal2(file,var,fcn,name)\
  if(fscanreal(file,var))\
  {\
    readrealerr(fcn,name);\
    break;\
  }

int fscansoilpar(Soilpar **soilpar,   /* Pointer to Soilpar array */
                 const char *filename /* filename */
                )                     /* returns number of elements in array */
{
  int nsoil,n,id,l;
  char *cmd;
  FILE *file;
  String s;
  Soilpar *soil;
  #ifdef USE_CPP
    cmd=malloc(strlen(filename)+strlen(cpp_cmd)+1);
    strcat(strcpy(cmd,cpp_cmd),filename);
    file=pt_popen(cmd,"r");
    free(cmd);
  #else
    file=fopen(filename,"r");
  #endif
  if(file==NULL){
    printfopenerr("fscansoilpar",filename);
    return 0;
  }

  if(fscanf(file,"%d",&nsoil)!=1){
    readinterr(filename,"nsoil");
    pt_pclose(file);
    return 0;
  }
  *soilpar=newvec(Soilpar,nsoil);
  check(*soilpar);
  for(n=0;n<nsoil;n++){
    if(fscanf(file,"%d",&id)!=1){
      readinterr(filename,"soiltype");
      break;
    } 
    if(id<0 || id>=nsoil){
      fprintf(stderr,"Error in '%s': invalid range of 'soilpar'.\n",filename);
      break;
    }
    soil=(*soilpar)+id;
    if(fscanstring(file,s)){
      readstringerr(filename,"name");
      break;
    }
    soil->name=strdup(s);
    soil->type=id;
    fscanreal2(file,&soil->k2,filename,"k2");
    fscanreal2(file,&soil->k1,filename,"k1");
    for(l=0;l<NSOILLAYER;l++){
      fscanreal2(file,soil->whc+l,filename,"whc");
    }
    fscanreal2(file,&soil->tdiff_0,filename,"tdiff_0");
    fscanreal2(file,&soil->tdiff_15,filename,"tdiff_15");
    fscanreal2(file,&soil->tdiff_100,filename,"tdiff_100");
  }
  pt_pclose(file);

  return n;
} /* of 'fscansoilpar' */
