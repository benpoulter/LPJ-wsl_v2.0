/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  s r c / p r i n t l p j . c                                  **/
/**                                                               **/
/**  prints restart file of LPJ                                   **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"
#include "tree.h"

#define PRINTLPJ_VERSION "2.0"
#define NTYPES 2

int main(int argc,char **argv)
{
  int npft,ncft,nsoil,startgrid,ngridcell,ncountries,nregions;
  Pftpar *pftpar;
  Soilpar *soilpar;
  Countrypar *countrypar;
  Regionpar *regionpar;
  Cell *grid=NULL;
  Config config;
  Fscanpftparfcn scanfcn[NTYPES]={fscanpft_grass,fscanpft_tree};
  printf("**** %s C Version %s (" __DATE__ ") ****\n",argv[0],PRINTLPJ_VERSION);
  if(fscanconfig(&config,&argc,&argv))
    fail("Error reading config\n");
  if(argc>0)
    startgrid=atoi(argv[0]);
  else
    startgrid=config.startgrid;
  if(argc>1)
    ngridcell=atoi(argv[1])-startgrid+1;
  else
    ngridcell=config.ngridcell;
  config.ngridcell=min(ngridcell,config.ngridcell-startgrid+config.startgrid); 
  if(startgrid>=config.startgrid)
    config.startgrid=startgrid;
  if((npft=fscanpftpar(&pftpar,config.pftpar_filename,scanfcn,NTYPES,&ncft))==0)
    fail("Error reading '%s'",config.pftpar_filename);
  if((nsoil=fscansoilpar(&soilpar,config.soilpar_filename))==0)
    fail("Error reading '%s',",config.soilpar_filename);
  if((ncountries=fscancountrypar(&countrypar,config.countrypar_filename))==0)
    fail("Error reading '%s',",config.countrypar_filename); 
  if((nregions=fscanregionpar(&regionpar,config.regionpar_filename))==0)
    fail("Error reading '%s',",config.regionpar_filename);
  config.restart_filename=config.write_restart_filename;

  printf("Year: %d\n",config.lastyear);
  printcell(grid,config.ngridcell);
  return EXIT_SUCCESS;
} /* of 'main' */
