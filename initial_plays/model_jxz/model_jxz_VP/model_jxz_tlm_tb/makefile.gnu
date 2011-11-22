
###############################################################################
### VP Component Makefile for model_jxz_tlm_tb
###############################################################################

####### Files

HEADERS =	include/mw_support_tb.h include/model_jxz_tlm_tb_def.h include/model_jxz_tlm_tb.h
		
SOURCES =	src/mw_support_tb.cpp src/model_jxz_tlm_tb.cpp src/model_jxz_tlm_tb_main.cpp
		
OBJECTS =	obj/mw_support_tb.o obj/model_jxz_tlm_tb.o obj/model_jxz_tlm_tb_main.o

STATICLIB 	=	../model_jxz_tlm/lib/libmodel_jxz_tlm.a ../model_jxz/lib/libmodel_jxz.a
 
EXEC		=	model_jxz_tlm_tb.exe

####### Tools

AS		=	as
CC		=	gcc
CXX		=	gcc
LD		=	gcc
AR		=	ar
RANLIB	=	ranlib
MAKE    = make

####### Inc path

SYSTEMC_INCPATH =	'$(SYSTEMC_INC_PATH)'
TLM_INCPATH     =	'$(TLM_INC_PATH)'
MATLAB_INCPATH  = '/export/scratch/opt/extern/include'

####### Lib path

SYSTEMC_LIBPATH =	'$(SYSTEMC_LIB_PATH)'
MATLAB_LIBPATH =  '/export/scratch/opt/bin/glnx86'
MATLAB_LIBPATH2 = '/export/scratch/opt/sys/os/glnx86'

SYSTEMC_LIBNAME = '$(SYSTEMC_LIB_NAME)'

####### Include, Lib

INCDIR		= -I ./include -I '../model_jxz_tlm/include' -I '../model_jxz/include' -I '../model_jxz/utils' -I $(SYSTEMC_INCPATH) -I $(TLM_INCPATH) -I $(MATLAB_INCPATH)
LIBDIR		= -L $(MATLAB_LIBPATH) -Wl,-rpath $(MATLAB_LIBPATH) -Wl,-rpath $(MATLAB_LIBPATH2)
LIBS		= $(STATICLIB) $(SYSTEMC_LIBPATH)/$(SYSTEMC_LIBNAME) -lmat -lmx

SC_OPTION	= -D SC_INCLUDE_DYNAMIC_PROCESSES

LOG_OPTION  =   -DTLMG_CURR_VERBOSITY=tlmgPrintTerse

####### Flags

ASFLAGS		=
CFLAGS		=	$(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS) -Wall $(INCDIR) $(SC_OPTION) $(LOG_OPTION)
CXXFLAGS	=	$(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS) -Wall -Wno-deprecated $(INCDIR) $(SC_OPTION) $(LOG_OPTION)
LDFLAGS		=	$(LIBDIR) -Wl,--start-group $(LIBS) -Wl,--end-group -lm -lstdc++ 
ARFLAGS 	=	cru

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c .s

obj/%.o: src/%.s
	$(AS) $(ASFLAGS) -o $@ $<

obj/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS)  -o $@ $<

obj/%.o: src/%.cxx
	$(CXX) -c $(CXXFLAGS)  -o $@ $<

obj/%.o: src/%.cc
	$(CXX) -c $(CXXFLAGS)  -o $@ $<

obj/%.o: src/%.C
	$(CXX) -c $(CXXFLAGS)  -o $@ $<

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS)  -o $@ $<

####### Targets
all:
	$(MAKE) -f makefile.gnu all-am OPT_CXXFLAGS="-O3"

all-debug:
	$(MAKE) -f makefile.gnu all-am OPT_CXXFLAGS="-g"

relink:
	$(MAKE) -f makefile.gnu relink-am OPT_CXXFLAGS="-O3"

relink-debug:
	$(MAKE) -f makefile.gnu relink-am OPT_CXXFLAGS="-g"

all-am: dep $(EXEC)

dep:
	cd ../model_jxz_tlm; $(MAKE) -f makefile.gnu all-am

$(EXEC): $(OBJECTS) $(STATICLIB)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(EXEC)
	@echo "-- Build model_jxz_tlm_tb.exe completed --"	
	@echo ""

relink-am:
	rm -f $(EXEC)
	$(MAKE) -f makefile.gnu all-am
		
clean:
	cd ../model_jxz_tlm; $(MAKE) -f makefile.gnu clean
	rm -f $(OBJECTS) $(EXEC)
	@echo "-- Clean model_jxz_tlm_tb completed --"
	@echo ""
	
####### Compile dependencies

$(OBJECTS): $(HEADERS)


