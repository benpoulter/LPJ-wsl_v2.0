#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/tree/ M a k e f i l e                                    ###
#####################################################################
include ../../Makefile.inc

OBJ	= lai_tree.$O turnover_tree.$O npp_tree.$O phenology_tree.$O\
          reproduction_tree.$O mortality_tree.$O init_tree.$O\
          allocation_tree.$O isneg_tree.$O light_tree.$O fpar_tree.$O\
          litter_update_tree.$O fpc_tree.$O allometry_tree.$O\
          new_tree.$O fwrite_tree.$O fscanpft_tree.$O  fprint_tree.$O \
          fread_tree.$O establishment_tree.$O fire_tree.$O free_tree.$O\
          vegc_sum_tree.$O adjust_tree.$O mix_veg_tree.$O

INC     = ../../include
LIB	= ../../lib/libtree.$A

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/tree.h\
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
