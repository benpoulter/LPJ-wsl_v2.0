/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ c h e c k _ f l u x e s . c                        **/
/*******************************************************************/

#include "lpj.h"

void check_fluxes(Cell *cell,
                  Config config,
                  int year,
                  int cellid,
                  Real delta_totc)
{
  Real balanceC,c_in,c_out;
  Real balanceW,totw;
  Stand *stand;
  int s,l;
  /* carbon balance check */    
  c_in    = cell->nep + cell->output.flux_estab;
  c_out   = cell->output.firec + cell->output.flux_harvest + cell->woodproducts_temp + cell->output.flux_luc; 
  balanceC= (c_in - c_out) - delta_totc; //the change in fluxes (c_in,c_out) must equal change in stocks
                
  if(year>config.firstyear+1 && (balanceC>8.5 || balanceC<-8.5)){
    fail("y: %d c: %d (%0.2f/%0.2f) \n\
		    BALANCE_C-error %.10f \n\
		    nep              : %.2f \n\
	            firec            : %.2f \n\
		    flux_estab       : %.2f \n\
	            flux_harvest     : %.2f \n\
		    flux_luc         : %.2f \n\
		    flux_woodProducts: %.2f \n\
		    delta_totc       : %.2f \n",
       year,cellid,cell->coord.lon,cell->coord.lat,
       balanceC,cell->nep,
       cell->output.firec,
       cell->output.flux_estab,
       cell->output.flux_harvest,
       cell->output.flux_luc,
       cell->woodproducts_temp,
       delta_totc);
  }
  /* water balance check */  
  totw=0.0;
  foreachstand(stand,s,cell->standlist){
    totw+=stand->soil.snowpack*stand->frac;
    foreachsoillayer(l)
      totw+=stand->soil.w[l]*stand->soil.soildepth[l]*stand->soil.par->whc[l]*stand->frac;
  }

  totw+=(cell->discharge.dmass_new+cell->discharge.dmass_lake)/cell->coord.area;

  balanceW=totw-cell->totw-cell->aprec+cell->awater_flux+cell->output.aevap_lake;
  if(year>config.firstyear+1 && (balanceW>1 || balanceW<-1)){
    fail("y: %d c: %d (%0.2f/%0.2f) \n\
		    BALANCE_W-error %.2f \n\
		    cell->totw : %.2f \n\
		    totw       : %.2f \n\
		    awater_flux: %.2f \n\
		    aprec      : %.2f \n\
		    dmass      : %.2f\n",
         year,cellid,cell->coord.lon,cell->coord.lat,
	 balanceW,
	 cell->totw,
	 totw,
	 cell->awater_flux+cell->output.aevap_lake,
	 cell->aprec,
         (cell->discharge.dmass_new+cell->discharge.dmass_lake)/cell->coord.area);
  }
  cell->totw=totw;
}
