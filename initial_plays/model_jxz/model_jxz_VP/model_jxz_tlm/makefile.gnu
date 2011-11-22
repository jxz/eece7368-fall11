
###############################################################################
### VP Component Makefile for model_jxz_tlm
###############################################################################

####### Files

HEADERS =   include/model_jxz_tlm.h include/model_jxz_tlm_def.h

SOURCES =	src/model_jxz_tlm.cpp

OBJECTS =	obj/model_jxz_tlm.o

STATICLIB	=	lib/libmodel_jxz_tlm.a

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

####### Include, Lib

INCDIR		= -I ./include -I '../model_jxz/include' -I '../model_jxz/utils' -I $(SYSTEMC_INCPATH) -I $(TLM_INCPATH) 
SC_OPTION	= -D SC_INCLUDE_DYNAMIC_PROCESSES

####### Flags

ASFLAGS		=
CFLAGS		=	$(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS) -Wall $(INCDIR) $(SC_OPTION)
CXXFLAGS	=	$(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS) -Wall -Wno-deprecated $(INCDIR) $(SC_OPTION)
LDFLAGS		=	
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

all-am: dep $(STATICLIB)

dep:
	cd ../model_jxz; $(MAKE) -f makefile.gnu all-am

$(STATICLIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(STATICLIB) $(OBJECTS)
	$(RANLIB) $(STATICLIB) 
	@echo "-- Build libmodel_jxz_tlm.a completed --"	
	@echo ""
			
clean:
	cd ../model_jxz; $(MAKE) -f makefile.gnu clean
	rm -f $(OBJECTS) $(STATICLIB)
	@echo "-- Clean model_jxz_tlm completed --"
	@echo ""
	
####### Compile dependencies

$(OBJECTS): $(HEADERS)


