#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/tools/ M a k e f i l e                                   ###
#####################################################################
include ../../Makefile.inc

OBJ	= swap.$O header.$O coord.$O openinputfile.$O openinputfile_ncdf.$O check_stand_fracs.$O findstand.$O setaside.$O\
      sort_ForestTile.$O existlandusetype.$O mix_stands.$O ageclass_transition.$O

INC	= ../../include
LIB	= ../../lib/libtools.$A

HDRS	= $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h\
          $(INC)/climate.h $(INC)/date.h $(INC)/swap.h\
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
