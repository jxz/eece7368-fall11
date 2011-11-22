
###############################################################################
### VP Component Makefile for model_jxz
###############################################################################

####### Files

HEADERS =   include\model_jxz.h include\model_jxz_private.h include\model_jxz_types.h include\rtwtypes.h 

SOURCES =	src\model_jxz.cpp 

OBJECTS =	obj\model_jxz.obj 

STATICLIB	=	lib\model_jxz.lib

####### Tools

AS		=	ml.exe
CC		=	cl.exe
CXX		=	cl.exe
LD		=	link.exe
AR		=	lib.exe
MAKE  = nmake.exe

####### Inc path


####### Include, Lib

INCDIR		=  /I ".\include" /I ".\utils"

####### Flags

ASFLAGS		=
CFLAGS		=	$(OPT_CXXFLAGS) /Fd".\obj\model_jxz.pdb" $(EXTRA_CXXFLAGS) /D "WIN32" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /D "UNICODE" $(INCDIR) $(SC_OPTION) /nologo /FD /EHsc /W3 /TP /wd4244 /wd4267 /vmg
CXXFLAGS	=	$(OPT_CXXFLAGS) /Fd".\obj\model_jxz.pdb" $(EXTRA_CXXFLAGS) /D "WIN32" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /D "UNICODE" $(INCDIR) $(SC_OPTION) /nologo /FD /EHsc /W3 /TP /wd4244 /wd4267 /vmg
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
	
all-am: $(STATICLIB)
	
$(STATICLIB): $(OBJECTS)
	$(AR) $(ARFLAGS) /out:$(STATICLIB) $(OBJECTS)
	@echo "-- Build model_jxz.lib completed --"	
	@echo ""
			
clean:
	del /f $(OBJECTS) obj\model_jxz.idb obj\model_jxz.pdb $(STATICLIB)
	@echo "-- Clean model_jxz completed --"
	@echo ""
	
####### Compile dependencies

$(OBJECTS): $(HEADERS)


