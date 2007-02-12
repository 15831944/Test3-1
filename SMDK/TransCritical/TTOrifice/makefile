# Makefile for Dynamic Flash Train Models
DLLNAME = TTechFT
SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin



CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
PRECIP_OBJ = precip.obj
OBJS = TTFT.obj TTOrifice.obj
LIBS = SMDK1.lib scdlib.lib

ifeq ($(SYSCAD),91)
 IDIR = "..\..\smdk\Include"
 LDIR = "..\..\smdk\bin\rls"
else
 IDIR = "..\..\include"
 LDIR = "..\..\bin\rls"
endif





%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<



TTechCAR.dll: $(OBJS)
	link /OUT:"$(SYSCADDIR)\$(DLLNAME).dll" $(LFLAGS) /PDB:".\Rls/$(DLLNAME).pdb" \
	/IMPLIB:".\Rls/$(DLLNAME).lib" /LIBPATH:$(LDIR) /MACHINE:X86 $(OBJS) $(LIBS)
	../../smdk/devlic.exe /q /f:"$(SYSCADDIR)\$(DLLNAME).dll"

TTOrifice.obj : TTOrifice.cpp TTOrifice.h

TTFT.obj : TTFT.cpp TTFT.h










clean:
	rm -f $(OBJS) vc70.idb vc70.pdb
