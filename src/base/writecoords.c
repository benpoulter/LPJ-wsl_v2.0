/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ w r i t e c o o r d s . c                          **/
/*******************************************************************/

#include "lpj.h"

int writecoords(FILE *file,const Cell grid[],int ncell)
{
  int cell;
  for(cell=0;cell<ncell;cell++){
    if(!grid[cell].skip)
    {
      if(writecoord(file,grid[cell].coord))
        break;
    }
  }
  return cell;
} /* of 'writecoords' */
