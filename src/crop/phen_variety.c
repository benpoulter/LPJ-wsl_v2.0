/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ p h e n _ v a r i e t y . c                        **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

#define KEYDAY_NHEMISPHERE 365 /* last day of driest month (Dez) of northern hemisphere */
#define KEYDAY_SHEMISPHERE 181 /* last day of driest month (Jun) of southern hemisphere */

void phen_variety(Pftcrop *crop,
      const Pftcroppar *croppar,
      int vern_date20,
      Real lat,
      int sdate,
      Bool wtyp)
{  
  int keyday,keyday1;

  if(lat>=0){
    keyday=KEYDAY_NHEMISPHERE;
    keyday1=0;
  }else{
    keyday=keyday1=KEYDAY_SHEMISPHERE;
  }

  if(sdate>365) sdate-=365;
     
  if(vern_date20>0){
    if(wtyp){
      crop->pvd=max(0,min(60,vern_date20-sdate-croppar->pvd));
      crop->phu=max(croppar->phu.low,-0.1081*pow((sdate-keyday),2)
        +3.1633*(sdate-keyday)+croppar->phu.high);
    }else{
      crop->pvd=max(0,min(60,vern_date20-sdate));
      crop->phu=croppar->phu.low;
    }
  }else{
    crop->pvd=0;
    crop->phu=min(croppar->phu.high,max(croppar->phu.low,-700.0/90.0*(sdate-keyday1)+croppar->phu_par));
  }
   
  crop->basetemp=min(croppar->basetemp.high,max(croppar->basetemp.low,-15.0/110.0*(sdate-keyday1)+25.5));
}

/*
- called in cultivate()
- this function adjusts parameters (pvd,phu,basetemp) for certain cfts (pvd, phu 
  for temperate cereals, rapeseed, sunflower; basetemp only for maize) for a
  variety of the phenology between colder and warmer regions 
- the parameters pvd, phu.low/high, phu_par, basetemp.low/high are set in pft.par, 
  so that only for the above mentioned cfts a variety can be determined
- sets the keydays dependent on n/shemisphere
- brings the local variable sdate (sowing date) to the interval [1,365]
- 3 different determinations of phu (phenological heat units) and pvd 
  (vernalization days) dependent of variables vern_date20 and wtyp
  -> hints: the parameters phu.low/high are equal for all cfts except for 
            temperate cereals,rapeseed,sunflower 
            the parameter phu_par is set to 9999 for all cfts except for sunflower
- checks if vernalization days exist (vern_date20>0)
  -> only for temperate cereals, rapeseed possible
  -> if TRUE: checks if cft is a winter type (wtyp)

- determination of basetemp (base temperature) dependent on sdate
  -> hint: parameters bastemp.low/high are equal for all cfts except for 
           sunflower, so that the basetemp of these cfts is equal to the 
           parameters basetemp.low/high (see pft.par) 
*/
