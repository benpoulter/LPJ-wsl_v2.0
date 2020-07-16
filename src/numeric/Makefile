#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/numeric/ M a k e f i l e                                 ###
#####################################################################
include ../../Makefile.inc

OBJ	= leftmostzero.$O bisect.$O linreg.$O fail.$O date.$O interpolate.$O\
          list.$O buffer.$O fscanreal.$O fscanstring.$O\
          rand.$O popen.$O petpar.$O petparRAD.$O

INC	= ../../include
LIB	= ../../lib/libnum.$A

HDRS	= $(INC)/types.h $(INC)/numeric.h $(INC)/date.h\
          $(INC)/list.h $(INC)/buffer.h $(INC)/errmsg.h $(INC)/popen.h

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS)$(LIB) $(OBJ)

$(OBJ): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

clean: 
	$(RM) $(RMFLAGS) $(OBJ) $(LIB)
