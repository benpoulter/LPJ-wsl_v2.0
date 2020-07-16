/* StJ 08/05 returns vector of cells for drainage */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "date.h"
#include "header.h"

#define NYEAR 103      /* default value for number of years */
#define FIRSTYEAR 1901 /* default value for first year */
#define FIRSTCELL 0
#define ORDER CELLYEAR

/* Calculate distance (in km) between two points specified by latitude/longitude */
float distHaversine(float lat1, 
                    float lat2,
                    float lon1,
                    float lon2) 
{  /* Haversine version */
  int R = 6371; /* earth's mean radius in km */
  double dLat  = lat2-lat1;
  double dLong = lon2-lon1;

  double a = sin(dLat/2)*sin(dLat/2)+cos(lat1)*cos(lat2)*sin(dLong/2)*sin(dLong/2);
  double c = 2*atan2(sqrt(a),sqrt(1-a));
  int d = R*c;

  return d;
}

int main(int argc,char **argv)
{  
  FILE *ifp;
  FILE *mfp;
  FILE *ofp;
  Header header,header_grid;
  char str[1000];
  char indir[1000],inname[1000],ofdir[1000],ofname[1000];
  int nodata;
  int cols, rows, xcorner, ycorner;
  int ilat, ilon, i, j, l, c1, c2, c3, c4;  
  int value;
  int swap;
  short n;
  short *nr, *drain_cell;
  short rbuf1;
  float width;
  float lon, lat, *lpjlon, *lpjlat; 
  float lonnew, latnew;
  short *d; 
  
  /* header informations */
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=ORDER;
  header.firstcell=FIRSTCELL;
  header.nbands=NMONTH;
  
  /* Parse command line */
  /* z.B. "drainage output/grid DDM30.asc output 44 (number_infiles)" */
  if(argc!=5){
    fprintf(stdout,"USAGE: drainage infile-directory DDM30.asc outfile-directory number_infiles\n");
    exit(99);
  }
 
  /* Open in- & output file */
  if(!(strcpy(indir,argv[1]))){
    fprintf(stderr,"Error reading infile-dir.\n");
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


  for(l=0;l<n;l++){
    if((mfp=fopen(argv[2],"r")) == NULL){
      fprintf(stderr,"Warning: File open failed on mask-file \n");
      exit(1);
    } 
    /* read lines of mask file */
    fscanf(mfp,"%s %d",str,&cols);
    /* printf("cols=%d\n",cols); */
    fscanf(mfp,"%s %d",str,&rows);
    /* printf("rows=%d\n",rows); */
    fscanf(mfp,"%s %d",str,&xcorner);
    fscanf(mfp,"%s %d",str,&ycorner);
    fscanf(mfp,"%s %f",str,&width); 
    /* printf("cellsize=%f\n",width); */
    fscanf(mfp,"%s %d",str,&nodata);

    sprintf(inname,"%s%d.bin",indir,l);
    if(!(ifp=fopen(inname,"rb"))){         
       printf("Warning: file open failed on infile number %d.\n",l); 
       exit(1);
    } 

    if(freadheader(ifp,&header_grid,&swap,LPJGRID_HEADER,LPJGRID_VERSION)){
      fclose(ifp);
      fail("Invalid header in '%s'.",inname);
    }

    fseek(ifp,headersize(LPJGRID_HEADER),SEEK_SET);

    nr=newvec(short,header_grid.ncell);
    drain_cell=newvec(short,header_grid.ncell);
    lpjlon=newvec(float,header_grid.ncell);
    lpjlat=newvec(float,header_grid.ncell);
    d=newvec(short,header_grid.ncell);

    for(i=0;i<header_grid.ncell;i++){
      /* initialisation of output array */  
      drain_cell[i]=-1;
      d[i]=0;
      nr[i]=i;
      /* read input file and make array for longitude and latitude */
      fread(&rbuf1,sizeof(short),1,ifp);
      lpjlon[i]=rbuf1/100.;
      fread(&rbuf1,sizeof(short),1,ifp);
      lpjlat[i]=rbuf1/100.;
      /* printf("%d %5.2f %5.2f\n",nr[i],lpjlon[i],lpjlat[i]); */
    }
    fclose(ifp);

    c1=c2=c3=c4=0;
    for(ilat=rows-1;ilat>=0;ilat--){
      /* printf("ilon= %d\n",ilon); */
      for(ilon=0;ilon<cols;ilon++){
        /* read discrete values of mask file */
        fscanf(mfp,"%d",&value);
        /* printf("%d %d %d\n",ilat,ilon,value); */
        c1=c1+1;
        /* determine latitude and longitude of current position */
        lat=ycorner+ilat*width+0.25;
        lon=xcorner+ilon*width+0.25;
        /* printf("%d %5.2f %5.2f\n",c1,lon,lat); */
  
        for (i=0;i<header_grid.ncell;i++){
          /* comparison if current position in input-file  */
          if (lat==lpjlat[i] && lon==lpjlon[i]){
            if(value==-9) drain_cell[i]=-9;
            if(value==0){ 
              drain_cell[i]=-1;
              c4+=1;
              /* determine the distance between the two points*/
              d[i]=25; 
                    /* printf("%ld\n",d[i]); */
            }

            c2=c2+1;
            /* printf("%d %5.2f %5.2f\n",c2,lon,lat); */
            /* only values > 0 of interesting  */
            if(value>0){ 
              if(value<=8) {
                if(value<=2) {
                  if(value==1) {
                    latnew=lat;
                    lonnew=lon+width;
                    c3=c3+1;
                  }else{
                    latnew=lat-width;
                    lonnew=lon+width;
                    c3=c3+1;
                  }
                }else{
                  if(value==4) {
                    latnew=lat-width;
                    lonnew=lon;
                    c3=c3+1;                  
                  }else{
                    latnew=lat-width;
                    lonnew=lon-width;
                    c3=c3+1;                 
                  }
                }
              }else{
                if(value<=32) {
                  if(value==16) {
                    latnew=lat;
                    lonnew=lon-width;
                    c3=c3+1;                  
                  }else{
                    latnew=lat+width;
                    lonnew=lon-width;
                    c3=c3+1;          
                  }
                }else{
                  if(value==64) {
                    latnew=lat+width;
                    lonnew=lon;
                    c3=c3+1;                
                  }else{
                    latnew=lat+width;
                    lonnew=lon+width;
                    c3=c3+1;                 
                  }   
                }
              }
              
              for(j=0;j<header_grid.ncell;j++){
                if(latnew==lpjlat[j] && lonnew==lpjlon[j]){
                  drain_cell[i]=nr[j];         
                  /* determine the distance between the two points*/
                  d[i]=distHaversine(lat*M_PI/180,latnew*M_PI/180,lon*M_PI/180,lonnew*M_PI/180); 
                }
              }
            }    /* end of if value>0 */
          }      /* end of if lat==lpjlat && lon==lpjlon */  
        }        /* end of for (i=0;i<NCELL;i++) */
      }          /* end of ilon-loop */
    }            /* end of ilat-loop */
    fclose(mfp);
    printf("c1= %d c2= %d c3= %d c4= %d\n",c1,c2,c3,c4); 

    sprintf(ofname,"%s%d.bin",ofdir,l);
    if(!(ofp=fopen(ofname,"wb"))){         
      printf("Warning: file open failed on outfile number %d.\n",l); 
      exit(1);
    } 

    /* HEADER */
    header.ncell=header_grid.ncell;
    fwriteheader(ofp,header,LPJDRAIN_HEADER,LPJDRAIN_VERSION);

    /* write data on the screen  */
    for(i=0;i<header.ncell;i++) { 
      /* printf("d=%d l=%ld\n", drain_cell[i],d[i]); */
      /* write binary file */
      fwrite(&drain_cell[i],sizeof(short),1,ofp);
      fwrite(&d[i],sizeof(short),1,ofp);
    }
    fclose(ofp);
    free(nr);
    free(drain_cell);
    free(lpjlon);
    free(lpjlat);
    free(d);
  }
  exit(0);
}
