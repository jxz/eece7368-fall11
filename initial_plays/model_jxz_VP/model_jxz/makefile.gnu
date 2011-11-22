
###############################################################################
### VP Component Makefile for model_jxz
###############################################################################

####### Files

HEADERS =   include/model_jxz.h include/model_jxz_private.h include/model_jxz_types.h include/rtwtypes.h 

SOURCES =	src/model_jxz.cpp 

OBJECTS =	obj/model_jxz.o 

STATICLIB	=	lib/libmodel_jxz.a

####### Tools

AS		=	as
CC		=	gcc
CXX		=	gcc
LD		=	gcc
AR		=	ar
RANLIB	=	ranlib
MAKE    = make

####### Inc path


####### Include, Lib

INCDIR		= -I ./include -I ./utils

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
	
all-am: $(STATICLIB)
	
$(STATICLIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(STATICLIB) $(OBJECTS)
	$(RANLIB) $(STATICLIB) 
	@echo "-- Build libmodel_jxz.a completed --"	
	@echo ""
			
clean:
	rm -f $(OBJECTS) $(STATICLIB)
	@echo "-- Clean model_jxz completed --"
	@echo ""
	
####### Compile dependencies

$(OBJECTS): $(HEADERS)


