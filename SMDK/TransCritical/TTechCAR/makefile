SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin125
CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
PRECIP_OBJ = precip.obj
OBJS =  CARTubeDigester.obj $(PRECIP_OBJ)  TTCARPrecip.obj sthx.obj TTechCAR.obj
COMMON = ../TTCommon/
LIBS = DevLib.lib scdlib.lib


IDIR = "..\..\include"
LDIR = "..\..\bin\rls"



%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<



TTechCAR.dll: $(OBJS)
	link /OUT:"$(SYSCADDIR)\TTechCAR.dll" $(LFLAGS) /PDB:".\Rls/TTechCAR.pdb" /LIBPATH:$(LDIR) /IMPLIB:".\Rls/TTechCAR.lib" /MACHINE:X86 $(OBJS) $(LIBS)
	../../devlic.exe /q /f:"$(SYSCADDIR)\TTechCAR.dll"
#	# cp "$(SYSCADDIR)\TTechCAR.dll" "g:/archive/syscad/syscad$(SYSCAD)"

CARTubeDigester.obj : CARTubeDigester.cpp CARTubeDigester.h

TTechCAR.obj : TTechCAR.cpp TTechCAR.h

TTCARPrecip.obj : TTCARPrecip.cpp TTCARPrecip.h $(COMMON)precip.h



sthx.obj : $(COMMON)STHx.cpp $(COMMON)STHx.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $(COMMON)sthx.cpp

precip.obj : $(COMMON)precip.cpp $(COMMON)precip.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $(COMMON)precip.cpp





list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps precip.h precip.cpp TTCARPrecip.cpp TTCARPrecip.h


list2 :  
	/cygwin/bin/a2ps -Ma4 -olist.ps cartubedigester.cpp


list1: 
	/cygwin/bin/a2ps -Ma4 -olist.ps temp.cpp


install:
	cp "$(SYSCADDIR)\TTechCAR.dll" "g:\archive\syscad\syscad$(SYSCAD)\TTechCAR.dll"

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
