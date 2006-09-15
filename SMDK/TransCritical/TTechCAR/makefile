CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
PRECIP_OBJ = precip.obj
OBJS = TTechCAR.obj CARTubeDigester.obj $(PRECIP_OBJ)  TTCARPrecip.obj sthx.obj
COMMON = ../TTCommon/


%.obj : %.cpp
	 cl /I "..\Include" $(DFLAGS) $(CFLAGS) $<



TTechCAR.dll: $(OBJS)
	link /OUT:"..\..\SysCAD90\Bin\TTechCAR.dll" $(LFLAGS) /PDB:".\Rls/TTechCAR.pdb" /IMPLIB:".\Rls/TTechCAR.lib" /MACHINE:X86 $(OBJS)
	lic.bat

CARTubeDigester.obj : CARTubeDigester.cpp CARTubeDigester.h

TTechCAR.obj : TTechCAR.cpp TTechCAR.h

TTCARPrecip.obj : TTCARPrecip.cpp TTCARPrecip.h $(COMMON)precip.h



sthx.obj : $(COMMON)STHx.cpp $(COMMON)STHx.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) $(COMMON)sthx.cpp

precip.obj : $(COMMON)precip.cpp $(COMMON)precip.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) $(COMMON)precip.cpp




list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps precip.h precip.cpp TTCARPrecip.cpp TTCARPrecip.h


list2 :  
	/cygwin/bin/a2ps -Ma4 -olist.ps cartubedigester.cpp


list1: 
	/cygwin/bin/a2ps -Ma4 -olist.ps temp.cpp




clean:
	rm -f TTechCAR.obj
	rm -f CARTubeDigester.obj
	rm -f CARPrecip.obj
	rm -f hxutils.obj
	rm -f precip.obj
	rm -f STHx.obj
	rm -f TTCARPrecip.obj
	rm -f vc70.idb
	rm -f vc70.pdb
