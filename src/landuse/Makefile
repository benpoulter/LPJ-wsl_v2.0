#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/landuse/ M a k e f i l e                                 ###
#####################################################################
include ../../Makefile.inc

OBJ	= cultivate.$O freelanduse.$O freelanduse_ncdf.$O getlanduse.$O getlanduse_NET_ncdf.$O\
          getlanduse_GROSS_ncdf.$O initlanduse.$O initlanduse_NET_ncdf.$O initlanduse_GROSS_ncdf.$O landusechange.$O\
          landusechange_NET.$O landusechange_GROSS.$O landusechange_AGECLASS_GROSS.$O crop_sum_frac.$O sowing.$O check_lu.$O\
          reclaim_land.$O allocation_today.$O\
          initmanage.$O fscancountrypar.$O fscanregionpar.$O litter_update_deforest.$O clearcutpft.$O wood_harvest.$O 

INC	= ../../include
LIB	= ../../lib/liblanduse.$A

HDRS	= $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h\
          $(INC)/landuse.h $(INC)/date.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h\
          $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h \
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h\
          $(INC)/config.h $(INC)/stand.h

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS)$(LIB) $(OBJ)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJ) $(LIB)
