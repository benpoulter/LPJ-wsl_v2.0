/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ f s c a n p f t _ c r o p . c                      **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

#define fscanreal2(file,var,fcn,pft,name) \
  if(fscanreal(file,var)) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    return TRUE; \
  }

#define fscanint(file,var,fcn,pft,name) \
  if(fscanf(file,"%d",var)!=1) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    return TRUE; \
  }

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

#define fscancropdate2(file,var,fcn)\
  if(fscancropdate(file,var))\
  {\
    fprintf(stderr,"Error reading cropdate in '%s'.\n",fcn); \
    return TRUE; \
  }

static Bool fscancropdate(FILE *file,Initdate *initdate)
{
  int datenh,datesh;
  if(fscanf(file,"%d %d",&datenh,&datesh)!=2)
    return TRUE;
  initdate->sdatenh=datenh;
  initdate->sdatesh=datesh;
  return FALSE;
} /* of 'fscancropdate' */

#define fscancropphys2(file,var,fcn,name)\
  if(fscancropphys(file,var))\
  {\
    fprintf(stderr,"Error reading cropphys '%s' in '%s'.\n",name,fcn); \
    return TRUE; \
  }

static Bool fscancropphys(FILE *file,Cropphys *phys)
{
  double root,so,pool;
  if(fscanf(file,"%lg %lg %lg",&root,&so,&pool)!=3)
    return TRUE;
  if(root<=0 || so<=0 || pool<=0)
    return TRUE;
  phys->root=root;
  phys->so=so;
  phys->pool=pool;
  return FALSE;
} /* of 'fscancropphys' */
  
Bool fscanpft_crop(FILE *file,          /* file pointer */
                   Pftpar *pft,         /* Pointer to Pftpar array */
                   const char *filename /* filename */
                   )                    /* returns FALSE for success */
{
  Pftcroppar *crop;
  pft->newpft=new_crop;
  pft->npp=npp_crop;
  pft->leaf_phenology=phenology_crop;
  pft->fwrite=fwrite_crop;
  pft->fprint=fprint_crop;
  pft->fread=fread_crop;
  pft->litter_update=litter_update_crop;
  pft->isneg=isneg_crop;
  pft->lai=lai_crop;
  pft->fpar=fpar_crop;
  pft->free=free_crop;
  pft->vegc_sum=vegc_sum_crop;
  crop=new(Pftcroppar);
  pft->data=crop;
  fscanint(file,&crop->calcmethod_sdate,filename,pft->name,"calcmethod_sdate");
  fscancropdate2(file,&crop->initdate,filename);
  fscanint(file,&crop->hlimit,filename,pft->name,"max length of crop cycle");
  fscanint(file,&crop->fallow_days,filename,pft->name,"fallow period after harvest");
  fscanreal2(file,&crop->temp_fall,filename,pft->name,"temperature threshold for fall");
  fscanreal2(file,&crop->temp_spring,filename,pft->name,"temperature threshold for spring");
  fscanreal2(file,&crop->temp_vern,filename,pft->name,"temperature threshold for vernalization");
  fscanreal2(file,&crop->trg,filename,pft->name,"vernalization temperature");
  fscanreal2(file,&crop->pvd,filename,pft->name,"number of vernalising days");
  fscanreal2(file,&crop->psens,filename,pft->name,"photoperiod sensitivity");
  fscanreal2(file,&crop->pb,filename,pft->name,"photoperiod basal");
  fscanreal2(file,&crop->ps,filename,pft->name,"photoperiod saturated");
  fscanlimit2(file,&crop->phu,filename,pft->name,"potential heat units");
  fscanreal2(file,&crop->phu_par,filename,pft->name,"parameter for determining the variability of phu");
  fscanlimit2(file,&crop->basetemp,filename,pft->name,"base temperature");
  fscanreal2(file,&crop->fphuc,filename,pft->name,"growing season to the first point");
  fscanreal2(file,&crop->flaimaxc,filename,pft->name,"plant maximal LAI to the first point");
  fscanreal2(file,&crop->fphuk,filename,pft->name,"growing season to the second point");
  fscanreal2(file,&crop->flaimaxk,filename,pft->name,"plant maximal LAI to the second point");
  fscanreal2(file,&crop->fphusen,filename,pft->name,"growing period");
  fscanreal2(file,&crop->flaimaxharvest,filename,pft->name,"maximal LAI at harvest");
  fscanreal2(file,&crop->laimax,filename,pft->name,"maximal LAI");
  fscanreal2(file,&crop->laimin,filename,pft->name,"minimal LAI");
  fscanreal2(file,&crop->hiopt,filename,pft->name,"optimum harvest index");
  fscanreal2(file,&crop->himin,filename,pft->name,"minimum harvest index");
  fscanreal2(file,&crop->shapesenescencenorm,filename,pft->name,"shapesenescencenorm");
  fscancropphys2(file,&crop->cn_ratio,filename,"cn_ratio");
  fscanreal2(file,&crop->pip,filename,pft->name,"pip");
  fscanreal2(file,&crop->pmp,filename,pft->name,"pmp");
  fscanreal2(file,&crop->ms,filename,pft->name,"ms");
  return FALSE;
} /* of 'fscanpft_crop' */

/*
- called in fscanpftpar()
- scans crop specific parameters from pft.par
  -> struct Pftcroppar in crop.h
*/
