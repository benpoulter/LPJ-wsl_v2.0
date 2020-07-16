#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/climate/ M a k e f i l e                                 ###
#####################################################################
include ../../Makefile.inc

OBJ	= initclimate.$O getclimate.$O freeclimate.$O\
	freeclimate_daily_ncdf.$O getclimate_daily_ncdf.$O initclimate_daily_ncdf.$O getinund_ncdf.$O\
          prdaily.$O getclimate.$O getco2.$O getco2_ncdf.$O getclimate_ncdf.$O getcellID.$O getsinglecellID.$O freeclimate_ncdf.$O initclimate_ncdf.$O

INC	= ../../include
LIB	= ../../lib/libclimate.$A

HDRS	= $(INC)/buffer.h $(INC)/lpj.h\
          $(INC)/climate.h $(INC)/date.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/header.h\
          $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h \
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h\
          $(INC)/config.h


$(LIB): $(OBJ)
	$(AR) $(ARFLAGS)$(LIB) $(OBJ)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJ) $(LIB)
