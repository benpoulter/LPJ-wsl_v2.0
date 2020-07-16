/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f r e e g r i d . c                                **/
/*******************************************************************/

#include "lpj.h"

void freegrid(Cell *grid,int n,Bool restart)
{
  int cell;
  for(cell=0;cell<n;cell++)
  {
    if(!grid[cell].skip || !restart){
      while(!isempty(grid[cell].standlist)){
      freestand(grid[cell].standlist,0);
      }

      freelist(grid[cell].standlist);    
      freeclimbuf(&grid[cell].climbuf);  
    }
    freecropdates(&grid[cell].cropdates);
    freeoutput(&grid[cell].output);
    free(grid[cell].gdd);
  }  
  free(grid);
} /* of 'freegrid' */
