/* CAT2BSQ */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "types.h"

#define SHORT 1
#define FLOAT 0
#define DOUBLE 2
#define INT 3
#define CHAR 4

int main(int argc,char **argv)
{
  FILE *ofp;
  int i,k,p,*n,runs,bands;

  char infile[1000],ofname[1000];
  struct stat filestat;
  double remainder,integer;

  if(argc!=7){
    fprintf(stderr,"USAGE: cat2bsq infilename infilename-ending(out,bip,...) outfile number-of-runs number-of-bands data-type(float=0,short=1,double=2,int=3,char=4)\n");
    return EXIT_FAILURE;
  }

  runs=atoi(argv[4]);
  bands=atoi(argv[5]);
  k=atoi(argv[6]);

  FILE *ifp[runs];
  n=(int *)malloc(runs*sizeof(int));

  sprintf(ofname,"%s",argv[3]);

  ofp=fopen(ofname,"wb");
  if(ofp==NULL){
    fprintf(stderr,"Error opening '%s': %s\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }

  for(i=0;i<runs;i++){
    sprintf(infile,"%s%d.%s",argv[1],i,argv[2]);

    if(!(ifp[i]=fopen(infile,"rb")))
      fprintf(stderr,"Warning: File open failed on path %s\n",infile);
  }

  for(p=0;p<bands;p++){
    for(i=0;i<runs;i++){
      fstat(fileno(ifp[i]),&filestat);
      switch(k){
        case 0:{
          /* calculate number of pixels in each chunk and store it in array n[i] */
          integer=round(filestat.st_size/(float)bands/(float)sizeof(float));
          remainder=filestat.st_size/(float)bands/(float)sizeof(float)-integer;
          if(remainder>0.00013 || remainder<-0.00013)
            fail("Filesize=%d is not divisible without remainder=%f, bands=%d, size=%d.",
              filestat.st_size,remainder,bands,sizeof(float));
          n[i]=(int)integer;
          if(p==0) printf("run: %d n: %d\n",i,n[i]);
          fseek(ifp[i],sizeof(float)*n[i]*p,SEEK_SET);
          float *data;
          data=(float *)malloc(n[i]*sizeof(float));
          fread(data,sizeof(float),n[i],ifp[i]);
          fwrite(data,sizeof(float),n[i],ofp);
          free(data);
        }
        
        break;

        case 1:{
          remainder=modf(filestat.st_size/(float)bands/(float)sizeof(short),&integer);
          if(remainder>0.00001 || remainder<-0.00001)
            fail("Filesize=%d is not divisible without remainder=%f, bands=%d, size=%d.\n",
              filestat.st_size,remainder,bands,sizeof(short));
          n[i]=(int)integer;
          if(p==0) printf("run: %d n: %d\n",i,n[i]);
          fseek(ifp[i],sizeof(short)*n[i]*p,SEEK_SET);
          short *data;
          data=(short *)malloc(n[i]*sizeof(short));
          fread(data,sizeof(short),n[i],ifp[i]);
          fwrite(data,sizeof(short),n[i],ofp);
          free(data);
        }
       
        break;

        case 2:{
          remainder=modf(filestat.st_size/(float)bands/(float)sizeof(double),&integer);
          if(remainder>0.00001 || remainder<-0.00001)
            fail("Filesize=%d is not divisible without remainder=%f, bands=%d, size=%d.\n",
              filestat.st_size,remainder,bands,sizeof(double));
          n[i]=(int)integer;
          if(p==0) printf("run: %d n: %d\n",i,n[i]);
          fseek(ifp[i],sizeof(double)*n[i]*p,SEEK_SET);
          double *data;
          data=(double *)malloc(n[i]*sizeof(double));
          fread(data,sizeof(double),n[i],ifp[i]);
          fwrite(data,sizeof(double),n[i],ofp);
          free(data);
        }
        
        break;
        
        case 3:{
          remainder=modf(filestat.st_size/(float)bands/(float)sizeof(int),&integer);
          if(remainder>0.00001 || remainder<-0.00001)
            fail("Filesize=%d is not divisible without remainder=%f, bands=%d, size=%d.\n",
              filestat.st_size,remainder,bands,sizeof(int));
          n[i]=(int)integer;
          if(p==0) printf("run: %d n: %d\n",i,n[i]);
          fseek(ifp[i],sizeof(int)*n[i]*p,SEEK_SET);
          int *data;
          data=(int *)malloc(n[i]*sizeof(int));
          fread(data,sizeof(int),n[i],ifp[i]);
          fwrite(data,sizeof(int),n[i],ofp);
          free(data);
        }
        
        break;
        
        case 4:{
          remainder=modf(filestat.st_size/(float)bands/(float)sizeof(char),&integer);
          if(remainder>0.00001 || remainder<-0.00001)
            fail("Filesize=%d is not divisible without remainder=%f, bands=%d, size=%d.\n",
              filestat.st_size,remainder,bands,sizeof(char));
          n[i]=(int)integer;
          if(p==0) printf("run: %d n: %d\n",i,n[i]);
          fseek(ifp[i],sizeof(char)*n[i]*p,SEEK_SET);
          char *data;
          data=(char *)malloc(n[i]*sizeof(char));
          fread(data,sizeof(char),n[i],ifp[i]);
          fwrite(data,sizeof(char),n[i],ofp);
          free(data);
        }
      }      /* Ende switch */
    }
  }
  
  for(i=0;i<runs;i++) fclose(ifp[i]);
  fclose(ofp);
  return EXIT_SUCCESS;
}
