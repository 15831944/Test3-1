CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG

PRECIP_OBJ = precip.obj


OBJS = TTMADBayer.obj   TTMADBayerSM.obj 
UOBJS =  Precipitator.obj TubeDigester.obj $(PRECIP_OBJ) sthx.obj
COMMON = ../TTCommon/



%.obj : %.cpp
	 cl /I "..\Include" $(DFLAGS) $(CFLAGS) $<


all: Alumina2.dll


Alumina2.dll: $(OBJS) $(UOBJS)
#	link /OUT:"C:\SysCAD90\Bin\Alumina2.dll" $(LFLAGS) /PDB:".\Rls/TTMADBayer.pdb" /IMPLIB:".\Rls/TTMADBayer.lib" /MACHINE:X86 $(OBJS) 
	link /OUT:"C:\SysCAD90\Bin\Alumina2.dll" $(LFLAGS) /PDB:".\Rls/TTMADBayer.pdb" /IMPLIB:".\Rls/TTMADBayer.lib" /MACHINE:X86 $(OBJS) $(UOBJS)
	lic.bat



TTMADBayerSM.obj : TTMADBayerSM.cpp TTMADBayerSM.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) TTMADBayerSM.cpp

TTMADBayer.obj : TTMADBayer.cpp TTMADBayer.h




precip.obj : $(COMMON)precip.cpp $(COMMON)precip.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) $(COMMON)precip.cpp

sthx.obj : $(COMMON)STHx.cpp $(COMMON)STHx.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) $(COMMON)sthx.cpp



TubeDigester.obj : TubeDigester.cpp TubeDigester.h


Precipitator.obj : Precipitator.cpp Precipitator.h $(COMMON)precip.h


clean:
	rm -f $(OBJS) $(UOBJS)

list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps TTMADBayerSM.h TTMADBayerSM.cpp \
	    TTMADBayer.h TTMADBayer.cpp

install: 
	cp "C:\SysCAD90\Bin\Alumina2.dll" "\\TT_3\share\Alumina2.dll"
	cp "C:\SysCAD90\Bin\Alumina2.dll" "G:\archive\syscad"
