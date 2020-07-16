/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ n e w g r i d . c                                   **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

Cell *newgrid(Config *config,                 /* Pointer to LPJ configuration */
              const Pftpar pftpar[],
              int npft,                       /* number of PFT's              */
              int ncft,
              const Soilpar soilpar[],
              int nsoil,                      /* number of soil types         */
              const Countrypar countrypar[],
              int ncountries,                 /* number of countries          */
              const Regionpar regionpar[],
              int nregions,                   /* number of regions            */
              Landuse *landuse)               /* returns allocated cell grid  */
{
  Cell *grid;
  Stand *stand;
  Header header,header_cow;
  int i,j,offset,n;
  Bool swap,swap_cow;
  char soilcode;
  short cowcode,regcode;
  Real soildepth[2];      //SOILDEPTH - create a soildepth array
  int pft, retval;        //PFTFIX - create new pft variable
  FILE *file_restart=NULL;
  FILE *file_countrycode=NULL;
  Real latIn[config->ngridcell], lonIn[config->ngridcell];
  int lonMatch, latMatch; //matching longitude and latitudes
  #ifdef STANDAGEINIT
    short standage;  
    FILE *file_standage;  
  #endif
  #ifdef RIVER_ROUTING
    char lakefrac;
    FILE *file_lakes;
  #endif
  #ifdef ELEV
    short elev; //for ozone
  #endif
  Coordfile *file_coord;

  /* Open coordinate file */
  file_coord=opencoord(config->coord_filename);
  if(file_coord==NULL){
    printf("Problem with coordinate grid file\n");    //for clarity of error messages
    printfopenerr("newgrid",config->coord_filename);
    return NULL;
  }

  if(config->totalgridcell!=numcoord(file_coord))
    fprintf(stderr,"Warning: number of gridcells in '%s' distinct from %d.\n",
            config->coord_filename,numcoord(file_coord));

  //NCDF - open soiltype file as clm
  #ifndef CLM_NCDF
    FILE  *file_soil;
    file_soil=fopen(config->soil_filename,"rb");
    if(file_soil==NULL){
      printfopenerr("newgrid",config->soil_filename);
      return NULL;
    }
  #else
    //NCDF - open soiltype file as ncdf
    int file_soil;
  
    // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file
    if ((retval = nc_open(config->soil_filename, NC_NOWRITE, &file_soil)))
       ERR(retval);
  #endif

  //OZONE, use elevation correction
  #ifdef ELEV
    int file_elev;
  
    // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file
    if ((retval = nc_open(config->elev_filename, NC_NOWRITE, &file_elev)))
       ERR(retval);
  #endif

  #ifdef TOPMODEL
    int file_topm;
  
    // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file
    if ((retval = nc_open(config->topm_filename, NC_NOWRITE, &file_topm)))
        ERR(retval);
  #endif
  
  #ifdef PERMAFROST
    int file_tsl;
  
    // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file
    if ((retval = nc_open(config->tsl_filename, NC_NOWRITE, &file_tsl)))
        ERR(retval);
  #endif
  
  #if defined(LUHWATER) //reading the ice/water fractions from LUH Hurtt
    int file_icewater;
    // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file
    if ((retval = nc_open(config->lakes_filename, NC_NOWRITE, &file_icewater)))
       ERR(retval);
  #endif

  /* Open countrycode file */
  file_countrycode=fopen(config->countrycode_filename,"rb");
  if(file_countrycode==NULL){
    printfopenerr("newgrid",config->countrycode_filename);
    return NULL;
  }  
  if(freadheader(file_countrycode,&header_cow,&swap_cow,LPJ_COUNTRY_HEADER,LPJ_COUNTRY_VERSION)){
    fprintf(stderr,"Invalid header in '%s'.\n",config->countrycode_filename);
    fclose(file_countrycode);                                                         
    return NULL;
  }

  #ifdef RIVER_ROUTING  
    /* Open file for lake fraction */
    file_lakes=fopen(config->lakes_filename,"rb");
    if(file_lakes==NULL){
      printfopenerr("newgrid",config->lakes_filename);
      return NULL;
    }
  #endif
  
  #ifdef STANDAGEINIT
    /* Open file for forest stand age */
    file_standage=fopen(config->standage_filename,"rb");
    if(file_standage==NULL){
      printfopenerr("newgrid",config->standage_filename);
      return NULL;
    }
  #endif

  /* If FROM_RESTART open restart file */
  if(config->restart_filename!=NULL){
    file_restart=fopen(config->restart_filename,"rb");
    if(file_restart==NULL){
      printfopenerr("newgrid",config->restart_filename);
      return NULL;
    }

    /* Read restart header */
    if(freadheader(file_restart,&header,&swap,RESTART_HEADER,RESTART_VERSION)){
      fprintf(stderr,"Invalid header in '%s'.\n",config->restart_filename);
      fclose(file_restart);
      return NULL;
    }
    if( (config->firstyear!=config->lastyear) && (header.firstyear!=config->firstyear) )
      fprintf(stderr,"Warning: year of restartfile=%d not equal start year=%d.\n",header.firstyear,config->firstyear);

    offset=config->startgrid-header.firstcell;
    if(offset<0){
      fprintf(stderr,"Warning: first grid cell not in restart file, set to %d.\n",header.firstcell);
      config->startgrid=header.firstcell;
      offset=0;
    }

    if(offset>0){
      if(offset>=header.ncell){
        fprintf(stderr,"Error: First grid cell not in restart file.\n");
        fclose(file_restart);
        return NULL;
      }
      fseek(file_restart,offset*sizeof(int),SEEK_CUR);
    }

    freadint1(&offset,swap,file_restart);
    fseek(file_restart,offset,SEEK_SET);

    if(header.ncell<config->ngridcell){
      fprintf(stderr,"Warning: restart file too short, grid truncated to %d.\n",header.ncell);
      config->ngridcell=header.ncell;
    }
  } //..end if config file exists

  /* allocate grid */
  if((grid=newvec(Cell,config->ngridcell))==NULL){
    printallocerr("newgrid","grid");
    return NULL;
  }

  //Create vector of ALL latitudes and longitudes for NCDF handling
  printf("Reading coordinates\n");
  seekcoord(file_coord,config->startgrid);
  for(j=0; j<config->ngridcell; j++){
    readcoord(file_coord,&grid[j].coord,config->resolution);
    latIn[j]=grid[j].coord.lat;
    lonIn[j]=grid[j].coord.lon; 
    //printf("ncells=%d\tcellID=%d\tlat=%f\tlon=%f\n",config->ngridcell,j,grid[j].coord.lat,grid[j].coord.lon);
  }
  closecoord(file_coord);

  //Reset seek coord
  file_coord=opencoord(config->coord_filename);
  seekcoord(file_coord,config->startgrid);

  //NCDF - read soil as binary
  #ifndef CLM_NCDF
    fseek(file_soil,config->startgrid*sizeof(soilcode),SEEK_SET);
  #endif
  
  //read country codes
  fseek(file_countrycode,headersize(LPJ_COUNTRY_HEADER)+config->startgrid*sizeof(cowcode)*sizeof(regcode),SEEK_SET);
  
  #ifdef RIVER_ROUTING
    fseek(file_lakes,config->startgrid*sizeof(lakefrac),SEEK_SET);
  #endif
  
  #ifdef STANDAGEINIT
    //move to cell location to read standage - (no header in file)
    fseek(file_standage,config->startgrid*sizeof(standage),SEEK_SET);
  #endif

  //Begin looping through grid cells
  for(i=0;i<config->ngridcell;i++){
    if(readcoord(file_coord,&grid[i].coord,config->resolution)){
      fprintf(stderr,"Unexpected end of file in '%s', number of gridcells truncated to %d.\n",config->coord_filename,i);
      config->ngridcell=i;
      break;
    }

    #ifndef CLM_NCDF
      if(fread(&soilcode,sizeof(soilcode),1,file_soil)!=1){
        fprintf(stderr,"Unexpected end of file in '%s', number of gridcells truncated to %d.\n",config->soil_filename,i);
        config->ngridcell=i;
        break;
      }
    #endif

    //SOILDEPTH - set default soil depths
    //.............................................................................................
    //NOTE IMPT (ZZheng): there is a bug that if soildepth changed, soil moisture will be changed
    //                    so actually soildepth is still 1000 even running PERMAFROST flag
    //.............................................................................................
    soildepth[0]=500;
    soildepth[1]=1000;

    //PFTFIX - initialize PFT id
    pft=0;

    //initialize lake, standage fractions
    //values are overwriten if flags are set for lake and standage
    grid[i].lakefrac=0.0;
    grid[i].standage=0.0;
  
    //OZONE - elevation
    #ifdef ELEV
      int varid_elev, elev_db[1];
     
      //Match lat lon from ingrid to netcdf file ncdf
      getsinglecellID(file_soil, grid[i].coord.lon, grid[i].coord.lat, &lonMatch, &latMatch);
     
      size_t start_elev[2], count_elev[2];
      start_elev[0]=latMatch;
      start_elev[1]=lonMatch;
      count_elev[0]=1;
      count_elev[1]=1;
    
      // Read the variable name from last dimension
      if ((retval = nc_inq_varid(file_elev, "elev", &varid_elev))){
        fprintf(stderr,"Error reading elevation variable from file,  elevation varname should be 'elev'; check variable names.\n");
        ERR(retval);
      }
    
      // Read the data from netcdf file
      if ((retval = nc_get_vara_int(file_elev, varid_elev, start_elev, count_elev, elev_db)))
        ERR(retval);
      elev=elev_db[0];
    #endif

    //NCDF (read soil data)
    #ifdef CLM_NCDF
      #ifdef FAO
        int varid_soilcode,soilcode_db[1];
        size_t start_soil[3], count_soil[3];
        //Match lat lon from ingrid to netcdf file ncdf
        getsinglecellID(file_soil, grid[i].coord.lon, grid[i].coord.lat, &lonMatch, &latMatch);
    
        start_soil[0]=0;
        start_soil[1]=latMatch;
        start_soil[2]=lonMatch;
        count_soil[0]=1;
        count_soil[1]=1;
        count_soil[2]=1;
    
        // Read the variable name from last dimension from either FAO grid
        char *varsoil;
        varsoil="soil";
    
        if ((retval = nc_inq_varid(file_soil, varsoil, &varid_soilcode))){
          fprintf(stderr,"Error reading soil variable from file, soil varname should be '%s'; check variable names.\n",varsoil);
          ERR(retval);
        }
    
        // Read the data from netcdf file
        if ((retval = nc_get_vara_int(file_soil, varid_soilcode, start_soil, count_soil, soilcode_db)))
             ERR(retval);
    
        soilcode=soilcode_db[0];
      #endif
    
      //Reread soil par for each cell loop
      Soilpar *soilpar;
      fscansoilpar(&soilpar,config->soilpar_filename);


      #ifdef USDA    //Convert USDA soil texture class to FAO
        //Match lat lon from ingrid to netcdf file ncdf
        getsinglecellID(file_soil, grid[i].coord.lon, grid[i].coord.lat, &lonMatch, &latMatch);

        size_t start_soil_usda[2], count_soil_usda[2];
        start_soil_usda[0]=latMatch;
        start_soil_usda[1]=lonMatch;
        count_soil_usda[0]=1;
        count_soil_usda[1]=1;

        char *varrefdepth;
        char *varusda_top, *varoc_top, *varsand_top, *varclay_top, *varsilt_top, *varoc_bot, *varsand_bot, *varclay_bot, *varsilt_bot;
        int varid_refdepth;
        int varid_usda_top, varid_oc_top, varid_sand_top, varid_clay_top, varid_silt_top;
        int varid_oc_bot, varid_sand_bot, varid_clay_bot, varid_silt_bot;
        int refdepth[1];
        float usda_top[1], oc_bot[1], sand_bot[1], clay_bot[1], silt_bot[1], oc_top[1], sand_top[1], clay_top[1], silt_top[1];

        //Get the USDA data
        varusda_top="t_usda_tex";
        varoc_top="t_oc";
        varsand_top="t_sand";
        varclay_top="t_clay";
        varsilt_top="t_silt";
        varoc_bot="s_oc";
        varsand_bot="s_sand";
        varclay_bot="s_clay";
        varsilt_bot="s_silt";
        varrefdepth="ref_depth";

        //Get NETCDF codes for each variable
        if ((retval = nc_inq_varid(file_soil, varusda_top, &varid_usda_top))){
           fprintf(stderr,"Error reading soil variable from file,  usda texture varname should be '%s'; check variable names.\n",varusda_top);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varoc_top, &varid_oc_top))){
           fprintf(stderr,"Error reading soil variable from file,  top ogranic content varname should be '%s'; check variable names.\n",varoc_top);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varsand_top, &varid_sand_top))){
           fprintf(stderr,"Error reading soil variable from file,  top sand fraction varname should be '%s'; check variable names.\n",varsand_top);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varclay_top, &varid_clay_top))){
           fprintf(stderr,"Error reading soil variable from file,  top clay fraction varname should be '%s'; check variable names.\n",varclay_top);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varsilt_top, &varid_silt_top))){
           fprintf(stderr,"Error reading soil variable from file,  top silt fraction varname should be '%s'; check variable names.\n",varsilt_top);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varoc_bot, &varid_oc_bot))){
           fprintf(stderr,"Error reading soil variable from file,  bottom organic content varname should be '%s'; check variable names.\n",varoc_bot);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varsand_bot, &varid_sand_bot))){
           fprintf(stderr,"Error reading soil variable from file,  bottom sand fraction varname should be '%s'; check variable names.\n",varsand_bot);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varclay_bot, &varid_clay_bot))){
           fprintf(stderr,"Error reading soil variable from file,  bottom clay fraction varname should be '%s'; check variable names.\n",varclay_bot);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varsilt_bot, &varid_silt_bot))){
           fprintf(stderr,"Error reading soil variable from file,  bottom silt fraction varname should be '%s'; check variable names.\n",varsilt_bot);
           ERR(retval);
        }
        if ((retval = nc_inq_varid(file_soil, varrefdepth, &varid_refdepth))){
           fprintf(stderr,"Error reading soil variable from file,  reference depth varname should be '%s'; check variable names.\n",varrefdepth);
           ERR(retval);
        }

        // Read the data from netcdf file
        if ((retval = nc_get_vara_float(file_soil, varid_usda_top, start_soil_usda, count_soil_usda, usda_top)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_oc_top, start_soil_usda, count_soil_usda, oc_top)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_sand_top, start_soil_usda, count_soil_usda, sand_top)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_clay_top, start_soil_usda, count_soil_usda, clay_top)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_silt_top, start_soil_usda, count_soil_usda, silt_top)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_oc_bot, start_soil_usda, count_soil_usda, oc_bot)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_sand_bot, start_soil_usda, count_soil_usda, sand_bot)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_clay_bot, start_soil_usda, count_soil_usda, clay_bot)))
           ERR(retval);
        if ((retval = nc_get_vara_float(file_soil, varid_silt_bot, start_soil_usda, count_soil_usda, silt_bot)))
           ERR(retval);
        if ((retval = nc_get_vara_int(file_soil, varid_refdepth, start_soil_usda, count_soil_usda, refdepth)))
           ERR(retval);

        //Populate the first structure with the soil parameters using Saxton's equations
        if(usda_top[0]>0){
          fscansoilpar_ncdf(&soilpar, usda_top[0], oc_top[0]/100., sand_top[0]/100., clay_top[0]/100., silt_top[0]/100., 
                            oc_bot[0]/100., sand_bot[0]/100., clay_bot[0]/100., silt_bot[0]/100.);
          //Set soil code to 1 because soilpar is only populated for the first structure
          soilcode=1;
        } else {
          //Set soil code to 0 if no usda soil class
          soilcode=0;
        }

        //Distribute soil depth to upper and lower pools (and convert from cm to m)
        #ifdef SOILDEPTH
          soildepth[0]=(double)refdepth[0]*0.3/100;
          soildepth[1]=(double)refdepth[0]*0.7/100;
          printf("depth1=%f,depth2=%f\n",soildepth[0],soildepth[1]);
        
          //Remove grid cells where no soil depth information
          if(refdepth[0] < 0)
          soilcode=0;
        #endif

      #endif

      #ifdef TOPMODEL //read in parameters for topmodel module
        int varid_cs, varid_fmax, varid_cti, varid_tj, varid_tm;
        float topm_cs[1], topm_fmax[1], topm_cti[1], topm_jt[1], topm_mt[1];

        //Match lat lon from ingrid to netcdf file ncdf
        getsinglecellID(file_topm, grid[i].coord.lon, grid[i].coord.lat, &lonMatch, &latMatch);

        size_t start_topm[3], count_topm[3];
        start_topm[0]=0;
        start_topm[1]=latMatch;
        start_topm[2]=lonMatch;
        count_topm[0]=1;
        count_topm[1]=1;
        count_topm[2]=1;

        char *varcs,*varfmax,*varcti,*vartj,*vartm;
        varcs   ="cs";
        varfmax ="fmax";
        varcti  ="mcti";
        vartj   ="temp_j";
        vartm   ="temp_m";

        // Read the variable name from last dimension
        if ((retval = nc_inq_varid(file_topm, varcs, &varid_cs))){
          fprintf(stderr,"Error reading topmodel variable from file,  'cs' varname should be '%s'; check variable names.\n",varcs);
          ERR(retval);
        }
        if ((retval = nc_inq_varid(file_topm, varfmax, &varid_fmax))){
          fprintf(stderr,"Error reading topmodel variable from file,  'fmax' varname should be '%s'; check variable names.\n",varfmax);
          ERR(retval)
        }
        if ((retval = nc_inq_varid(file_topm, varcti, &varid_cti))){
          fprintf(stderr,"Error reading topmodel variable from file,  'mcti' varname should be '%s'; check variable names.\n",varcti);
          ERR(retval);
        }
        if ((retval = nc_inq_varid(file_topm, vartj, &varid_tj))){
          fprintf(stderr,"Error reading topmodel variable from file,  'temp_j' varname should be '%s'; check variable names.\n",vartj);
          ERR(retval);
        }
        if ((retval = nc_inq_varid(file_topm, vartm, &varid_tm))){
          fprintf(stderr,"Error reading topmodel variable from file,  'temp_m' varname should be '%s'; check variable names.\n",vartm);
          ERR(retval);
        }

        // Read the data from netcdf file
        if ((retval = nc_get_vara_float(file_topm, varid_cs, start_topm, count_topm, topm_cs)))
          ERR(retval);
        if ((retval = nc_get_vara_float(file_topm, varid_fmax, start_topm, count_topm, topm_fmax)))
          ERR(retval);
        if ((retval = nc_get_vara_float(file_topm, varid_cti, start_topm, count_topm, topm_cti)))
          ERR(retval);
        if ((retval = nc_get_vara_float(file_topm, varid_tj, start_topm, count_topm, topm_jt)))
          ERR(retval);
        if ((retval = nc_get_vara_float(file_topm, varid_tm, start_topm, count_topm, topm_mt)))
          ERR(retval);

      #endif

      #ifdef PERMAFROST //read in soil temerpature data for permafrost initialization

        int varid_tsl;
        float tsl[1];

        //Match lat lon from ingrid to netcdf file ncdf
        getsinglecellID(file_tsl, grid[i].coord.lon, grid[i].coord.lat, &lonMatch, &latMatch);

        size_t start_tsl[3], count_tsl[3];
        start_tsl[0]=0;
        start_tsl[1]=latMatch;
        start_tsl[2]=lonMatch;
        count_tsl[0]=1;
        count_tsl[1]=1;
        count_tsl[2]=1;

        char *vartsl;
        vartsl   ="msoiltemp";

        // Read the variable name from last dimension
        if ((retval = nc_inq_varid(file_tsl, vartsl, &varid_tsl))){
          fprintf(stderr,"Error reading soil temperature variable from file,  soit temp  varname should be '%s'; check variable names.\n",vartsl);
          ERR(retval);
        }
            
        // Read the data from netcdf file
        if ((retval = nc_get_vara_float(file_tsl, varid_tsl, start_tsl, count_tsl, tsl)))
        ERR(retval);
      #endif
  
      #ifdef PFTFIX
        int varid_pft, pft_db[1];
        // Read the variable name from last dimension
        if ((retval = nc_inq_varid(file_soil, "PFT", &varid_pft))){
          fprintf(stderr,"Error reading  PFTFIX variable from soil file,  pft fix varname should be 'PFT'; check variable names.\n");
          ERR(retval);
        }

        // Read the data from netcdf file
        if ((retval = nc_get_vara_int(file_soil, varid_pft, start_soil, count_soil, pft_db)))
          ERR(retval);

        pft=pft_db[0];
      #endif

    #endif //end CLM_NCDF

    if(fread(&cowcode,sizeof(cowcode),1,file_countrycode)!=1){                                                                                                        fprintf(stderr,"Unexpected end of file in '%s', number of gridcells truncated to %d.\n",config->countrycode_filename,i);  
      config->ngridcell=i;                                                                                                 
      break;                                                                                                               
    }
    if(fread(&regcode,sizeof(regcode),1,file_countrycode)!=1){                                                                                                        fprintf(stderr,"Unexpected end of file in '%s', number of gridcells truncated to %d.\n",config->countrycode_filename,i);  
      config->ngridcell=i;                                                                                                 
      break;                                                                                                               
    }
    if(swap_cow){ 
      cowcode=swapshort(cowcode);
      regcode=swapshort(regcode);
    }

    #ifdef NAT_VEG //RESET cowcode and region code to be dummy values
      cowcode=195;
      regcode=308;
    #endif

    if(cowcode<0||cowcode>ncountries||regcode<0||regcode>nregions){
       if(soilcode>=1&&soilcode<nsoil)
         fprintf(stderr," Invalid countrycode=%d or regioncode=%d with valid soilcode in cell (not skipped)\n",cowcode,regcode);
    }else initmanage(&grid[i].manage,countrypar+cowcode,regionpar+regcode);

    #ifdef RIVER_ROUTING
      //Caution/Warning printed to console when usingu RIVER_ROUTING && GROSS_LUC
      //..because LUH data has its own ice/water fractions
      #ifndef LUHWATER
        #if defined(GROSS_LUC) || defined(NETinGROSS)
          fprintf(stderr,"\n CAUTION...gross landusechange requires ice/water dataset as input from LUH data...it will cause conflicts...");
        #endif
    
        if(fread(&lakefrac,sizeof(lakefrac),1,file_lakes)!=1){
          fprintf(stderr,"Unexpected end of file in '%s'.\n",config->lakes_filename);
          break;
        }
        grid[i].lakefrac=(Real)lakefrac*0.01;
      #else
        //get lake fractions from netcdf LUH data
        int varid_icewater;
        float icewater_frac[1];
        size_t start_icewater[3], count_icewater[3];
    
        //Match lat lon from ingrid to netcdf file ncdf
        getsinglecellID(file_icewater, grid[i].coord.lon, grid[i].coord.lat, &lonMatch, &latMatch);
    
        start_icewater[0]=0;
        start_icewater[1]=latMatch;
        start_icewater[2]=lonMatch;
        count_icewater[0]=1;
        count_icewater[1]=1;
        count_icewater[2]=1;

        // Read the variable name from last dimension from either grid
        char *varicewater;
        varicewater="gicew";
    
        if ((retval = nc_inq_varid(file_icewater, varicewater, &varid_icewater))){
                 fprintf(stderr,"Error reading icewater variable from file,  ice water fraction varname should be '%s'; check variable names.\n",varicewater);
           ERR(retval);
              }
    
        // Read the data from netcdf file
        if ((retval = nc_get_vara_float(file_icewater, varid_icewater, start_icewater, count_icewater, icewater_frac)))
           ERR(retval);
    
        //resolve floating-point errors by rounding
        grid[i].lakefrac=round(icewater_frac[0]*1000000)/1000000;
      #endif
    #endif

    #ifdef STANDAGEINIT
        //set up forest stand age
        if(fread(&standage,sizeof(standage),1,file_standage)!=1){
          fprintf(stderr,"Unexpected end of file in '%s'.\n",config->standage_filename);
          break;
        }
        grid[i].standage =(Real)standage;
        printf( "Stand Initiation year is %f\n", grid[i].standage);
    #endif

    
    //---------------------------------------
    //   I n i t i a l i z e   C e l l s
    //---------------------------------------
    grid[i].elev=0; //for ozone
    #ifdef ELEV
      grid[i].elev=elev; //for ozone
    #endif
    grid[i].pft=pft;
    grid[i].cropfrac=0;
    grid[i].totw=grid[i].totc=0.0;
    grid[i].discharge.dmass_new=grid[i].discharge.waterdeficit=0.0;
    grid[i].discharge.dmass_lake_max=grid[i].lakefrac*H*grid[i].coord.area*1000;
    grid[i].discharge.dmass_lake=0.0;
    #ifdef TOPMODEL
      grid[i].topmodel.cs   = topm_cs[0];
      grid[i].topmodel.fmax = topm_fmax[0];
      grid[i].topmodel.cti  = topm_cti[0];
      grid[i].topmodel.tj   = topm_jt[0];
      grid[i].topmodel.tm   = topm_mt[0];
    #endif
    //wood harvest product pools
    grid[i].product_100yr = 0.0;
    grid[i].product_10yr  = 0.0;
    grid[i].product_1yr   = 0.0;
    //deforestation product pools
    grid[i].product_25yr  = 0.0;
    grid[i].product_2yr   = 0.0;
    #ifdef OUTPUT_BYSTAND
      grid[i].product_100yr_primary  = 0.0;
      grid[i].product_10yr_primary   = 0.0;
      grid[i].product_1yr_primary    = 0.0;
      grid[i].product_25yr_primary   = 0.0;
      grid[i].product_2yr_primary    = 0.0;
    
      grid[i].product_100yr_secforest= 0.0;
      grid[i].product_10yr_secforest = 0.0;
      grid[i].product_1yr_secforest  = 0.0;
      grid[i].product_25yr_secforest = 0.0;
      grid[i].product_2yr_secforest  = 0.0;
    #endif

    //---------------------------------
    // allocate memory for output
    // ..check vectors
    //---------------------------------
    grid[i].gdd=newgdd(npft+ncft);

    grid[i].laimax=newvec(Real,ncft);                      /* country-specific laimax*/

    grid[i].cropdates.fall_sdate20=newvec(int,ncft);       /* sowing dates */
      check(grid[i].cropdates.fall_sdate20);
    grid[i].cropdates.last_update_fall=newvec(int,ncft);   /* sowing dates */
      check(grid[i].cropdates.last_update_fall);
    grid[i].cropdates.spring_sdate20=newvec(int,ncft);     /* sowing dates */
      check(grid[i].cropdates.spring_sdate20);
    grid[i].cropdates.last_update_spring=newvec(int,ncft); /* sowing dates */
      check(grid[i].cropdates.last_update_spring);
    grid[i].cropdates.vern_date20=newvec(int,ncft);        /* vernalization dates */
      check(grid[i].cropdates.vern_date20);
    grid[i].cropdates.last_update_vern=newvec(int,ncft);   /* vernalization dates */
      check(grid[i].cropdates.last_update_vern);
    grid[i].cropdates.fallow=newvec(int,(ncft)*2);
      check(grid[i].cropdates.fallow);
    
    grid[i].output.sdate=newvec(int,2*ncft);
      check(grid[i].output.sdate);
    grid[i].output.pft_npp=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.pft_npp);
    grid[i].output.pft_gpp=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.pft_gpp);
    grid[i].output.pft_gcgp=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.pft_gcgp);
    grid[i].output.gcgp_count=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.gcgp_count);
    grid[i].output.pft_harvest=newvec(Real,2*(ncft+NGRASS));
      check(grid[i].output.pft_harvest);
    grid[i].output.pft_rharvest=newvec(Real,2*(ncft+NGRASS));
      check(grid[i].output.pft_rharvest);
    grid[i].output.fpc = newvec(Real, npft+1);
      check(grid[i].output.fpc);
    #ifdef OUTPUT_BYSTAND
      grid[i].output.fpc_primary = newvec(Real, npft+1);
        check(grid[i].output.fpc_primary);   
      grid[i].output.fpc_secforest = newvec(Real, npft+1);
        check(grid[i].output.fpc_secforest);   
    #endif
    grid[i].output.pft_fo3uptake=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.pft_fo3uptake);
    grid[i].output.pft_transp=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.pft_transp);
    grid[i].output.pft_gc=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.pft_gc);
    grid[i].output.pft_lai=newvec(Real,npft+2*(ncft+NGRASS));
      check(grid[i].output.pft_lai);
    grid[i].output.pft_vegc  = newvec(Real, npft);
      check(grid[i].output.pft_vegc);
    grid[i].output.pft_nind  = newvec(Real, npft);
      check(grid[i].output.pft_nind);
    grid[i].output.pft_mort  = newvec(Real, npft);
      check(grid[i].output.pft_mort);
    grid[i].output.pft_maxphenday = newvec(Real, npft);              //PHENDATE
      check(grid[i].output.pft_maxphenday);
    grid[i].output.pft_bimonfpar = newvec(Real, npft*53);
      check(grid[i].output.pft_bimonfpar);
  
    //natural pfts only
    grid[i].output.mpft_lai = newvec(Real, npft*12);                 //Monthly LAI by PFT
      check(grid[i].output.mpft_lai);
    grid[i].output.mpft_gc = newvec(Real, npft*12);                  //Monthly PFT_GC
      check(grid[i].output.mpft_gc);
    grid[i].output.mpft_ci = newvec(Real, npft*12);                  //Monthly PFT_CI
      check(grid[i].output.mpft_ci);                                  
    grid[i].output.mpft_transp = newvec(Real, npft*12);              //Monthly PFT_TRANSP
      check(grid[i].output.mpft_transp);                              
    grid[i].output.mpft_gpp = newvec(Real, npft*12);                 //Monthly PFT_GPP
      check(grid[i].output.mpft_gpp);

    #ifdef OUTPUT_AGRICULTURE_MPFT
      //crop pfts only
      grid[i].output.mpft_ag_npp = newvec(Real, 12*(2*(ncft+NGRASS)));    //Monthly NPP
        check(grid[i].output.mpft_ag_npp);
      grid[i].output.mpft_ag_gpp = newvec(Real, 12*(2*(ncft+NGRASS)));    //Monthly GPP
        check(grid[i].output.mpft_ag_gpp);
      grid[i].output.mpft_ag_lai = newvec(Real, 12*(2*(ncft+NGRASS)));    //Monthly LAI
        check(grid[i].output.mpft_ag_lai);
      grid[i].output.mpft_ag_transp = newvec(Real, 12*(2*(ncft+NGRASS))); //Monthly transpiration
        check(grid[i].output.mpft_ag_transp);
    #endif

    //initialize outputs
    initoutput_annual(&grid[i].output, npft, ncft);
    init_cropdates(pftpar,&grid[i].cropdates,npft,ncft,grid[i].coord.lat);

    if(config->restart_filename==NULL){
      grid[i].standlist=newlist();
      n=addstand(grid[i].standlist);
      stand=getstand(grid[i].standlist,n-1);
      stand->frac=1-grid[i].lakefrac;
      //init values for LUC
      stand->ageclass_standid=1;
      stand->len_frac_transition=1;

      //set first frac_transition
      stand->frac_transition[0]=stand->frac;

      if(soilcode<1 || soilcode>nsoil || grid[i].lakefrac >= 1.0){
        //cell id to help track which cells are skipped
        fprintf(stderr,"Invalid soilcode: %d, lakefrac: %f, cell id: %d skipped\n",soilcode,grid[i].lakefrac, i);
        grid[i].skip=TRUE;
      }else {
        initsoil(&stand->soil,soilpar+soilcode-1,soildepth); //SOILDEPTH - define soildepth variable when initializing
        #ifdef PERMAFROST
          initpermafrost(&stand->permafrost,&stand->soil,oc_top[0],tsl[0]);
        #endif

        grid[i].skip=FALSE;
      }
      new_climbuf(&grid[i].climbuf);
      
      if(!grid[i].skip) check_stand_fracs(grid[i].standlist,grid[i].lakefrac);  
    }else{
      #ifdef HYDE_BINARY
        getlanduse(landuse,config->firstyear,config->ngridcell,ncft);
      #else
        //Landuse driver is NETCDF
        //Get land use data on first iteration / read
        if(i==0){
          #if defined(GROSS_LUC) || defined(NETinGROSS)
            getlanduse_GROSS_ncdf(*config,landuse, config->firstyear-config->nspinup, config->ngridcell, ncft, latIn, lonIn);
          #else
            getlanduse_NET_ncdf(landuse, config->firstyear-config->nspinup, config->ngridcell, ncft, latIn, lonIn);
          #endif
        }
      #endif

      if(freadcell(file_restart,grid+i,pftpar,npft,ncft,soilpar+soilcode-1,swap,landuse,i)){
        fprintf(stderr,"Unexpected end of file in '%s', number of gridcells truncated to %d.\n",config->restart_filename,i);
        config->ngridcell=i;
        break;
      }
      if(!grid[i].skip) check_stand_fracs(grid[i].standlist,grid[i].lakefrac);
    }
  } /* of for(i=0;...) all cells*/

  if(config->restart_filename!=NULL)
    fclose(file_restart);
  closecoord(file_coord);

  #ifndef CLM_NCDF
    fclose(file_soil);
  #endif
  
  #ifdef CLM_NCDF
    if ((retval = nc_close(file_soil)))
    ERR(retval);
    #ifdef ELEV
      if ((retval = nc_close(file_elev)))
      ERR(retval);
    #endif
  #endif
  
  #if defined(LUHWATER)
    if ((retval = nc_close(file_icewater)))
    ERR(retval);
  #endif

  fclose(file_countrycode);
  return grid;
} /* of 'newgrid' */
