/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f p r i n t c e l l . c                            **/
/*******************************************************************/

#include "lpj.h"

void fprintcell(FILE *file,const Cell grid[],int ncell)
{
  int cell;
  for(cell=0;cell<ncell;cell++)
  {
    fprintf(file,"Coord :");
    fprintcoord(file,grid[cell].coord);
    fprintf(file,"\n");
    if(grid[cell].skip)
      fprintf(file,"Invalid soil\n");
    else
      fprintstandlist(file,grid[cell].standlist);
  }
} /* of 'fprintcell' */ 
