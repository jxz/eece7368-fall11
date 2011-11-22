
###############################################################################
### VP Component Makefile for model_jxz_tlm
###############################################################################

####### Files

HEADERS =   include\model_jxz_tlm.h include\model_jxz_tlm_def.h

SOURCES =	src\model_jxz_tlm.cpp

OBJECTS =	obj\model_jxz_tlm.obj

STATICLIB	=	lib\model_jxz_tlm.lib

####### Tools

AS		=	ml.exe
CC		=	cl.exe
CXX		=	cl.exe
LD		=	link.exe
AR		=	lib.exe
MAKE  = nmake.exe

####### Inc path

SYSTEMC_INCPATH =	"$(SYSTEMC_INC_PATH)"
TLM_INCPATH     =	"$(TLM_INC_PATH)"

####### Include, Lib

INCDIR		=  /I ".\include" /I "..\model_jxz\include" /I "..\model_jxz\utils" /I $(SYSTEMC_INCPATH) /I $(TLM_INCPATH)

SC_OPTION	= /D SC_INCLUDE_DYNAMIC_PROCESSES

####### Flags

ASFLAGS		=
CFLAGS		=	$(OPT_CXXFLAGS) /Fd".\obj\model_jxz_tlm.pdb" $(EXTRA_CXXFLAGS) /D "WIN32" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /D "UNICODE" $(INCDIR) $(SC_OPTION) /nologo /FD /EHsc /W3 /TP /wd4244 /wd4267 /vmg
CXXFLAGS	=	$(OPT_CXXFLAGS) /Fd".\obj\model_jxz_tlm.pdb" $(EXTRA_CXXFLAGS) /D "WIN32" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /D "UNICODE" $(INCDIR) $(SC_OPTION) /nologo /FD /EHsc /W3 /TP /wd4244 /wd4267 /vmg
LDFLAGS		=	
ARFLAGS 	= /nologo /subsystem:console

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c .asm

{src\}.asm{obj\}.obj:
	$(AS) /c $(ASFLAGS) /Fo$@ $<

{src\}.cpp{obj\}.obj:
	$(CXX) /c $(CXXFLAGS) /Fo$@ $<

{src\}.cxx{obj\}.obj:
	$(CXX) /c $(CXXFLAGS) /Fo$@ $<

{src\}.cc{obj\}.obj:
	$(CXX) /c $(CXXFLAGS) /Fo$@ $<

{src\}.C{obj\}.obj:
	$(CXX) /c $(CXXFLAGS) /Fo$@ $<

{src\}.c{obj\}.obj:
	$(CC) /c $(CFLAGS) /Fo$@ $<

####### Targets
all:
	$(MAKE) /nologo /f makefile.mk all-am OPT_CXXFLAGS="/O2 /MT /D _NDEBUG"

all-debug:
	$(MAKE) /nologo /f makefile.mk all-am OPT_CXXFLAGS="/Od /RTC1 /Zi /MTd /D _DEBUG"

all-am: dep $(STATICLIB)
	
dep:
	cd ..\model_jxz &&	$(MAKE) /nologo /f makefile.mk all-am

$(STATICLIB): $(OBJECTS)
	$(AR) $(ARFLAGS) /out:$(STATICLIB) $(OBJECTS)
	@echo "-- Build model_jxz_tlm.lib completed --"	
	@echo ""
			
clean:
	cd ..\model_jxz && $(MAKE) /nologo /f makefile.mk clean
	del /f $(OBJECTS) obj\model_jxz_tlm.idb obj\model_jxz_tlm.pdb $(STATICLIB) 
	@echo "-- Clean model_jxz_tlm completed --"
	@echo ""
	
####### Compile dependencies

$(OBJECTS): $(HEADERS)


