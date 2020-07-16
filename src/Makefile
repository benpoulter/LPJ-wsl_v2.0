#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  src/ M a k e f i l e                                         ###
#####################################################################
include ../Makefile.inc

OBJS	= cru2clm.$O headergrid.$O cfts26_lu2clm.$O drainage.$O\
          river_sections_input_grid.$O river_sections_input_soil.$O\
          river_sections_input_climate.$O river_sections_input_landuse.$O\
          river_sections_input_countrycode.$O\
          river_sections_input_irrig.$O\
          printlpj.$O catlpj.$O cat2bsq.$O clm2bsq.$O grd2bsq.$O lpj.$O 

SRC	= cru2clm.c headergrid.c cfts26_lu2clm.c drainage.c\
          river_sections_input_grid.c river_sections_input_soil.c\
          river_sections_input_climate.c river_sections_input_landuse.c\
          river_sections_input_countrycode.c\
          river_sections_input_irrig.c\
          printlpj.c catlpj.c cat2bsq.c clm2bsq.c grd2bsq.c

INC     = ../include

HDRS    = $(INC)/buffer.h $(INC)/coord.h $(INC)/lpj.h $(INC)/pftlist.h\
          $(INC)/soil.h $(INC)/manage.h $(INC)/climate.h $(INC)/date.h $(INC)/pft.h\
          $(INC)/pftpar.h $(INC)/types.h $(INC)/tree.h $(INC)/header.h\
          $(INC)/landuse.h $(INC)/crop.h $(INC)/grass.h $(INC)/errmsg.h $(INC)/numeric.h\
          $(INC)/conf.h $(INC)/swap.h $(INC)/soilpar.h $(INC)/managepar.h $(INC)/stand.h\
          $(INC)/list.h $(INC)/cell.h  $(INC)/units.h $(INC)/output.h\
          $(INC)/config.h $(INC)/popen.h $(INC)/input.h 

LIB	= ../lib

LPJLIBS	= $(LIB)/liblpj.$A  $(LIB)/libbase.$A  $(LIB)/libsoil.$A\
          $(LIB)/libtree.$A  $(LIB)/libgrass.$A  $(LIB)/libnum.$A\
          $(LIB)/libclimate.$A $(LIB)/liblanduse.$A $(LIB)/libtools.$A\
          $(LIB)/libcrop.$A

EXE	= ../lpj$E

bin: 
	(cd base && $(MAKE))
	(cd climate && $(MAKE))
	(cd numeric && $(MAKE))
	(cd lpj && $(MAKE))
	(cd soil && $(MAKE))
	(cd tree && $(MAKE))
	(cd grass && $(MAKE))
	(cd crop && $(MAKE))
	(cd tools && $(MAKE))
	(cd landuse && $(MAKE))
	$(MAKE) lpj

clean:
	(cd base && $(MAKE) clean)
	(cd climate && $(MAKE) clean)
	(cd lpj && $(MAKE) clean)
	(cd numeric && $(MAKE) clean)
	(cd soil && $(MAKE) clean)
	(cd tree && $(MAKE) clean)
	(cd grass && $(MAKE) clean)
	(cd crop && $(MAKE) clean)
	(cd tools && $(MAKE) clean)
	(cd landuse && $(MAKE) clean)
	$(RM) $(RMFLAGS) $(OBJS) ../lpj$E ../cru2clm$E ../headergrid$E ../cfts26_lu2clm$E ../drainage$E\
                                 ../river_sections_input_grid$E ../river_sections_input_soil$E\
                                 ../river_sections_input_climate$E ../river_sections_input_landuse$E\
                                 ../river_sections_input_countrycode$E\
                                 ../river_sections_input_irrig$E\
                                 ../catlpj$E ../cat2bsq$E ../clm2bsq$E ../grd2bsq$E ../printlpj$E

$(OBJS): $(HDRS)

.c.$O: 
	$(CC) $(CFLAGS) -I$(INC) -c $*.c

lpj: $(LPJLIBS) lpj.$O
	$(LINK) $(LNOPTS)$(EXE) lpj.$O $(LPJLIBS)  $(LIBS)  

../printlpj: printlpj.$O $(LPJLIBS)
	$(LINK) $(LNOPTS) ../printlpj$E printlpj.$O $(LIBS) $(LPJLIBS)

../cru2clm$E: cru2clm.$O $(INC)/types.h $(INC)/swap.h $(INC)/header.h\
        $(INC)/date.h $(INC)/buffer.h $(INC)/coord.h $(INC)/config.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../cru2clm$E cru2clm.$O $(LIB)/libnum.$A $(LIB)/libtools.$A 

../headergrid$E: headergrid.$O $(INC)/types.h $(INC)/swap.h $(INC)/header.h\
        $(INC)/date.h $(INC)/buffer.h $(INC)/coord.h $(INC)/config.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../headergrid$E headergrid.$O $(LIB)/libnum.$A $(LIB)/libtools.$A 

../cfts26_lu2clm$E: cfts26_lu2clm.$O $(INC)/types.h $(INC)/swap.h $(INC)/header.h\
        $(INC)/date.h $(INC)/buffer.h $(INC)/coord.h $(INC)/config.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../cfts26_lu2clm$E cfts26_lu2clm.$O $(LIB)/libnum.$A $(LIB)/libtools.$A

../drainage$E: drainage.$O $(INC)/types.h $(INC)/swap.h $(INC)/header.h $(INC)/date.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LM) $(LNOPTS)../drainage$E drainage.$O $(LIB)/libnum.$A $(LIB)/libtools.$A 

../river_sections_input_grid$E: river_sections_input_grid.$O $(INC)/types.h $(INC)/swap.h\
        $(INC)/header.h $(INC)/date.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../river_sections_input_grid$E river_sections_input_grid.$O\
        $(LIB)/libnum.$A $(LIB)/libtools.$A 

../river_sections_input_soil$E: river_sections_input_soil.$O $(INC)/types.h $(INC)/swap.h\
        $(INC)/header.h $(INC)/date.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../river_sections_input_soil$E river_sections_input_soil.$O\
        $(LIB)/libnum.$A $(LIB)/libtools.$A 

../river_sections_input_climate$E: river_sections_input_climate.$O $(INC)/types.h $(INC)/swap.h\
        $(INC)/header.h $(INC)/date.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../river_sections_input_climate$E river_sections_input_climate.$O\
        $(LIB)/libnum.$A $(LIB)/libtools.$A 

../river_sections_input_landuse$E: river_sections_input_landuse.$O $(INC)/types.h $(INC)/swap.h\
        $(INC)/header.h $(INC)/date.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../river_sections_input_landuse$E river_sections_input_landuse.$O\
        $(LIB)/libnum.$A $(LIB)/libtools.$A 

../river_sections_input_irrig$E: river_sections_input_irrig.$O $(INC)/types.h $(INC)/swap.h\
        $(INC)/header.h $(INC)/date.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../river_sections_input_irrig$E river_sections_input_irrig.$O\
        $(LIB)/libnum.$A $(LIB)/libtools.$A 

../river_sections_input_countrycode$E: river_sections_input_countrycode.$O $(INC)/types.h $(INC)/swap.h\
        $(INC)/header.h $(INC)/date.h\
        $(LIB)/libnum.$A $(LIB)/libtools.$A
	$(LINK) $(LNOPTS)../river_sections_input_countrycode$E river_sections_input_countrycode.$O\
        $(LIB)/libnum.$A $(LIB)/libtools.$A

../catlpj$E: catlpj.$O $(INC)/types.h $(INC)/swap.h $(INC)/header.h\
             $(LIB)/libtools.$(A) $(LIB)/libnum.$(A)  
	$(LINK) $(LNOPTS)../catlpj$E catlpj.$(O) $(LIB)/libtools.$(A) $(LIB)/libnum.$(A)

../cat2bsq$E: cat2bsq.$O $(LPJLIBS)
	$(LINK) $(LNOPTS) ../cat2bsq$E cat2bsq.$O $(LIBS) $(LPJLIBS)

../grd2bsq$E: grd2bsq.$O $(LPJLIBS)
	$(LINK) $(LNOPTS) ../grd2bsq$E grd2bsq.$O $(LIBS) $(LPJLIBS)

all: $(EXE) ../printlpj ../cru2clm ../headergrid ../cfts26_lu2clm ../drainage\
            ../river_sections_input_grid ../river_sections_input_soil\
            ../river_sections_input_climate ../river_sections_input_landuse\
            ../river_sections_input_countrycode\
            ../riversections_input_irrig\
            ../catlpj ../cat2bsq ../clm2bsq ../grd2bsq
