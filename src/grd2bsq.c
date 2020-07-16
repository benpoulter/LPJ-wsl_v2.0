/* transforms LPJ-output to image of BSQ-format */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc,char **argv)
{  
  char str[1000];
  short rbuf;
  int i;
  float lon;
  float lat;
  float data0;
  short data1;
  double data2;
  int data3;
  char data4;
  FILE *ifp;
  FILE *ofp;
  FILE *gfp;
  float slat, elat, slon, elon;
  int npix,nrec,nyrs,type;
  int ilat,ilon;
  float res;
  int **lw;
  int ip,ny,sy,ey;
  int bytel;
  
  if(argc!=15){
    fprintf(stdout,"Usage:\n");
    fprintf(stdout,"grd2bsq infilename outfilename gridfilename output-start-lon output-end-lon output-start-lat output-end-lat output-start-year(1) output-end-year(103) numberpixels resolution-in-degs number-records-per-pixel number-years data-type(float=0,short=1,double=2,int=3,char=4)\n");
    fprintf(stdout,"argc is: %d\n",argc);
    puts("TEST 1");
    exit(1);
  }

  puts("1");
  
  /* PARSE COMMAND LINE */
  if((ifp=fopen(argv[1],"rb"))==NULL){
    fprintf(stderr,"Warning: File open failed on input-file.\n");
    exit(1);
  }
  if((ofp=fopen(argv[2],"wb"))==NULL){
    fprintf(stderr,"Warning: File open failed on output-file.\n");
    exit(1);
  }
  if((gfp=fopen(argv[3],"rb"))==NULL){
    fprintf(stderr,"Warning: File open failed on grid-file.\n");
    exit(1);
  }

  if(!(strcpy(str,argv[4]))){
     fprintf(stderr,"Error reading output start longitude.\n");
     exit(99);
  }
  else slon=atof(str);

  if(!(strcpy(str,argv[5]))){
     fprintf(stderr,"Error reading output end longitude.\n");
     exit(99);
  }
  else elon=atof(str);

  if(!(strcpy(str,argv[6]))){
     fprintf(stderr,"Error reading output start latitude.\n");
     exit(99);
  }
  else slat=atof(str);

  if(!(strcpy(str,argv[7]))){
     fprintf(stderr,"Error reading output end latitude.\n");
     exit(99);
  }
  else elat=atof(str);

  if(!(strcpy(str,argv[8]))){
     fprintf(stderr,"Error reading output start year.\n");
     exit(99);
  }
  else sy=(int)atof(str);

  if(!(strcpy(str,argv[9]))){
     fprintf(stderr,"Error reading output end year.\n");
     exit(99);
  }
  else ey=(int)atof(str);

  if(!(strcpy(str,argv[10]))){
     fprintf(stderr,"Error reading number of pixels.\n");
     exit(99);
  }
  else npix=(int)atof(str);

  if(!(strcpy(str,argv[11]))){
     fprintf(stderr,"Error reading resolution in degrees.\n");
     exit(99);
  }
  else res=atof(str);

  if(!(strcpy(str,argv[12]))){
     fprintf(stderr,"Error reading number of records per pixel per year.\n");
     exit(99);
  }
  else nrec=(int)atof(str);

  if(!(strcpy(str,argv[13]))){
     fprintf(stderr,"Error reading number of years.\n");
     exit(99);
  }
  else nyrs=(int)atof(str);

  if(!(strcpy(str,argv[14]))){
     fprintf(stderr,"Error reading type size: 0 (for float),1 (for short),2 (for double),3 (for int),4 (for char).\n");
     exit(99);
  }
  else type=(int)atof(str);

  if(type<0 || type>4){
     fprintf(stderr,"Type should be 0 (for float),1 (for short),2 (for double),3 (for int),4 (for char).\n");
     exit(99);
  }

  /* puts("bef alloc"); */
  
  lw=(int **)calloc((int)(180./res),sizeof(int *));
  for(i=0;i<(int)(180./res);i++) 
    lw[i]=(int *)calloc((int)(360./res),sizeof(int));

  /*--------------------------------------------------*/

  for(ilon=0;ilon<(int)(360./res);ilon++){
    for(ilat=0;ilat<(int)(180./res);ilat++){
      lw[ilat][ilon]=-1.;
    }
  }

  for(ip=0;ip<npix;ip++){
    fread(&rbuf,sizeof(short),1,gfp);
    lon=rbuf/100.;
    /* printf("lon %.2f ",lon); */
    fread(&rbuf,sizeof(short),1,gfp);
    lat=rbuf/100.;
    /* printf("lat %.2f\n",lat); */

    ilon=(int)((lon+180.)/res+0.01);
    ilat=(int)((lat+90.)/res+0.01);

    lw[ilat][ilon]=ip;
  }
  fclose(gfp);

  if(type==0) bytel=sizeof(float);
  if(type==1) bytel=sizeof(short);
  if(type==2) bytel=sizeof(double);
  if(type==3) bytel=sizeof(int);
  if(type==4) bytel=sizeof(char);

  ny=(ey-sy)+1;

  for(i=0;i<ny*nrec;i++){
    if(i%nrec==0)printf("%d\n",i+nrec);
    /* Reverse latitude order for easy displaying */
    for(ilat=(int)(180./res)-1;ilat>=0;ilat--){
      for(ilon=0;ilon<(int)(360./res);ilon++){
  
  lon=ilon*res-180.+res/2.;
  lat=ilat*res-90.+res/2.;

  if(lon>=slon && lon<=elon && lat>=slat && lat<=elat){
    ip=lw[ilat][ilon];
    if(ip!=-1){  /* data present */
      fseek(ifp,ip*bytel+i*bytel*npix+(sy-1)*bytel*nrec*npix,SEEK_SET);
      if(type==0){
        fread(&data0,sizeof(float),1,ifp);
        puts("TEST");
      }
      if(type==1)
        fread(&data1,sizeof(short),1,ifp);
      if(type==2)
        fread(&data2,sizeof(double),1,ifp);
      if(type==3)
        fread(&data3,sizeof(int),1,ifp);
      if(type==4)
        fread(&data4,sizeof(char),1,ifp);
    }
    else{ /* ocean or lake or fill */
      if(type==0) data0=-9;
      if(type==1) data1=-9;
      if(type==2) data2=-9;
      if(type==3) data3=-9;
      if(type==4) data4=0;
    }
    /* Write output */
    if(type==0) fwrite(&data0,sizeof(float),1,ofp); 
    if(type==1) fwrite(&data1,sizeof(short),1,ofp); 
    if(type==2) fwrite(&data2,sizeof(double),1,ofp); 
    if(type==3) fwrite(&data3,sizeof(int),1,ofp); 
    if(type==4) fwrite(&data4,sizeof(char),1,ofp); 
  } /* end: if in window */
      } /* end: lat loop over global grid */
    } /* end: lon loop over global grid */
  } /* end of for(...) */

  fclose(ifp);
  fclose(ofp);

  exit(0);
}
