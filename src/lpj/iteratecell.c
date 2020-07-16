/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ i t e r a t e c e l l . c                           **/
/*******************************************************************/
#include "lpj.h"

#define nspinyear 20 /* cycle length during spinup (yr) */

int iteratecell(FILE **output,
                Cell *grid,
                Climate *climate,
                const Pftpar *pftpar,
                int npft,
                int ntypes,
                Config config)
{
  Real co2;
  int cell,year;
  for(year=config.firstyear-config.nspinup;year<=config.lastyear;year++)
  {
    printf("Year: %d\n",year); 
    if(iswriterestart(config) && year==config.firstyear-1){
      printf("Write restart file '%s'.\n",config.write_restart_filename);
      fwriterestart(config.write_restart_filename,
                    grid,config.startgrid,config.ngridcell,
                    npft,year);
    }
    co2=getco2(climate,year);
    for(cell=0;cell<config.ngridcell;cell++){
      if(getclimate(climate,cell,
                    (year<config.firstyear) ? 
                    (year-config.firstyear+config.nspinup) % nspinyear+
                      config.firstyear 
                    : year))
      {
        fprintf(stderr,"Simulation stopped.\n");
        return year;
      }
      iterateyear(output,grid+cell,climate,co2,pftpar,npft,ntypes,config,year);
    } /* of 'for(cell=...)' */
  } /* of 'for(year=...)' */
  return year;
} /* of 'iteratecell' */
