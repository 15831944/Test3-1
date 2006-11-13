#DBG = 1
SYSCAD=90
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800 /I C:/Tcl/Include
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"   /D SYSCAD$(SYSCAD)
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
PRECIP_OBJ = Precip.obj
OBJS = TTechWOR.obj TTWORClassifier.obj  TTWORPrecip.obj precip.obj brahmapsd.obj \
	TTWORPSDUnit.obj TTWORFilter.obj TTWORClassPrecip.obj
LIBS = SMDK1.lib scdlib.lib



COMMON = ../TTCommon/

ifdef DBG
LFLAGS := $(LFLAGS)   $(COMMON)ttcommon.lib c:/Tcl/lib/Tcl84.lib  c:/Tcl/lib/Tk84.lib
DFLAGS := $(DFLAGS) /D "TTDEBUG"
endif



ifeq ($(SYSCAD),91)
 IDIR = "..\..\smdk\Include"
 LDIR = "..\..\smdk\bin\rls"
else
 IDIR = "..\..\include"
 LDIR = "..\..\bin\rls"
endif




%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<


BATC_Brahma.dll: $(OBJS)
	link /OUT:"$(SYSCADDIR)\BATC_Brahma.dll" $(LFLAGS) /PDB:".\Rls/BATC_Brahma.pdb" \
	/IMPLIB:".\Rls/TTechWOR.lib" /LIBPATH:$(LDIR) /MACHINE:X86 $(OBJS) $(LIBS)
	../../smdk/devlic.exe /q /f:"$(SYSCADDIR)\BATC_Brahma.dll"




TTechWOR.obj : TTechWOR.cpp TTechWOR.h
TTWORPrecip.obj : TTWORPrecip.cpp TTWORPrecip.h wordata.cpp worcalc.cpp $(COMMON)precip.h
TTWORClassPrecip.obj : TTWORClassPrecip.cpp TTWORClassPrecip.h wordata.cpp worcalc.cpp $(COMMON)precip.h
TTWORClassifier.obj: TTWORClassifier.cpp TTWORClassifier.h
TTWORFilter.obj: TTWORFilter.cpp TTWORFilter.h
TTWORPSDUnit.obj: TTWORPSDUnit.cpp TTWORPSDUnit.h
client.obj: $(COMMON)client.cpp

precip.obj : $(COMMON)precip.cpp $(COMMON)precip.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $(COMMON)precip.cpp

brahmapsd.obj: brahmapsd.h brahmapsd.cpp


list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps precip.h precip.cpp TTPrecip.cpp TTPrecip.h worcalc.cpp

list1: 
	/cygwin/bin/a2ps -Ma4 -olist.ps bla.cpp


clean:
	rm -f $(OBJS)
	rm -f vc70.idb
	rm -f vc70.pdb
