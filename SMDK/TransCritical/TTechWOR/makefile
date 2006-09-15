#DBG = 1

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800 /I C:/Tcl/Include
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
PRECIP_OBJ = Precip.obj
OBJS = TTechWOR.obj TTWORClassifier.obj  TTWORPrecip.obj precip.obj brahmapsd.obj \
	TTWORPSDUnit.obj TTWORFilter.obj TTWORClassPrecip.obj

COMMON = ../TTCommon/

ifdef DBG
LFLAGS := $(LFLAGS)   $(COMMON)ttcommon.lib c:/Tcl/lib/Tcl84.lib  c:/Tcl/lib/Tk84.lib
DFLAGS := $(DFLAGS) /D "TTDEBUG"
endif




%.obj : %.cpp
	 cl /I "..\Include" $(DFLAGS) $(CFLAGS) $<


BATC_Brahma.dll: $(OBJS)
	link /OUT:"..\..\SysCAD90\Bin\BATC_Brahma.dll" $(LFLAGS) /PDB:".\Rls/BATC_Brahma.pdb" \
	/IMPLIB:".\Rls/TTechWOR.lib" /MACHINE:X86 $(OBJS)
	lic.bat
# cp "..\..\SysCAD90\bin\BATC_Brahma.dll" "..\..\SysCAD90\binw\BATC_Brahma.dll"

TTechWOR.obj : TTechWOR.cpp TTechWOR.h

TTWORPrecip.obj : TTWORPrecip.cpp TTWORPrecip.h wordata.cpp worcalc.cpp $(COMMON)precip.h

TTWORClassPrecip.obj : TTWORClassPrecip.cpp TTWORClassPrecip.h wordata.cpp worcalc.cpp $(COMMON)precip.h



TTWORClassifier.obj: TTWORClassifier.cpp TTWORClassifier.h

TTWORFilter.obj: TTWORFilter.cpp TTWORFilter.h

TTWORPSDUnit.obj: TTWORPSDUnit.cpp TTWORPSDUnit.h

client.obj: $(COMMON)client.cpp

precip.obj : $(COMMON)precip.cpp $(COMMON)precip.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) $(COMMON)precip.cpp

brahmapsd.obj: brahmapsd.h brahmapsd.cpp


list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps precip.h precip.cpp TTPrecip.cpp TTPrecip.h worcalc.cpp

list1: 
	/cygwin/bin/a2ps -Ma4 -olist.ps bla.cpp


clean:
	rm -f $(OBJS)
	rm -f vc70.idb
	rm -f vc70.pdb
