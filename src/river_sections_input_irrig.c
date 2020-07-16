/* StJ 07/06 returns irrigation input-data */
/* search for water in one of the 8 neighboring pixel as follows: 
   choose neighbor pixel with the most water input from other pixels */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "date.h"
#include "header.h"
#include "swap.h"

#define NCOLS 720
#define NROWS 360
#define WIDTH 0.5
#define XCORNER -180
#define YCORNER 90

void sort(int feld0[],int feld1[],int feld2[],int len){
  int i,j,min,tmp;

  for(i=0;i<len-1;i++){
    min=i;
    for(j=i+1;j<len;j++)
      if(feld0[j]<feld0[min]) 
        min=j;
    
    tmp=feld0[min];
    feld0[min]=feld0[i];
    feld0[i]=tmp;

    tmp=feld1[min];
    feld1[min]=feld1[i];
    feld1[i]=tmp;

    tmp=feld2[min];
    feld2[min]=feld2[i];
    feld2[i]=tmp;    
  }
}

int main(int argc,char **argv)
{  
  FILE *gfp;
  FILE *mfp;
  FILE *ofp;
  Header header,header_grid;
  char str[1000];
  char ofdir[1000],indir[1000];
  char ofname[1000],inname[1000];
  int ilat,ilon,ilat_n[8],ilon_n[8];
  int i,j,k,ncell,cell,n;  
  int swap_grid;
  int neighb[8];
  short rbuf;
  short river[NROWS][NCOLS];
  short cell_n;
  float *lon,*lat; 
  float lat_n,lon_n;

  header.nyear=0;
  header.firstyear=0;
  header.order=0;
  header.firstcell=0;
  header.nbands=1;
 
/* Parse command line */
/* z.B. "river_sections_input_irrig numbered_grid_files vec_drain_river.bip outfile-directory/neighb_irr_pix 44 (number-infiles)" */
  if(argc!=5){
    fprintf(stdout,"USAGE: river_sections_input_irrig numbered_grid-infile_name vec_drain_river.bip outfile-directory number-infiles\n");
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

  if(!(strcpy(ofdir,argv[3]))){
    fprintf(stderr,"Error reading outfile-dir.\n");
    exit(1);
  }

  if(!(strcpy(str,argv[4]))){
    fprintf(stderr,"Error reading number of infiles.\n");
    exit(1);
  }
  else n=(short)atof(str);

  for(i=0;i<NROWS;i++){
    fread(river[i],sizeof(short),NCOLS,mfp);
    /* printf("i:%d %d\n",i,river[i][180]); */
  }
  fclose(mfp);

  for(i=0;i<n;i++){
    printf("file number=%d\n",i);
    sprintf(ofname,"%s%d.bin",ofdir,i);
    if(!(ofp=fopen(ofname,"wb"))){
       fprintf(stderr,"Warning: file open failed on outfile\n"); 
       exit(1);
    } 

    sprintf(inname,"%s%d.bin",indir,i);
    if(!(gfp=fopen(inname,"rb"))){         
       printf("Warning: file open failed on infile number %d.\n",i); 
       exit(1);
    } 

    /* HEADER */
    if(freadheader(gfp,&header_grid,&swap_grid,LPJGRID_HEADER,LPJGRID_VERSION)){
      fclose(gfp);
      fail("Invalid header in '%s'.",inname);
    }

    ncell=header_grid.ncell;
    header.ncell=ncell;

    /* write header */
    fwriteheader(ofp,header,LPJNEIGHB_IRRIG_HEADER,LPJNEIGHB_IRRIG_VERSION);
    lat=newvec(float,ncell);
    lon=newvec(float,ncell);

    for(cell=0;cell<ncell;cell++){
      if(fread(&rbuf,sizeof(short),1,gfp)==1){
        if(swap_grid) rbuf=swapshort(rbuf);
        lon[cell]=rbuf/100.;
        fread(&rbuf,sizeof(short),1,gfp);
        if(swap_grid) rbuf=swapshort(rbuf);
        lat[cell]=rbuf/100.;
        /* printf("c:%d %5.2f %5.2f\n",cell,lon[cell],lat[cell]); */
      }
      else{
        fprintf(stderr,"Error reading grid.\n");
        exit(1);
      }
    }
    fclose(gfp);

    for(cell=0;cell<ncell;cell++){
      /* determine ilatitude and ilongitude of current position */
      ilat=(YCORNER+0.25-lat[cell])/WIDTH-1;
      ilon=(lon[cell]-XCORNER+0.25)/WIDTH-1;
      /* printf("A c:%d lon:%d %5.2f lat:%d %5.2f\n",cell,ilon,lon[cell],ilat,lat[cell]); */

      for(j=0;j<8;j++){ 
        neighb[j]=0;
        ilat_n[j]=ilat;
        ilon_n[j]=ilon;
      }

      if((ilat-1)>=0 && (ilon-1)>=0 && river[ilat-1][ilon-1]>0){
        neighb[0]=river[ilat-1][ilon-1];
        ilat_n[0]=ilat-1;
        ilon_n[0]=ilon-1;
      }

      if((ilat-1)>=0 && river[ilat-1][ilon]>0){
        neighb[1]=river[ilat-1][ilon];
        ilat_n[1]=ilat-1;
        ilon_n[1]=ilon;
      }

      if((ilat-1)>=0 && (ilon+1)<NCOLS && river[ilat-1][ilon+1]>0){
        neighb[2]=river[ilat-1][ilon+1];
        ilat_n[2]=ilat-1;
        ilon_n[2]=ilon+1;
      }

      if((ilon-1)>=0 && river[ilat][ilon-1]>0){
        neighb[3]=river[ilat][ilon-1];
        ilat_n[3]=ilat;
        ilon_n[3]=ilon-1;
      }

      if((ilon+1)<NCOLS && river[ilat][ilon+1]>0){
        neighb[4]=river[ilat][ilon+1];
        ilat_n[4]=ilat;
        ilon_n[4]=ilon+1;
      }

      if((ilat+1)<NROWS && (ilon-1)>=0 && river[ilat+1][ilon-1]>0){
        neighb[5]=river[ilat+1][ilon-1];
        ilat_n[5]=ilat+1;
        ilon_n[5]=ilon-1;
      }

      if((ilat+1)<NROWS && river[ilat+1][ilon]>0){
        neighb[6]=river[ilat+1][ilon];
        ilat_n[6]=ilat+1;
        ilon_n[6]=ilon;
      }

      if((ilat+1)<NROWS && (ilon+1)<NCOLS && river[ilat+1][ilon+1]>0){
        neighb[7]=river[ilat+1][ilon+1];
        ilat_n[7]=ilat+1;
        ilon_n[7]=ilon+1;
      }

      sort(neighb,ilat_n,ilon_n,8);

      cell_n=-9;
      for(j=7;j>=0;j--){
        lat_n=YCORNER+0.25-WIDTH*(ilat_n[j]+1);
        lon_n=XCORNER-0.25+WIDTH*(ilon_n[j]+1);
        /* printf("C c:%d lon:%d %5.2f lat:%d %5.2f\n",cell,ilon_n[j],lon_n,ilat_n[j],lat_n); */
        for(k=0;k<ncell;k++)
          if(lat_n==lat[k] && lon_n==lon[k]){ 
            cell_n=k;
            break;
          }
        if(cell_n>=0) break;
      }

      /* printf("c:%d c_n:%d\n",cell,cell_n); */
      if(cell_n==-9) cell_n=cell;

      fwrite(&cell_n,sizeof(short),1,ofp); 
    } /* of cell loop */
    fclose(ofp);
    free(lat);
    free(lon);
  }

  exit(0);
}
