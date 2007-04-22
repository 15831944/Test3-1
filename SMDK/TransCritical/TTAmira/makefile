SYSCAD=91
SYSCADDIR=C:\SysCAD$(SYSCAD)\bin

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG

PRECIP_OBJ = precip.obj

LIBS = SMDK1.lib scdlib.lib


OBJS = TTAmira.obj   TTAmiraSM.obj 


ifeq ($(SYSCAD),91)
 IDIR = "..\..\smdk\Include"
 LDIR = "..\..\smdk\bin\rls"
else
 IDIR = "..\..\include"
 LDIR = "..\..\bin\rls"
endif




%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<


all: TTAmira.dll


TTAmira.dll: $(OBJS)
	link /OUT:"$(SYSCADDIR)\TTAmira.dll" $(LFLAGS) /PDB:".\Rls/TTAmira.pdb" /IMPLIB:".\Rls/TTAmira.lib" /MACHINE:X86 /LIBPATH:$(LDIR) $(OBJS) $(LIBS)
	../../smdk/devlic.exe /q /f:"$(SYSCADDIR)\TTAmira.dll"



TTAmiraSM.obj : TTAmiraSM.cpp TTAmiraSM.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) TTAmiraSM.cpp

TTAmira.obj : TTAmira.cpp TTAmira.h






clean:
	rm -f $(OBJS)

list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps TTAmiraSM.h TTAmiraSM.cpp \
	    TTAmira.h TTAmira.cpp

install: 
	cp "C:\SysCAD90\Bin\Alumina2.dll" "\\TT_3\share\Alumina2.dll"
	cp "C:\SysCAD90\Bin\Alumina2.dll" "G:\archive\syscad"
