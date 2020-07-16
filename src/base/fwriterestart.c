/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f w r i t e r e s t a r t . c                      **/
/*******************************************************************/

#include "lpj.h"

Bool fwriterestart(const char *name,     /* filename                 */
                   Cell *grid,           /* cell array               */
                   int firstcell,        /* index of first cell      */
                   int ncell,            /* number of cells          */
                   int npft,             /* number of PFT's          */
                   int ncft,             /* number of cfts           */
                   int year,             /* year                     */
                   const Pftpar *pftpar, /* pft parameter            */
                   Bool intercrop        /* intercrop on setaside stand*/
                  )                      /* returns FALSE on success */ 
{
  FILE *file;
  int *index;
  int pos,cell,s,sg,p;
  Stand *stand,*standg;
  Pft *pft;
  Header header;
  Real rharvest;
  
  file=fopen(name,"wb");
  if(file==NULL)
  {
    printfopenerr("fwriterestart",name);   
    return TRUE;
  }
  for(cell=0;cell<ncell;cell++){
    if(!grid[cell].skip){
      foreachstand(stand,s,grid[cell].standlist){
        if(stand->landusetype==AGRICULTURE){ 
          /* setaside all existing croplands for restart file */
          foreachpft(pft,p,stand->pftlist){
            litter_update(&stand->soil.litter,pft,pft->nind,&rharvest,FALSE,FALSE);
          }
          setaside(grid+cell,&s,pftpar,intercrop,npft,FALSE);
        }else if(stand->landusetype==GRASSLAND){ 
          /* mix all existing GRASSLAND stands for restart file */
          foreachpft(pft,p,stand->pftlist){
            litter_update(&stand->soil.litter,pft,pft->nind,&rharvest,FALSE,FALSE);
          }
          foreachstand(standg,sg,grid[cell].standlist){
            if(standg->landusetype==GRASSLAND && s!= sg){
              foreachpft(pft,p,standg->pftlist){
                litter_update(&standg->soil.litter,pft,pft->nind,&rharvest,FALSE,FALSE);
              }
              mixsoil(stand,standg);
              stand->frac+=standg->frac;
              delstand(grid[cell].standlist,sg);
              break;
            }
	  }//..end stands
        }//..end ag and grassland check
      }//..end stands
    }//..end skip
  }//..end gridcells

  index=newvec(int,ncell);
  check(index);
  header.order=0;
  header.firstyear=year;
  header.nyear=1;
  header.firstcell=firstcell;
  header.ncell=ncell;
  header.nbands=0;
  fwriteheader(file,header,RESTART_HEADER,RESTART_VERSION);
  pos=ftell(file);
  fseek(file,sizeof(int)*ncell,SEEK_CUR);
  if(fwritecell(file,index,grid,ncell,npft,ncft,TRUE)!=ncell)
  {
    fprintf(stderr,"Error writing restart file '%s': %s\n",name,
            strerror(errno));
    free(index);
    fclose(file);
    return TRUE;
  }
  fseek(file,pos,SEEK_SET);
  fwrite(index,sizeof(int),ncell,file);
  fclose(file);
  free(index);
  return FALSE;
} /* of 'fwriterestart' */
