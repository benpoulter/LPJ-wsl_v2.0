/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ u p d a t e _ m o n t h l y . c                     **/
/*******************************************************************/
#include "lpj.h"


void update_monthly(FILE **output,
        Cell *cell,
        Real mtemp,
        int month,
        int year,
        Config config,
        int npft 
        )
{
  int s,l;
  Real nep,awater_flux;
  Stand *stand;

  monthly_climbuf(&cell->climbuf,mtemp);
  foreachstand(stand,s,cell->standlist)
    getlag(&stand->soil,month);
  for(l=0;l<NSOILLAYER;l++)
    cell->output.mswc[l]*=ndaymonth1[month];

  cell->output.mpar*=ndaymonth1[month];
  //Climate variables
  cell->output.mtair*=ndaymonth1[month];
  cell->output.mswdown*=ndaymonth1[month];
  cell->output.mlwdown*=ndaymonth1[month];
  cell->output.mcld*=ndaymonth1[month];
  cell->output.mco2conc*=ndaymonth1[month];

  #ifdef TOPMODEL
    //monthly mean wetfrac per cell
    cell->output.wtd*=ndaymonth1[month];
    cell->output.wetfrac*=ndaymonth1[month];
  #endif

  #ifdef PERMAFROST
    cell->output.mtsoil_0*=ndaymonth1[month];
    cell->output.mtsoil_10*=ndaymonth1[month];
    cell->output.mtsoil_25*=ndaymonth1[month];
    cell->output.mtsoil_50*=ndaymonth1[month];
    cell->output.mtsoil_70*=ndaymonth1[month];
    cell->output.mtsoil_100*=ndaymonth1[month];
    cell->output.mtsoil_150*=ndaymonth1[month];
    cell->output.mtsoil_200*=ndaymonth1[month];
    cell->output.mtsoil_top*=ndaymonth1[month];
    cell->output.mthaw_depth*=ndaymonth1[month];
    cell->output.mFwater_10*=ndaymonth1[month];
    cell->output.mFwater_20*=ndaymonth1[month];
    cell->output.mFwater_30*=ndaymonth1[month];
    cell->output.mFwater_50*=ndaymonth1[month];
    cell->output.mFwater_70*=ndaymonth1[month];
    cell->output.mFwater_100*=ndaymonth1[month];
    cell->output.mFwater_150*=ndaymonth1[month];
    cell->output.mFwater_200*=ndaymonth1[month];
    cell->output.mFice_10*=ndaymonth1[month];
    cell->output.mFice_20*=ndaymonth1[month];
    cell->output.mFice_30*=ndaymonth1[month];
    cell->output.mFice_50*=ndaymonth1[month];
    cell->output.mFice_70*=ndaymonth1[month];
    cell->output.mFice_100*=ndaymonth1[month];
    cell->output.mFice_150*=ndaymonth1[month];
    cell->output.mFice_200*=ndaymonth1[month];
  #endif

  #ifdef RIVER_ROUTING
    //daily mean discharge per month in 1.000.000 m per cell
    cell->output.mdischarge=cell->output.mdischarge/ndaymonth[month]/1000/1000/1000; 
  #endif

  if((config.firstyear!=config.lastyear) && (year>=(config.firstyear-config.nspinup))){
    #ifdef SINGLECELL_CSVOUTPUT
      //DEVQ: add variables for climate MAP,MAT and MAP_STD,MAT_STD
      //: ..add file for water specific outputs
      if(year==(config.firstyear-config.nspinup) && month==0){
        fwriteoutput_monthly(output,cell,npft,year,FALSE);
      }else{fwriteoutput_monthly(output,cell,npft,year,TRUE);}
    #else
      fwriteoutput_monthly(output,cell,npft,year,FALSE);
    #endif
    /*printf("NPP: %f Rh: %f flux_estab: %f flux_h: %f\n",
              cell->output.mnpp,cell->output.mrh,
              cell->output.flux_estab,
              cell->output.flux_harvest);
    */
  }

  //---------------------------------------
  //      C A R B O N   B A L A N C E  
  //---------------------------------------
  nep=cell->output.mnpp-cell->output.mrh;
  cell->nep+=nep;

  //---------------------------------------
  //       W A T E R   B A L A N C E  
  //---------------------------------------
  awater_flux=cell->output.mrunoff+cell->output.mtransp+cell->output.mevap
              +cell->output.minterc-cell->output.mirrig
              +(cell->discharge.mfout-cell->discharge.mfin)/cell->coord.area;
  cell->awater_flux+=awater_flux;

} /* of 'monthly_update' */  
