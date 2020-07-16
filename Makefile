#####################################################################
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                  |  L P J  -  w s l  |                        ###
###                  +-+-+-+-+-+-+-+-+-+-+                        ###
###                                                               ###
###  M a k e f i l e                                              ###
#####################################################################
include Makefile.inc

INC     = include

HDRS    = $(INC)/buffer.h $(INC)/cell.h $(INC)/climate.h $(INC)/conf.h  $(INC)/config.h\
          $(INC)/coord.h $(INC)/crop.h $(INC)/cropdates.h $(INC)/date.h $(INC)/discharge.h\
          $(INC)/errmsg.h $(INC)/grass.h $(INC)/header.h $(INC)/input.h $(INC)/landuse.h\
          $(INC)/list.h $(INC)/lpj.h $(INC)/manage.h $(INC)/managepar.h $(INC)/numeric.h\
          $(INC)/output.h $(INC)/pft.h $(INC)/pftlist.h $(INC)/pftpar.h $(INC)/popen.h\
          $(INC)/soil.h $(INC)/soilpar.h $(INC)/stand.h $(INC)/swap.h $(INC)/tree.h\
          $(INC)/types.h $(INC)/units.h ${INC}/topmodel.h ${INC}/permafrost.h

CONF	= lpj.conf output.conf cru_1901-2003.conf

DATA    = par/manage.par par/manage_reg.par par/pft.par par/soil.par

CRU	= input/soil.bin input/cow_mg_2006.bin input/co2_2003.dat input/cld2003.clm\
          input/tmp2003.clm input/pre2003.clm input/wet2003.clm input/grid.bin

SCRIPTS	= configure.bat configure.sh distribute_river.sh distribute.sh\
          newpft.sh output_bsq.ksh

FILES	= Makefile Makefile.aix Makefile.gcc Makefile.win32\
          AUTHORS LPJ_Doc.txt INSTALL VERSION\
          $(CONF) $(DATA) $(HDRS) $(SCRIPTS) 

bin: 
	$(MKDIR) lib
	(cd src && $(MAKE))
cat:
	(cd src && $(MAKE) ../catlpj)
	(cd src && $(MAKE) ../cat2bsq)
	(cd src && $(MAKE) ../grd2bsq)
clm:
	(cd src && $(MAKE) ../cru2clm)
	(cd src && $(MAKE) ../cfts26_lu2clm)
	(cd src && $(MAKE) ../headergrid)
	(cd src && $(MAKE) ../drainage)
	(cd src && $(MAKE) ../river_sections_input_grid)
	(cd src && $(MAKE) ../river_sections_input_soil)
	(cd src && $(MAKE) ../river_sections_input_climate)
	(cd src && $(MAKE) ../river_sections_input_landuse)
	(cd src && $(MAKE) ../river_sections_input_countrycode)
	(cd src && $(MAKE) ../river_sections_input_irrig)
print:
	(cd src && $(MAKE) ../printlpj)

all: bin cat clm print

test: bin
	$(MKDIR) output
	./lpj

clean:
	rm -r lib
	(cd src  && $(MAKE) clean)

archive: 
	tar -cf lpjv3_2_LUC.tar $(FILES) src/Makefile src/*.c\
	    src/base/*.c src/base/Makefile src/climate/Makefile src/climate/*.c\
	    src/crop/*.c src/crop/Makefile src/grass/*.c src/grass/Makefile\
	    src/landuse/*.c src/landuse/Makefile src/lpj/*.c src/lpj/Makefile\
	    src/numeric/*.c src/numeric/Makefile src/soil/*.c src/soil/Makefile\
	    src/tools/*.c src/tools/Makefile src/tree/*.c src/tree/Makefile
	    gzip -f lpjv3_2_LUC.tar

zipfile: 
	zip -l lpjv3_2_LUC.tar $(FILES) src/Makefile src/*.c\
	    src/base/*.c src/base/Makefile src/climate/Makefile src/climate/*.c\
	    src/crop/*.c src/crop/Makefile src/grass/*.c src/grass/Makefile\
	    src/landuse/*.c src/landuse/Makefile src/lpj/*.c src/lpj/Makefile\
	    src/numeric/*.c src/numeric/Makefile src/soil/*.c src/soil/Makefile\
	    src/tools/*.c src/tools/Makefile src/tree/*.c src/tree/Makefile

cru.zip: $(CRU)
	zip cru.zip $(CRU)

cru.tar: $(CRU)
	tar -cf cru.tar $(CRU)
