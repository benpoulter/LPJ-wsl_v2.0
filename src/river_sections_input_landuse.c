/* StJ 10/05 returns landuse input-data */

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
  FILE *landuse_file;
  FILE *mfp;
  FILE *ofp;
  Header header,header_grid;
  char str[1000];
  char ofdir[1000],indir[1000];
  char ofname[1000],inname[1000];
  int i,j,k,l,m,n,ncell;  
  short rbuf1;
  short *rbuf2;
  float lon,lat,*lpjlon,*lpjlat; 
  int swap,swap_grid;
 
  /* Parse command line */
  /* z.B. "river_sections_input_landuse numbered_grid_files lpjgrid_lonlat.clm "luc".clm output 44 (number-infiles)" */
  if(argc!=6){
    fprintf(stdout,"USAGE: river_sections_input_landuse numbered_grid-infile-name lpjgrid_lonlat.clm luc.clm outfile-directory number-infiles\n");
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

  if((landuse_file=fopen(argv[3],"rb")) == NULL){
    fprintf(stderr,"Warning: File open failed on landuse-file \n");
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

  if(freadheader(landuse_file,&header,&swap,LPJ_LANDUSE_HEADER,LPJ_LANDUSE_VERSION)){
    fclose(landuse_file);
    fail("Invalid header in landuse-infile.");
  }
  if(header.order!=CELLYEAR){
    fclose(landuse_file);
    fail("Order in landuse-infile not CELLYEAR.");
  }
  ncell=header.ncell;

  lpjlat=newvec(float,ncell);
  lpjlon=newvec(float,ncell);
  rbuf2=newvec(short,header.nbands);

  /* reading original grid file */
  if(freadheader(mfp,&header_grid,&swap_grid,LPJGRID_HEADER,LPJGRID_VERSION)){
    fclose(mfp);
    fail("Invalid header in original grid file.");
  }  

  for(i=0;i<ncell;i++){
    if(fread(&rbuf1,sizeof(short),1,mfp)==1){
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlon[i]=rbuf1/100.;
      fread(&rbuf1,sizeof(short),1,mfp);
      if(swap_grid) rbuf1=swapshort(rbuf1);
      lpjlat[i]=rbuf1/100.;
      /* printf("%d %5.2f %5.2f\n",i,lpjlon[i],lpjlat[i]); */
    }
    else{
      fprintf(stderr,"Error reading lpjgridori.\n");
      exit(1);
    }
  }
  fclose(mfp);

  for(i=0;i<n;i++){
    printf("file number=%d\n",i);
    sprintf(ofname,"%s%d.clm",ofdir,i);
    if(!(ofp=fopen(ofname,"wb"))){         
       fprintf(stderr,"Warning: file open failed on outfile\n"); 
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

    /* HEADER */
    header.ncell=header_grid.ncell;
    fwriteheader(ofp,header,LPJ_LANDUSE_HEADER,LPJ_LANDUSE_VERSION);

    for(j=0;j<header.nyear;j++){
      /* printf("j=%d\n",j); */
      fseek(ifp,headersize(LPJGRID_HEADER),SEEK_SET);    
      for(k=0;k<header_grid.ncell;k++) {
        if(fread(&rbuf1,sizeof(short),1,ifp)==1){
          if(swap_grid) rbuf1=swapshort(rbuf1);
          lon=rbuf1/100.;
          fread(&rbuf1,sizeof(short),1,ifp);
          if(swap_grid) rbuf1=swapshort(rbuf1);
          lat=rbuf1/100.;
          /* printf("%d %5.2f %5.2f\n",k,lon,lat); */
        }
        else{
          fprintf(stderr,"Error reading lpjgrid.\n");
          exit(1);
        }

        for(l=0;l<ncell;l++){
          if(lat==lpjlat[l] && lon==lpjlon[l]){
            fseek(landuse_file,j*ncell*sizeof(short)*header.nbands+header.nbands*sizeof(short)*l+headersize(LPJ_LANDUSE_HEADER),SEEK_SET);
            fread(rbuf2,sizeof(short),header.nbands,landuse_file);
            if(swap)
              for(m=0;m<header.nbands;m++)
                rbuf2[m]=swapshort(rbuf2[m]);
            fwrite(rbuf2,sizeof(short),header.nbands,ofp);
            break;
          }
        }
      }
    }
    fclose(ifp);
    fclose(ofp);
  }
  fclose(landuse_file);
  free(lpjlat);
  free(lpjlon);
  free(rbuf2);
  exit(0);
}
