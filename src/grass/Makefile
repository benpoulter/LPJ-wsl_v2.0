#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/grass/ M a k e f i l e                                   ###
#####################################################################
include ../../Makefile.inc

OBJ	= lai_grass.$O isneg_grass.$O npp_grass.$O turnover_grass.$O\
          allocation_grass.$O fpc_grass.$O litter_update_grass.$O\
          phenology_grass.$O new_grass.$O light_grass.$O fpar_grass.$O\
          fwrite_grass.$O fscanpft_grass.$O fprint_grass.$O fread_grass.$O\
          establishment_grass.$O reproduction_grass.$O free_grass.$O\
          vegc_sum_grass.$O init_grass.$O mix_veg_grass.$O fire_grass.$O

INC     = ../../include
LIB	= ../../lib/libgrass.$A


HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h\
          $(INC)/grass.h $(INC)/errmsg.h $(INC)/numeric.h\
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
