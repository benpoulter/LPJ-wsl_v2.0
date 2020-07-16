/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / c l i m a t e . h                           **/
/*******************************************************************/

#ifndef CLIMATE_H /* Already included? */
#define CLIMATE_H

#define COLDEST_DAY_NHEMISPHERE 14
#define COLDEST_DAY_SHEMISPHERE 195

#define CLIMBUFSIZE 20 /* size of climate buffer */
#define NDAYS 31
#define NDAYS_PREC 10

#define CO2_P 280.0      /* preindustrial value for CO2 (ppm) */

/* Definitions of datatypes */

typedef struct
{
  int n,offset,size;        
  int n_day,offset_day,size_day;  
  int n_ozone,offset_ozone,size_ozone;                                          //reading of ozone inputs
  int order,firstyear,firstco2year,firstryear,nyear,ncell;
  Bool swap;
  Real *co2;
  Real *inund;                                            //reading of inundation from satellite product
  FILE *file_temp,*file_prec,*file_cloud,*file_wet,*file_sw,*file_lw,*file_co2; //reading of LW and SW files
  FILE *file_ozone;                                                             //OZONE - reading of ozone file
  int ncid_temp,ncid_prec,ncid_cloud,ncid_wet,ncid_sw,ncid_lw,ncid_co2;         //reading of LW and SW and NCD
  int ncid_ozone; 
  int ncid_inund;                                                               //inundation
  Real *temp,*prec,*cloud,*wet,*swdown,*lwdown,*ozone;
  Real *temp_day,*prec_day,*sw_day, *lw_day;
} Climate;

typedef struct
{
  Real gdd5;
  Real temp[NDAYS];
  Real dval_prec[NDAYS+1];
  Real temp_min,temp_max,atemp_mean;
  Buffer *min,*max;
} Climbuf;

/* Definitions of macros */

#define getcellco2(climate,cell) climate->co2+(cell)*NMONTH 
#define getcelltemp(climate,cell) climate->temp+(cell)*NMONTH
#define getcellprec(climate,cell) climate->prec+(cell)*NMONTH
#define getcellinund(climate,cell) climate->inund+(cell)*NMONTH 
#define getcellsun(climate,cell) climate->cloud+(cell)*NMONTH
#define getcellwet(climate,cell) climate->wet+(cell)*NMONTH
#define getcellsw(climate,cell) climate->swdown+(cell)*NMONTH 
#define getcelllw(climate,cell) climate->lwdown+(cell)*NMONTH 
#define getcellozone(climate,cell) climate->ozone+(cell)*NMONTH*3 
#define getprec(cell,d) (cell).climbuf.dval_prec[(d)+1]
#define initgdd5(climbuf) climbuf.gdd5=0
#define updategdd5(climbuf,temp) if(temp>5) (*climbuf).gdd5++

//Daily
#define getcelltempDay(climate,cell) climate->temp_day+(cell)*NDAYYEAR 
#define getcellprecDay(climate,cell) climate->prec_day+(cell)*NDAYYEAR 
#define getcellswDay(climate,cell) climate->sw_day+(cell)*NDAYYEAR 
#define getcelllwDay(climate,cell) climate->lw_day+(cell)*NDAYYEAR 

/* Declaration of functions */

#ifndef CLM_NCDF
extern Climate *initclimate(Config *);
#endif

#ifdef CLM_NCDF
extern Climate *initclimate_ncdf(Config *);  
#endif

extern Climate *initclimate_daily_ncdf(Config *);  
extern Bool getclimate(Climate *,int);
extern Bool getclimate_ncdf(Climate *, int, Real[], Real[], int);  
extern Bool getclimate_daily_ncdf(Climate *, int, Real[], Real[], int);  
extern Real getcellID(int, int,  Real[], Real[], int[], int[]);  
extern Real getsinglecellID(int, Real, Real, int *, int *);  
extern Real getco2(const Climate *,int);
extern Bool getco2_ncdf(const Climate *,int, Real[], Real[], int);
extern Bool getinund_ncdf(const Climate *,int, Real[], Real[], int);
extern void freeclimate(Climate *);
extern void freeclimate_ncdf(Climate *);  
extern void freeclimate_daily_ncdf(Climate *);  
extern void new_climbuf(Climbuf *);
extern void init_climbuf(Climbuf *);
extern void daily_climbuf(Climbuf *,Real);
extern void monthly_climbuf(Climbuf *,Real);
extern void annual_climbuf(Climbuf *);
extern Bool fwriteclimbuf(FILE *,const Climbuf *);
extern Bool freadclimbuf(FILE *,Climbuf *,Bool);
extern void freeclimbuf(Climbuf *);
extern void prdaily(Climbuf *,int,Real,Real);

#endif
