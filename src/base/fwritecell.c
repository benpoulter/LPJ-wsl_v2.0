/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f w r i t e c e l l . c                            **/
/*******************************************************************/

#include "lpj.h"

int fwritecell(FILE *file,int index[],const Cell grid[],int ncell,int npft,int ncft,
               Bool full)
{
  int cell;
  for(cell=0;cell<ncell;cell++)
  {
    if(index!=NULL)
      index[cell]=ftell(file);
    fwrite(&grid[cell].skip,sizeof(Bool),1,file);
    if(!grid[cell].skip){
      if(fwritestandlist(file,grid[cell].standlist,full)!=grid[cell].standlist->n)
        break;
      if(full){
        if(fwriteclimbuf(file,&grid[cell].climbuf))
          break;
        if(fwrite(grid[cell].gdd,sizeof(Real),npft+ncft,file)!=npft+ncft)
          break;
        if(fwrite(&grid[cell].discharge.dmass_new,sizeof(Real),1,file)!=1)
          break;
        if(fwrite(&grid[cell].discharge.dmass_lake,sizeof(Real),1,file)!=1)
          break;
        if(fwritecropdates(file,&grid[cell].cropdates,ncft))
          break;
        //store legacy effects from wood harvest and deforestation (remaining in product pools)
        if(fwrite(&grid[cell].product_100yr,sizeof(Real),1,file)!=1)
          break;
        if(fwrite(&grid[cell].product_10yr,sizeof(Real),1,file)!=1)
          break;
        if(fwrite(&grid[cell].product_1yr,sizeof(Real),1,file)!=1)
          break;
        if(fwrite(&grid[cell].product_25yr,sizeof(Real),1,file)!=1)
          break;
        if(fwrite(&grid[cell].product_2yr,sizeof(Real),1,file)!=1)
          break;
      }//..end full
    }//..end skip
  }//..end gridcells
  return cell;
} /* of 'fwritecell' */
