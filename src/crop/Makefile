#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/crop/ M a k e f i l e                                    ###
#####################################################################
include ../../Makefile.inc

OBJ	= lai_crop.$O npp_crop.$O phenology_crop.$O\
          isneg_crop.$O cropdates.$O litter_update_crop.$O\
          fpc_crop.$O new_crop.$O fwrite_crop.$O fscanpft_crop.$O\
          fprint_crop.$O fread_crop.$O free_crop.$O vegc_sum_crop.$O\
          allocation_daily_crop.$O phen_variety.$O fpar_crop.$O freecropdates.$O\
          harvest_grass.$O irrig_amount.$O laimax_manage.$O irrigation.$O

INC     = ../../include
LIB	= ../../lib/libcrop.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/cropdates.h $(INC)/crop.h\
          $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h \
          $(INC)/config.h

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS)$(LIB) $(OBJ)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c
clean:
	$(RM) $(RMFLAGS) $(OBJ) $(LIB)
