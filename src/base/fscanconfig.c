/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f s c a n c o n f i g . c                          **/
/*******************************************************************/

#include "lpj.h"

#define dflt_conf_filename "lpj.conf" /* Default LPJ configuration file */

#define fscanint(file,var,fcn,name) if(fscanf(file,"%d",var)!=1) readinterr(fcn,name)
#define fscanreal2(file,var,fcn,name) if(fscanreal(file,var))readrealerr(fcn,name) 
Bool iffire;

Bool fscanconfig(Config *config,       /* LPJ configuration           */
                 int *argc,            /* number of command line args */
                 char ***argv          /* list of commanfd line args  */
                )                      /* returns error code          */
{
  char *cmd,*filename;
  char **options;
  String name;
  FILE *argfile;
  int restart,flag,i,len,dcount,endgrid;
  options=newvec(char *,*argc-1);
  len=strlen(cpp_cmd);
  dcount=0;
  for(i=1;i<*argc;i++)
  {
    if((*argv)[i][0]=='-')
    {
      if((*argv)[i][1]=='D')
      {
        options[dcount++]=(*argv)[i];
        len+=strlen((*argv)[i])+1;
      } 
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n",(*argv)[i]);
        return TRUE;
      }
    }
    else
      break;
  }
  filename=(i==*argc)  ? dflt_conf_filename : (*argv)[i++];
  *argv+=i;
  *argc-=i;
  #ifdef USE_CPP
  cmd=malloc(strlen(filename)+len+1);
  strcpy(cmd,cpp_cmd);
  for(i=0;i<dcount;i++)
    strcat(strcat(cmd,options[i])," ");
  strcat(cmd,filename);
  argfile=pt_popen(cmd,"r");
  free(cmd);
  free(options);
  #else
  argfile=fopen(filename,"r");
  #endif
  if(argfile==NULL)
  {
    printfopenerr("fscanconfig",filename);
    return TRUE;
  }
  printf("Reading configuration from '%s'.\n",filename);
  fscanname(argfile,name,filename,"pftpar filename");
  config->pftpar_filename=strdup(name);
  fscanname(argfile,name,filename,"soilpar filename");
  config->soilpar_filename=strdup(name);
  fscanname(argfile,name,filename,"countrypar filename");
  config->countrypar_filename=strdup(name);
  fscanname(argfile,name,filename,"regionpar filename");
  config->regionpar_filename=strdup(name);
  fscancoord(argfile,&config->resolution);
  fscanname(argfile,name,filename,"coord filename");
  config->coord_filename=strdup(name);
  fscanname(argfile,name,filename,"soil filename");
  config->soil_filename=strdup(name);
  fscanname(argfile,name,filename,"countrycode filename");
  config->countrycode_filename=strdup(name);
  fscanname(argfile,name,filename,"lakes filename");
  config->lakes_filename=strdup(name);
  fscanname(argfile,name,filename,"luc filename");
  config->landuse_filename=strdup(name);
  fscanname(argfile,name,filename,"drainage filename");
  config->drainage_filename=strdup(name);
  fscanname(argfile,name,filename,"neighbour_irrig filename");
  config->neighb_irrig_filename=strdup(name);
  fscanname(argfile,name,filename,"temp filename");
  config->temp_filename=strdup(name);
  fscanname(argfile,name,filename,"prec filename");
  config->prec_filename=strdup(name);
  fscanname(argfile,name,filename,"cloud filename");
  config->cloud_filename=strdup(name);
  fscanname(argfile,name,filename,"sw filename");
  config->sw_filename=strdup(name);
  fscanname(argfile,name,filename,"lw filename");
  config->lw_filename=strdup(name);
  fscanname(argfile,name,filename,"ozone filename");
  config->ozone_filename=strdup(name);        
  fscanname(argfile,name,filename,"topm_filename"); 
  config->topm_filename=strdup(name);
  fscanname(argfile,name,filename,"tsl_filename"); 
  config->tsl_filename=strdup(name);
  fscanname(argfile,name,filename,"elev_filename");
  config->elev_filename=strdup(name);
  fscanname(argfile,name,filename,"standage filename");
  config->standage_filename=strdup(name);        
  fscanname(argfile,name,filename,"co2 filename");
  config->co2_filename=strdup(name);
  fscanname(argfile,name,filename,"wetfrac filename");
  config->wetfrac_filename=strdup(name);
  fscanint(argfile,&flag,filename,"prec");
  if(flag==RANDOM_PREC)
  {
    fscanname(argfile,name,filename,"wet filename");
    config->wet_filename=strdup(name);
    fscanint(argfile,&config->seed,filename,"random seed");
  }
  else
    config->wet_filename=NULL;
  fscanint(argfile,&config->n_out,filename,"n output");
  config->out_filename=newvec(char *,config->n_out);
  check(config->out_filename);
  for (i=0;i<config->n_out;i++)
  {
    fscanname(argfile,name,filename,"outfilename");
    config->out_filename[i]=strdup(name);
  }
  fscanint(argfile,&iffire,filename,"fire");
  iffire=(iffire==FIRE);
  fscanint(argfile,&config->startgrid,filename,"startgrid");
  fscanint(argfile,&endgrid,filename,"endgrid");
  config->ngridcell=endgrid-config->startgrid+1;
  fscanint(argfile,&config->nspinup,filename,"nspinup");
  fscanint(argfile,&config->firstyear,filename,"firstyr_climate");
  fscanint(argfile,&config->lastyear,filename,"lastyr_climate");
  fscanint(argfile,&config->firstco2year,filename,"firstyear_co2");
  fscanint(argfile,&config->firstryear,filename,"firstyr_reanalysis");
  fscanint(argfile,&config->firstdyear,filename,"firstyr_dailyout");

  if(config->firstyear-config->nspinup>config->lastyear)
  {
    fprintf(stderr,"Error: first simulation year=%d greater than last simulation year=%d.\n",
            config->firstyear-config->nspinup,config->lastyear);
    return TRUE;
  }
  fscanint(argfile,&restart,filename,"restart");
  if(restart==RESTART)
  {
    fscanname(argfile,name,filename,"restart filename");
    config->restart_filename=strdup(name);
  }
  else
    config->restart_filename=NULL;
  fscanint(argfile,&restart,filename,"write_restart");
  if(restart==RESTART)
  {
    fscanname(argfile,name,filename,"write_restart filename");
    config->write_restart_filename=strdup(name);
  }
  else
    config->write_restart_filename=NULL;
  pt_pclose(argfile);
  config->totalgridcell=0;
  return FALSE;
} /* of 'fscanconfig' */
