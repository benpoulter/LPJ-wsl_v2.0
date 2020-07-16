/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ s t a n d l i s t . c                              **/
/*******************************************************************/

#include "lpj.h"

void delstand(List *list,int index)
{
  freestand(list,index);
}

int fwritestandlist(FILE *file,const Standlist standlist,Bool full)
{
  Stand *stand;
  int s;
  fwrite(&standlist->n,sizeof(int),1,file);
  foreachstand(stand,s,standlist){
    if(fwritestand(file,stand,full)){
      return s;
    }
  }
  return s;
} /* of 'fwritestandlist' */

void fprintstandlist(FILE *file,const Standlist standlist)
{
  Stand *stand;
  int s;
  fprintf(file,"Number of stands: %d\n",standlist->n);
  foreachstand(stand,s,standlist)
  {
    fprintf(file,"Stand: %d\n",s);
    fprintf(file,"Standfrac: %f\n",stand->frac);
    fprintf(file,"Standtype: %d\n",stand->landusetype);
    fprintf(file,"IRRIGATION: %d\n",stand->irrigation);    
    fprintstand(file,stand);
  }
} /* of 'fprintstandlist' */

Standlist freadstandlist(FILE *file,const Pftpar pftpar[],
                         const Soilpar *soilpar,Bool swap)
{
  int i,n;
  Standlist standlist;
  if(freadint1(&n,swap,file)!=1){
    return NULL;
  }
  standlist=newlist();
  for(i=0;i<n;i++)
    addlistitem(standlist,freadstand(file,pftpar,soilpar,swap));
  return standlist;
} /* of 'freadstandlist' */

int addstand(List *list)
{
  Stand *stand;
  stand=new(Stand);
  addlistitem(list,stand);
  stand->pftlist=newpftlist();
  initstand(stand);
  return getlistlen(list);
}

void initstand(Stand *stand)
{
  #if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
  int j;
  #endif
  
  #if (defined(GROSS_LUC) || defined(NETinGROSS)) && !defined(NAT_VEG)
    //  (1) if deforestation, the stand is converted to a SECFOREST (Age1) stand
    //  (2) if using AGECLASS and FIRE occurs, 
    //      ..then stand LUtype is set to SECFOREST 
    //      ..and LUtype is then updated to PRIMARY_TEMP or SECFOREST_TEMP 
    stand->landusetype=SECFOREST; 

    //if necessary, standids updated outside of this fn
    stand->ageclass_standid=1; //youngest ageclass
  #else
    //Natural (Primary) forest is only created during spin-up
    //DEVQ: this could be modified to allow Primary succession to occur during transient runs
    //      ..e.g.Fire resets stands to bedrock (no soil)
    stand->landusetype=PRIMARY;

    //standids also updated outside of this fn
    stand->ageclass_standid=12; //oldest ageclass
  #endif
  stand->irrigation=FALSE;
  stand->fire_sum=0;
  stand->count=0;
  stand->pot_irrig_amount=stand->irrig_amount=stand->irrig_sum=0.0;
  stand->TimeSinceDist=0; //initialize time since disturbance to zero
  stand->CropYrs=0;       //initialize crop years

  //len_frac_transitions are updated outside of this fn
  stand->len_frac_transition=1;
  
  #if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
    //initialize within-ageClass fractional transitions to zero
    //..reset all fractional transitions to zero
    for(j=0; j < MAX_WITHINSTAND_TRANS; j++){
      stand->frac_transition[j]=0.0;
    }
  #endif
}

int freestand(List *list,int index)
{
  Stand *stand;
  stand=getstand(list,index);
  freepftlist(stand->pftlist);
  free(stand);
  return dellistitem(list,index);
} /* of 'freestand' */
