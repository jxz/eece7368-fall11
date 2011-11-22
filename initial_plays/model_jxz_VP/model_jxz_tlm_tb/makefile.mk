
###############################################################################
### VP Component Makefile for model_jxz_tlm_tb
###############################################################################

####### Files

HEADERS =	include\mw_support_tb.h include\model_jxz_tlm_tb_def.h include\model_jxz_tlm_tb.h
		
SOURCES =	src\mw_support_tb.cpp src\model_jxz_tlm_tb.cpp src\model_jxz_tlm_tb_main.cpp
		
OBJECTS =	obj\mw_support_tb.obj obj\model_jxz_tlm_tb.obj obj\model_jxz_tlm_tb_main.obj

STATICLIB 	=	..\model_jxz_tlm\lib\model_jxz_tlm.lib ..\model_jxz\lib\model_jxz.lib

EXEC		=	model_jxz_tlm_tb.exe

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
MATLAB_INCPATH  =   "/export/scratch/opt/extern/include"

####### Lib path

SYSTEMC_LIBPATH =	"$(SYSTEMC_LIB_PATH)"
MATLAB_LIBPATH =   "/export/scratch/opt/extern/lib/glnx86/microsoft"
MATLAB_LIBPATH2 = 

SYSTEMC_LIBNAME = "$(SYSTEMC_LIB_NAME)"

####### Include, Lib

INCDIR		= /I ".\include" /I "..\model_jxz_tlm\include" /I "..\model_jxz\include" /I "..\model_jxz\utils" /I $(SYSTEMC_INCPATH) /I $(TLM_INCPATH) /I $(MATLAB_INCPATH)
LIBDIR		= /LIBPATH:$(SYSTEMC_LIBPATH) /LIBPATH:$(MATLAB_LIBPATH)
LIBS		= $(STATICLIB) $(SYSTEMC_LIBNAME) libmat.lib libmx.lib libeng.lib 

SC_OPTION	= /D SC_INCLUDE_DYNAMIC_PROCESSES

LOG_OPTION  =   /D TLMG_CURR_VERBOSITY=tlmgPrintTerse

####### Flags

ASFLAGS		=
CFLAGS		=	$(OPT_CXXFLAGS) /Fd".\obj\model_jxz_tlm_tb.pdb" $(EXTRA_CXXFLAGS) /D "WIN32" /D "_CONSOLE" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /D "UNICODE" $(INCDIR) $(SC_OPTION) $(LOG_OPTION) /nologo /FD /EHsc /W3 /TP /wd4244 /wd4267 /vmg 
CXXFLAGS	=	$(OPT_CXXFLAGS) /Fd".\obj\model_jxz_tlm_tb.pdb" $(EXTRA_CXXFLAGS) /D "WIN32" /D "_CONSOLE" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /D "UNICODE" $(INCDIR) $(SC_OPTION) $(LOG_OPTION) /nologo /FD /EHsc /W3 /TP /wd4244 /wd4267 /vmg
LDFLAGS		=	$(LIBDIR) /nologo /INCREMENTAL $(OPT_LDFLAGS) /SUBSYSTEM:CONSOLE /MACHINE:X86 $(LIBS) /PDB:".\obj\model_jxz_tlm_tb.pdb"
ARFLAGS 	=	/nologo /subsystem:console

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
	$(MAKE) /nologo /f makefile.mk all-am OPT_CXXFLAGS="/O2 /MT /D _NDEBUG" OPT_LDFLAGS=""

all-debug:
	$(MAKE) /nologo /f makefile.mk all-am OPT_CXXFLAGS="/Od /RTC1 /Zi /MTd /D _DEBUG" OPT_LDFLAGS="/DEBUG"

relink:
	$(MAKE) /nologo /f makefile.mk relink-am OPT_CXXFLAGS="/O2 /MT /D _NDEBUG" OPT_LDFLAGS=""

relink-debug:
	$(MAKE) /nologo /f makefile.mk relink-am OPT_CXXFLAGS="/Od /RTC1 /Zi /MTd /D _DEBUG" OPT_LDFLAGS="/DEBUG"

all-am: dep $(EXEC)

dep:
	cd ..\model_jxz_tlm && $(MAKE) /nologo /f makefile.mk all-am
  
$(EXEC): $(OBJECTS) $(STATICLIB)
	$(LD) $(OBJECTS) $(LDFLAGS) /out:$(EXEC)
	@echo "-- Build model_jxz_tlm_tb.exe completed --"	
	@echo ""

relink-am:
	del /f $(EXEC) model_jxz_tlm_tb.ilk
	$(MAKE) /f makefile.mk all-am
		
clean:
	cd ..\model_jxz_tlm && $(MAKE) /nologo /f makefile.mk clean
	del /f $(OBJECTS) obj\model_jxz_tlm_tb.idb obj\model_jxz_tlm_tb.pdb $(EXEC) model_jxz_tlm_tb.ilk 
	@echo "-- Clean model_jxz_tlm_tb completed --"
	@echo ""
	
####### Compile dependencies

$(OBJECTS): $(HEADERS)

