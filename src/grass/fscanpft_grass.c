/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ f s c a n p f t _ g r a s s . c                   **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

#define fscanint(file,var,fcn,pft,name) \
  if(fscanf(file,"%d",var)!=1) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    return TRUE; \
  }
#define fscangrassphys2(file,var,fcn,name) \
  if(fscangrassphys(file,var))\
  { \
    fprintf(stderr,"Error reading limit '%s' in '%s'.\n",name,fcn); \
    return TRUE; \
  }

#define fscanreal2(file,var,fcn,pft,name) \
  if(fscanreal(file,var)) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    return TRUE; \
  }
//Changed to return TRUE from BREAK
#define fscanlimit2(file,var,fcn,pft,name) \
  if(fscanlimit(file,var)) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    return TRUE; \
  }
static Bool fscanlimit(FILE *file,Limit *limit)
{
  double low,high;
  if(fscanf(file,"%lg %lg",&low,&high)!=2)
    return TRUE;
  limit->low=low;
  limit->high=high;
  return FALSE;
} /* of 'fscanlimit' */

static Bool fscangrassphys(FILE *file,Grassphys *phys)
{
  double leaf,root;
  if(fscanf(file,"%lg %lg",&leaf,&root)!=2)
    return TRUE;
  if(leaf<=0 ||  root<=0)
    return TRUE;
  phys->leaf=leaf;
  phys->root=root;
  return FALSE;
} /* of 'fscangrassphys' */

Bool fscanpft_grass(FILE *file,          /* file pointer */
                    Pftpar *pft,         /* Pointer to Pftpar array */
                    const char *filename /* filename */
                   )                     /* returns FALSE for success  */
{
  Pftgrasspar *grass;
  pft->newpft=new_grass;
  pft->turnover=turnover_grass;
  pft->npp=npp_grass;
  pft->fpc=fpc_grass;
  pft->fpar=fpar_grass;
  pft->leaf_phenology=phenology_grass;
  pft->fwrite=fwrite_grass;
  pft->mix_veg=mix_veg_grass;
  pft->fire=fire_grass;
  pft->fprint=fprint_grass;
  pft->fread=fread_grass;
  pft->litter_update=litter_update_grass;
  pft->allocation=allocation_grass;
  pft->isneg=isneg_grass;
  pft->establishment=establishment_grass;
  pft->reproduction=reproduction_grass;
  pft->lai=lai_grass;
  pft->init=init_grass;
  pft->free=free_grass;
  pft->vegc_sum=vegc_sum_grass;
  pft->light=light_grass;
  grass=new(Pftgrasspar);
  pft->data=grass;
  fscangrassphys2(file,&grass->turnover,filename,"turnover");
  fscangrassphys2(file,&grass->cn_ratio,filename,"cn_ratio");
  fscanreal2(file,&grass->o3crit,filename,pft->name,"o3crit");
  fscanlimit2(file,&grass->o3aparam,filename,pft->name,"o3aparam");
  fscanreal2(file,&grass->max_wtd,filename,pft->name,"max_wtd");
  fscanreal2(file,&grass->maxinunddays,filename,pft->name,"maxinunddays");
  fscanreal2(file,&grass->bulkdens,filename,pft->name,"bulkdens");
  fscanint(file,&grass->ispeat,filename,pft->name,"ispeat");
  fscanreal2(file,&grass->pip,filename,pft->name,"pip");
  fscanreal2(file,&grass->pmp,filename,pft->name,"pmp");
  fscanreal2(file,&grass->ms,filename,pft->name,"ms");

  grass->sapl.leaf=pft->lai_sapl/pft->sla;
  grass->sapl.root=(1.0/pft->lmro_ratio)*grass->sapl.leaf;
  return FALSE;
} /* of 'fscanpft_grass' */
