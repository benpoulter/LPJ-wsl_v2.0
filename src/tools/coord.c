/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ c o o r d . c                                     **/
/*******************************************************************/
#include "lpj.h"

typedef struct
{
  short lon,lat;
} Icoord;

Coordfile *opencoord(const char *filename /* filename of coord file */)           
{
  Coordfile *coordfile;
  Header header;
  
  coordfile=new(Coordfile);
  coordfile->file=fopen(filename,"rb");
  
  if(coordfile->file==NULL){
    free(coordfile);
    return NULL;
  }
  
  if(freadheader(coordfile->file,&header,&coordfile->swap,LPJGRID_HEADER,LPJGRID_VERSION)){
    fclose(coordfile->file);
    free(coordfile);
    return NULL;
  }
  coordfile->n=header.ncell;
  return coordfile;  
} /* of 'opencoord' */

void closecoord(Coordfile *coordfile)
{
  fclose(coordfile->file);
  free(coordfile);
} /* of 'closecoord' */

Bool readcoord(Coordfile *coordfile,
               Coord *coord,         /* cell coordinate read from file */
               Coord resol           /* resolution (deg) */
              )                      /* returns FALSE for successful read */
{
  Icoord icoord;
  if(fread(&icoord,sizeof(icoord),1,coordfile->file)!=1)
    return TRUE;
  if(coordfile->swap){ 
    coord->lat=swapshort(icoord.lat)*0.01;
    coord->lon=swapshort(icoord.lon)*0.01;
  }else{ 
    coord->lat=icoord.lat*0.01;
    coord->lon=icoord.lon*0.01;
  }
  coord->area=cellarea(*coord,resol);
  return FALSE;
} /* of 'getcoord' */

Bool writecoord(FILE *file,  /* file pointer */
                Coord coord  /* cell coordinat written to file */
               )             /* returns FALSE for successful write */
{
  Icoord icoord;
  icoord.lat=(int)(coord.lat*100);
  icoord.lon=(int)(coord.lon*100);
  return fwrite(&icoord,sizeof(icoord),1,file)!=1;
} /* of 'writecoord' */

int seekcoord(Coordfile *coordfile, /* file pointer */
              int pos               /*  position in file */
             )                      /* returns return code of fseek */
{
  return fseek(coordfile->file,
               pos*sizeof(Icoord)+strlen(LPJGRID_HEADER)+sizeof(int)+sizeof(Header),
               SEEK_SET);
} /* of 'seekcoord' */

Real cellarea(Coord coord, /* cell coordnate */
              Coord resol  /* Resolution (deg) */
             )             /* returns area of cell (m^2) */
{
  return (111e3*resol.lat)*(111e3*resol.lon)*cos(deg2rad(coord.lat));
} /* of 'cellarea' */
