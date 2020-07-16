/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  s r c / h e a d e r g r i d . c                              **/
/**                                                               **/
/**  Converts CRU data files into LPJ climate data files          **/
/*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "types.h"
#include "swap.h"
#include "date.h"
#include "buffer.h"
#include "coord.h"
#include "config.h"
#include "climate.h"
#include "header.h"

#define NCELL 67420

#define headergrid_VERSION "1.0.001"
#define USAGE    "Usage: headergrid [-h] [-rev] gridfile outfile\n"

int main(int argc,char **argv)
{
  FILE *file;
  short *data;
  int i,n,k;
  Header header;
  Bool revorder;
  struct stat filestat;
  revorder=FALSE;
  header.nyear=0;
  header.firstyear=0;
  header.order=0;
  header.firstcell=0;
  header.ncell=NCELL;
  header.nbands=2;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("headergrid " headergrid_VERSION " (" __DATE__ ") - adds header to gridfile for lpj C version\n");
        printf(USAGE
               "-h               print this help text\n" 
               "-rev             change byte order in gridfile\n"
               "gridfile          filename of griddata file"
               "outfile          filename of griddata file");
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[i],"-rev"))
        revorder=TRUE;
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n",argv[i]);
        fprintf(stderr,USAGE);
        return EXIT_FAILURE;
      }
    }
    else 
      break;
  if(argc<i+2)
  {
    fprintf(stderr,"Filenames missing.\n");
    fprintf(stderr,USAGE);
    return EXIT_FAILURE;
  }
  file=fopen(argv[i],"r");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[i],strerror(errno));
    return EXIT_FAILURE;
  }
  fstat(fileno(file),&filestat);
  n=filestat.st_size/sizeof(short);
  printf("Number of cells: %d\n",n/2);
  data=(short *)malloc(n*sizeof(short));
  fread(data,sizeof(short),n,file);
  if(revorder)
    for(k=0;k<n;k++)
      data[k]=swapshort(data[k]); 
  fclose(file);
  file=fopen(argv[i+1],"w");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  header.ncell=n/2;
  fwriteheader(file,header,LPJGRID_HEADER,LPJGRID_VERSION);
  fwrite(data,sizeof(short),n,file);
  free(data);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */
