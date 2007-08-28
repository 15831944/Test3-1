SYSCAD=91
SYSCADDIR=C:\SysCAD$(SYSCAD)\bin
DLLNAME = MurdochBayerSpMdl


CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY" /D DLL_GroupName=\"$(DLLNAME)\"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG

PRECIP_OBJ = precip.obj

LIBS = DevLib.lib scdlib.lib
#LIBS = SMDK1.lib scdlib.lib


OBJS = $(DLLNAME).obj   $(DLLNAME)SM.obj 


IDIR = "..\..\include"
LDIR = "..\..\bin\rls"




%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<


all: $(DLLNAME).dll


$(DLLNAME).dll: $(OBJS)
	link /OUT:"$(SYSCADDIR)\$(DLLNAME).dll" $(LFLAGS) /PDB:".\Rls/$(DLLNAME).pdb" /IMPLIB:".\Rls/$(DLLNAME).lib" /MACHINE:X86 /LIBPATH:$(LDIR) $(OBJS) $(LIBS) MurdochBayer.lib
	../../devlic.exe /q /f:"$(SYSCADDIR)\$(DLLNAME).dll"
	cp c:/SysCAD91/bin/$(DLLNAME).dll "C:/Documents and Settings/Rodney Stephenson/Desktop/MBRelease"



$(DLLNAME)SM.obj : $(DLLNAME)SM.cpp $(DLLNAME)SM.h
	cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $(DLLNAME)SM.cpp

$(DLLNAME).obj : $(DLLNAME).cpp $(DLLNAME).h






clean:
	rm -f $(OBJS)

list :  
	a2ps -Ma4 -olist.ps $(DLLNAME)SM.h $(DLLNAME)SM.cpp

