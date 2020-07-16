/* StJ 08/05 returns soil or other input-data of size char */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "date.h"
#include "header.h"
#include "swap.h"

int main(int argc,char **argv)
{
  FILE *ifp;
  FILE *mfp,*soil_file;
  FILE *ofp;
  Header header_grid;
  char str[1000];
  char indir[1000],inname[1000],ofdir[1000],ofname[1000];
  char *soil;
  char rbuf;
  short rbuf1;
  int i,j,k,n,ncell;
  int swap_grid;
  float *lpjlon,*lpjlat,lon,lat;
 
  /* Parse command line */
  /* z.B. "river_sections_input_soil numbered_grid_files lpjgrid_lonlat.clm input_data_of_size_char(no header) output 44 (number_infiles)" */
  if(argc!=6){
    fprintf(stdout,"USAGE: river_sections_input_soil numbered_grid-infile_name lpjgrid_lonlat.clm input_data_of_size_char outfile-directory number_infiles\n");
    exit(99);
  }
 
  /* Open in- & output file */
  if(!(strcpy(indir,argv[1]))){
    fprintf(stderr,"Error reading infile-dir.\n");
    exit(1);
  }

  if((mfp=fopen(argv[2],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on mask-file \n");
    exit(1);
  }   

  if((soil_file=fopen(argv[3],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on mask-file \n");
    exit(1);
  }   

  if(!(strcpy(ofdir,argv[4]))){
    fprintf(stderr,"Error reading outfile-dir.\n");
    exit(1);
  }

  if(!(strcpy(str,argv[5]))){
    fprintf(stderr,"Error reading number of infiles.\n");
    exit(1);
  }
  else n=(short)atof(str);

  /* reading original grid file */
  if(freadheader(mfp,&header_grid,&swap_grid,LPJGRID_HEADER,LPJGRID_VERSION)){
    fclose(mfp);
    fail("Invalid header in original grid file.");
  }

  ncell=header_grid.ncell;
  printf("ncell:%d\n",ncell);

  lpjlat=newvec(float,ncell);
  lpjlon=newvec(float,ncell);
  soil=newvec(char,ncell);

  for(i=0;i<ncell;i++){
    if(fread(&rbuf1,sizeof(short),1,mfp)==1){
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlon[i]=rbuf1/100.;
      fread(&rbuf1,sizeof(short),1,mfp);
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlat[i]=rbuf1/100.;

      /* Reading soil input-file */
      fread(&rbuf,sizeof(char),1,soil_file);
      soil[i]=rbuf;
      /* printf("%d %5.2f %5.2f\n",i,lpjlon[i],lpjlat[i]); */
    }
    else{
      fprintf(stderr,"Error reading lpjgridori.\n");
      exit(1);
    }
  }
  fclose(mfp);
  fclose(soil_file);

  for(i=0;i<n;i++){
    sprintf(ofname,"%s%d.bin",ofdir,i);
    if(!(ofp=fopen(ofname,"wb"))){         
      printf("Warning: file open failed on outfile number %d.\n",i); 
      exit(1);
    } 

    sprintf(inname,"%s%d.bin",indir,i);
    if(!(ifp=fopen(inname,"rb"))){         
       printf("Warning: file open failed on infile number %d.\n",i); 
       exit(1);
    } 

    if(freadheader(ifp,&header_grid,&swap_grid,LPJGRID_HEADER,LPJGRID_VERSION)){
      fclose(ifp);
      fail("Invalid header in '%s'.",inname);
    }

    for(j=0;j<header_grid.ncell;j++){
      /* read input file for longitude and latitude */
      fread(&rbuf1,sizeof(short),1,ifp);
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lon=rbuf1/100.;
      fread(&rbuf1,sizeof(short),1,ifp);
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lat=rbuf1/100.;
      /* printf("%d %5.2f %5.2f\n",j,lon,lat); */
      for(k=0;k<ncell;k++)
        if(lat==lpjlat[k] && lon==lpjlon[k]){
          /* printf("j=%d k=%d soil=%d\n",j,k,soil[k]); */
          fwrite(&soil[k],sizeof(char),1,ofp);
          break;
        }
    }
    fclose(ifp);
    fclose(ofp);
  }
  exit(0);
}
