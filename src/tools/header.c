/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ h e a d e r . c                                   **/
/*******************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "header.h"
#include "swap.h"

Bool fwriteheader(FILE *file,Header header, const char *headername, int version)
{
  if(fwrite(headername,strlen(headername),1,file)!=1)
    return TRUE;
  if (fwrite(&version,sizeof(version),1,file)!=1)
    return TRUE;
  if(fwrite(&header,sizeof(Header),1,file)!=1)
    return TRUE;
  return FALSE;
} /* of 'fwriteheader' */

int headersize(char *headername)
{
  return sizeof(int)+sizeof(Header)+strlen(headername);
} /* of 'headersize' */

Bool freadheader(FILE *file,Header *header,Bool *swap, const char *headername, int version)
{
  char *buffer;
  int file_version;
  buffer=(char *)malloc(strlen(headername)+1);
  if(fread(buffer,strlen(headername),1,file)!=1){
    free(buffer);
    return TRUE;
  }
  buffer[strlen(headername)]='\0';
  if(strcmp(buffer,headername)){
    free(buffer);
    return TRUE;
  }
  free(buffer);
  if(fread(&file_version,sizeof(file_version),1,file)!=1)
    return TRUE;
  if((file_version & 0xff)==0){
    *swap=TRUE;
    file_version=swapint(file_version);
  }else{
    *swap=FALSE;
  }
  if (file_version!=version)
    return TRUE;
  if(fread(header,sizeof(Header),1,file)!=1)
    return TRUE;
  if(*swap){
    header->order=swapint(header->order);
    header->firstyear=swapint(header->firstyear);
    header->nyear=swapint(header->nyear);
    header->firstcell=swapint(header->firstcell);
    header->ncell=swapint(header->ncell);
    header->nbands=swapint(header->nbands);
  }
  return FALSE;
} /* of 'freadheader' */
