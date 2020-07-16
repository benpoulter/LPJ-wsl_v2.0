#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/lpj/ M a k e f i l e                                     ###
#####################################################################
include ../../Makefile.inc

OBJ	= update_daily.$O update_annual.$O init_annual.$O update_monthly.$O\
           newgrid.$O iterate.$O output.$O iterateyear.$O

INC	= ../../include
LIB	= ../../lib/liblpj.$A

HDRS	= $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/manage.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/crop.h $(INC)/errmsg.h\
          $(INC)/numeric.h $(INC)/header.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/managepar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/output.h\
          $(INC)/config.h


$(LIB): $(OBJ)
	$(AR) $(ARFLAGS)$(LIB) $(OBJ)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJ) $(LIB)
