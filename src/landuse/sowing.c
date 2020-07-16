/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ s o w i n g . c                                 **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"
#include "stand.h"

Real sowing(Standlist standlist,
      Cropdates *cropdates,
      const Pftpar *pftpar,
      Climbuf *climbuf,
      Real *landfrac,
      Real lat,
      int day,
      int npft,
      int ncrops,
      int ntypes,
      Output *output)
{  
  Bool alloc_today=FALSE;
  int cft, earliest_sdate,s;
  Real flux_estab=0;
  Pftcroppar *croppar;
  Stand *setasidestand;

  for(cft=0;cft<ncrops;cft++){
    if(existlandusetype(standlist,&s,SETASIDE)){
      setasidestand=getstand(standlist,s);
      croppar=pftpar[npft+cft].data;
      if(lat>=0) earliest_sdate=croppar->initdate.sdatenh;
      else earliest_sdate=croppar->initdate.sdatesh;
    
      if((day==cropdates->fall_sdate20[cft] && day>=earliest_sdate) ||
         (day+365==cropdates->fall_sdate20[cft] && day+365>=earliest_sdate)){
        if(cropdates->fallow[cft]<=0 && check_lu(standlist,landfrac+cft,npft+cft,FALSE)){
          if(!alloc_today) allocation_today(setasidestand, &alloc_today,ntypes);
          flux_estab+=cultivate(standlist,pftpar,cropdates->vern_date20[cft],landfrac+cft,npft+cft,FALSE,lat,day,TRUE,s);
          output->sdate[cft]=day;
        }

        if(cropdates->fallow[cft+ncrops]<=0 && check_lu(standlist,landfrac+cft+ncrops+NGRASS,npft+cft,TRUE)){
          if(!alloc_today) allocation_today(setasidestand, &alloc_today,ntypes);
          flux_estab+=cultivate(standlist,pftpar,cropdates->vern_date20[cft],landfrac+cft+ncrops+NGRASS,npft+cft,TRUE,lat,day,TRUE,s);
          output->sdate[cft+ncrops]=day;
        }

      }else{ 
        /* if((day==cropdates->spring_sdate20[cft] && day>=earliest_sdate) || */
        /*    (day+365==cropdates->spring_sdate20[cft] && day+365>=earliest_sdate)){ */
        if(day==cropdates->spring_sdate20[cft]){
          if(cropdates->fallow[cft]<=0 && croppar->calcmethod_sdate!=TEMP_PREC_CALC_SDATE &&  
             check_lu(standlist,landfrac+cft,npft+cft,FALSE)){
              
            if(!alloc_today) allocation_today(setasidestand, &alloc_today,ntypes);
            flux_estab+=cultivate(standlist,pftpar,cropdates->vern_date20[cft],landfrac+cft,npft+cft,FALSE,lat,day,FALSE,s);
            output->sdate[cft]=day;
          }

          if(cropdates->fallow[cft+ncrops]<=0 && check_lu(standlist,landfrac+cft+ncrops+NGRASS,npft+cft,TRUE)){
            if(!alloc_today) allocation_today(setasidestand, &alloc_today,ntypes);
            flux_estab+=cultivate(standlist,pftpar,cropdates->vern_date20[cft],landfrac+cft+ncrops+NGRASS,npft+cft,TRUE,lat,day,FALSE,s);
            output->sdate[cft+ncrops]=day;
          }
        }
    
        if(croppar->calcmethod_sdate==PREC_CALC_SDATE && day>=earliest_sdate &&
           cropdates->fallow[cft+ncrops]<=0 && check_lu(standlist,landfrac+cft+ncrops+NGRASS,npft+cft,TRUE)){

          if(!alloc_today) allocation_today(setasidestand, &alloc_today,ntypes);
          flux_estab+=cultivate(standlist,pftpar,cropdates->vern_date20[cft],landfrac+cft+ncrops+NGRASS,npft+cft,TRUE,lat,day,FALSE,s);
          output->sdate[cft+ncrops]=day;
        }
      }
    }else{
      break;
    }
  }
  
  return flux_estab;
}


/*
- called in update_daily()
- loop over all cfts
  -> check if set-aside stand exist; if not, no sowing possible
  -> set earliest sowing date (earliest_sdate) to initialised sowing date 
     (initdate.sdaten/sh) dependent on n/shemisphere (see also pft.par)
  -> checks if fall_sdate20 == day or day+365 and >= earliest_sdate
     -> if TRUE: differentiation between without/with irrigation
  -> checks if no fallow period and checks if no stand of such cft is still
     there (call of function check_lu())
     -> if TRUE: call of function cultivate()
  -> if FALSE: checks if spring_sdate20 == day
     -> if TRUE: differentiation between without/with irrigation
  -> checks if no fallow period and checks if no stand of such cft is still 
     there (call of function check_lu())
     -> if TRUE: call of function cultivate()

  -> for group PREC_CALC_SDATE with irrigation: checks if day >= earliest_day, 
     if no fallow period and no stand of such cft still exists (call of 
     function check_lu())
     -> if TRUE: call of function cultivate()
*/
