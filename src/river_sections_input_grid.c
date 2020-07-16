/* StJ 09/05 combining eventually several river-sections to data-sets of a 
   specific length which include longitude and latitude for the grid cells */
/* ATTENTION: to generate an optimal data set with largest computation time
              for largest river basin (Amazonas) manual manipulation for 
              single river basins with large computation time*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "date.h"
#include "header.h"
#include "swap.h"

#define KONST 2000 /* maximal length of the data-set */
#define NYEAR 103      /* default value for number of years */
#define FIRSTYEAR 1901 /* default value for first year */
#define FIRSTCELL 0
#define ORDER CELLYEAR


int main(int argc,char **argv)
{
  FILE *ifp1,*ifp2;
  FILE *ofp;
  Header header,header_grid;
  char ofdir[1000];
  char ofname[1000];
  int i, j, k, l, c, c0;  
  int ncell;
  int rbuf0;
  int *rivers;
  int value;
  int swap_grid;
  float *lpjlon,*lpjlat;
  short *riversnew;
  short *sum;
  short rbuf1;

  /* header informations */
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=ORDER;
  header.firstcell=FIRSTCELL;
  header.nbands=2;
 
  /* Parse command line */
  /* z.B. "river_sections_input_grid lpjgrid_lonlat.clm riversections.bin output (outfile-directory)" */
  if(argc!=4){
    fprintf(stdout,"USAGE: river_sections_input_grid lpjgrid_lonlat.clm riversections.bin output (outfile-directory)\n");
    exit(99);
  }
 
  /* Open in- & output file */
  if((ifp1=fopen(argv[1],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on data-file1 \n");
    exit(1);
  } 

  if((ifp2=fopen(argv[2],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on data-file2 \n");
    exit(1);
  } 

  if(!(strcpy(ofdir,argv[3]))){
    fprintf(stderr,"Error reading outfile-dir.\n");
    exit(1);
  }

  /* reading original grid file */
  if(freadheader(ifp1,&header_grid,&swap_grid,LPJGRID_HEADER,LPJGRID_VERSION)){
    fclose(ifp1);
    fail("Invalid header in original grid file.");
  }

  ncell=header_grid.ncell;

  lpjlat=newvec(float,ncell);
  lpjlon=newvec(float,ncell);
  rivers=newvec(int,ncell);
  riversnew=newvec(short,ncell);
  sum=newvec(short,ncell);

  for(i=0;i<ncell;i++){
    /* read input file and make array for number, longitude and latitude */
    if(fread(&rbuf1,sizeof(short),1,ifp1)==1){
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlon[i]=rbuf1/100.;
      fread(&rbuf1,sizeof(short),1,ifp1);
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlat[i]=rbuf1/100.;
      /* printf("%5.2f %5.2f\n",lpjlon[i],lpjlat[i]); */
    }
    if(fread(&rbuf0,sizeof(int),1,ifp2)==1){
      rivers[i]=rbuf0;
      /* printf("%d ",rivers[i]); */
    }
  }  
  fclose(ifp1);
  fclose(ifp2);
  
  /* initialization */
  for(i=0;i<ncell;i++) { 
    riversnew[i]=-99;
    sum[i]=-99;
  }
    
  /* changing the number for seperate river-sections, beginning at 1 */
  c0=0;
  value=1;  
  for(i=-1;i<ncell;i++){
    for(j=0;j<ncell;j++){
      if(rivers[j]==i){  
        riversnew[j]=value;
        c0+=1;
      }
    }
    if(c0>0){
      sum[value]=c0;
      value+=1;
      c0=0;
    }
  }

  /* generating the output data-sets with a length < KONST */
  k=1;
  l=c=c0=0;
  rbuf1=0;
  while(sum[k]!=-99){
    sprintf(ofname,"%s%d.bin",ofdir,l);
    if(!(ofp=fopen(ofname,"wb"))){         
      fprintf(stderr,"Warning: file open failed on outfile\n"); 
      exit(1);
    }

    fseek(ofp,headersize(LPJGRID_HEADER),SEEK_SET);

    if(c==0 && sum[k]>KONST){
      for(i=0;i<ncell;i++){
        if(riversnew[i]==k){
          /* fprintf(ofp,"%d %d\n",(int)(lpjlon[i]*100.),(int)(lpjlat[i]*100.)); */
          /* write binary file */
          rbuf1=(short)(lpjlon[i]*100.);
          fwrite(&rbuf1,sizeof(short),1,ofp);
          rbuf1=(short)(lpjlat[i]*100.);
          fwrite(&rbuf1,sizeof(short),1,ofp);
          c0+=1;
        }
      }
      k+=1;
    } 
    else{
      while((c+sum[k])<=KONST && sum[k]!=-99){
        for(i=0;i<ncell;i++){
          if(riversnew[i]==k){
            /* fprintf(ofp,"%d %d\n",(int)(lpjlon[i]*100.),(int)(lpjlat[i]*100.)); */
            /* write binary file */
            rbuf1=(short)(lpjlon[i]*100.);
            fwrite(&rbuf1,sizeof(short),1,ofp);
            rbuf1=(short)(lpjlat[i]*100.);
            fwrite(&rbuf1,sizeof(short),1,ofp);
            c0+=1;
          }
        }
        c+=sum[k];
        k+=1;
      }
    }
    printf("c=%d k=%d sum[k]=%d\n",c,k,sum[k]);

    /* HEADER */
    header.ncell=c0;
    fseek(ofp,0,SEEK_SET);
    fwriteheader(ofp,header,LPJGRID_HEADER,LPJGRID_VERSION);

    fclose(ofp);
    l+=1;
    c=c0=0;
  }
  exit(0);
}
