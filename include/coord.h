/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / c o o r d . h                               **/
/*******************************************************************/

#ifndef COORD_H /* Already included? */
#define COORD_H

/* Definition of datatypes */

typedef struct
{
  Real lon,lat;  /* longitude, latitude in degrees */
  Real area;     /* cell area (m^2) */
} Coord;
typedef struct
{
  FILE *file;
  int n;
  Bool swap;
} Coordfile;

/* Declaration of functions */

extern Coordfile *opencoord(const char *);
extern int seekcoord(Coordfile *,int);
extern Bool readcoord(Coordfile *,Coord *,Coord);
extern void closecoord(Coordfile *);
extern Bool writecoord(FILE *,Coord);
extern Real cellarea(Coord,Coord);

/* Definition of macros */

#define fprintcoord(file,coord) fprintf(file,"%5.2f %5.2f",coord.lon,coord.lat)
#define fscancoord(file,coord)  if(fscanf(file,"%lg %lg",coord.lon,coord.lat)==0){return 1;}

#define numcoord(coordfile) coordfile->n

#endif
