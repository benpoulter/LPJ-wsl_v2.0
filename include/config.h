/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / c o n f i g . h                             **/
/*******************************************************************/

#ifndef CONFIG_H /* Already included? */
#define CONFIG_H

/* Definition of datatypes */

typedef struct
{
  char *pftpar_filename;
  char *soilpar_filename;
  char *countrypar_filename;
  char *regionpar_filename;
  char *drainage_filename;
  char *neighb_irrig_filename;
  char *coord_filename;
  char *soil_filename;
  char *countrycode_filename;
  char *lakes_filename;
  char *landuse_filename;
  char *temp_filename;
  char *prec_filename;
  char *cloud_filename;
  char *sw_filename; 
  char *lw_filename; 
  char *ozone_filename; 
  char *elev_filename; 
  char *topm_filename;  
  char *tsl_filename;
  char *wet_filename;
  char *standage_filename; 
  char *co2_filename;
  char *wetfrac_filename;  
  char *restart_filename;
  char *write_restart_filename;
  char **out_filename;
  int ngridcell,startgrid,nspinup,lastyear,totalgridcell,firstyear,firstco2year,firstryear,firstdyear;
  int n_out;
  int seed;
  Coord resolution;
} Config;

/* Declaration of functions */

extern Bool fscanconfig(Config *,int *,char***);

#endif
