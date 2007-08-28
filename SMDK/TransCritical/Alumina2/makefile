SYSCAD=91
SYSCADDIR=C:\SysCAD$(SYSCAD)\bin

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG

PRECIP_OBJ = precip.obj

LIBS = DevLib.lib scdlib.lib


OBJS = Alumina2.obj   Alumina2SM.obj 
UOBJS =  Precipitator.obj TubeDigester.obj $(PRECIP_OBJ) sthx.obj
COMMON = ../TTCommon/


IDIR = "..\..\includeex"
LDIR = "..\..\binex\rls"




%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<


all: Alumina2.dll


Alumina2.dll: $(OBJS) $(UOBJS)
	link /OUT:"$(SYSCADDIR)\Alumina2.dll" $(LFLAGS) /PDB:".\Rls/Alumina2.pdb" /IMPLIB:".\Rls/Alumina2.lib" /MACHINE:X86 /LIBPATH:$(LDIR) $(OBJS) $(UOBJS) $(LIBS)
	../../smdk/devlic.exe /q /f:"$(SYSCADDIR)\Alumina2.dll"



Alumina2SM.obj : Alumina2SM.cpp Alumina2SM.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) Alumina2SM.cpp

Alumina2.obj : Alumina2.cpp Alumina2.h




precip.obj : $(COMMON)precip.cpp $(COMMON)precip.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $(COMMON)precip.cpp

sthx.obj : $(COMMON)STHx.cpp $(COMMON)STHx.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $(COMMON)sthx.cpp



TubeDigester.obj : TubeDigester.cpp TubeDigester.h


Precipitator.obj : Precipitator.cpp Precipitator.h $(COMMON)precip.h


clean:
	rm -f $(OBJS) $(UOBJS)

list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps Alumina2SM.h Alumina2SM.cpp \
	    Alumina2.h Alumina2.cpp

install: 
	cp "C:\SysCAD90\Bin\Alumina2.dll" "\\TT_3\share\Alumina2.dll"
	cp "C:\SysCAD90\Bin\Alumina2.dll" "G:\archive\syscad"
