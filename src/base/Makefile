#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/base/ M a k e f i l e                                    ###
#####################################################################
include ../../Makefile.inc

OBJ	= fscanpftpar.$O firepft.$O climbuf.$O pftlist.$O newpft.$O\
          survive.$O establish.$O fprintstand.$O temp_stress.$O\
          updategdd.$O fwritecell.$O fwriterestart.$O freadcell.$O\
          fscanconfig.$O nomortality.$O noinit.$O nofire.$O\
          noreproduction.$O noturnover.$O noallocation.$O noestablishment.$O\
          freepft.$O photosynthesis.$O freegrid.$O killpft.$O\
          light.$O gp.$O gp_sum.$O water_stressed.$O interception.$O fpc_sum.$O\
          establishmentpft.$O noadjust.$O nolight.$O freadpft.$O fwritepft.$O\
          fwritestand.$O fprintpft.$O fprintcell.$O writecoords.$O\
          freadstand.$O equilsom.$O initgdd.$O standlist.$O\
          nomix_veg.$O init_cropdates.$O check_fluxes.$O\
          drain.$O initdrain.$O

INC	= ../../include
LIB	= ../../lib/libbase.$A

HDRS	= $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/manage.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/tree.h $(INC)/header.h\
          $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/managepar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/discharge.h $(INC)/cell.h  $(INC)/units.h \
          $(INC)/config.h $(INC)/popen.h


$(LIB): $(OBJ)
	$(AR) $(ARFLAGS)$(LIB) $(OBJ)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJ) $(LIB)
