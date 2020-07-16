#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/soil/ M a k e f i l e                                    ###
#####################################################################
include ../../Makefile.inc

OBJ	= initsoil.$O soil.$O waterbalance.$O equilsoil.$O littersom.$O\
          getlag.$O temp_response.$O fscansoilpar.$O fscansoilpar_ncdf.$O soiltemp.$O snow.$O\
          fire_sum.$O fire_prob.$O freadsoil.$O fwritesoil.$O fprintsoil.$O\
		  litter_update_monthly.$O litter_update_daily.$O topmodel.$O\
		  initpermafrost.$O freadpermafrost.$O fwritepermafrost.$O permafrost.$O

INC     = ../../include
LIB	= ../../lib/libsoil.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h\
          $(INC)/crop.h $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/output.h\
          $(INC)/config.h $(INC)/popen.h

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS)$(LIB) $(OBJ)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJ) $(LIB)  
