#   Generic makefile for compiling SMDK using GNU tools and free MS Compilers
#   Time-stamp: <2007-06-28 23:39:30 Rod Stephenson Transcritical Pty Ltd>
SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin

COMMON = ../TTCommon/

SMDKDLL = TTDynTest

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /Zi /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" \
          /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"
LFLAGS =  /NOLOGO /DLL /DEBUG   $(COMMON)ttcommon.lib
LIBS = SMDK1.lib scdlib.lib

OBJS = 	TTDynTest.obj   \
	TTDynTestUM.obj


ifeq ($(SYSCAD),91)
 IDIR = "..\..\smdk\Include"
 LDIR = "..\..\smdk\bin\rls"
else
 IDIR = "..\..\include"
 LDIR = "..\..\bin\rls"
endif


%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<




TTDynTest.dll: $(OBJS)
	link /OUT:"c:\SysCAD$(SYSCAD)\Bin\$(SMDKDLL).dll" $(LFLAGS) /PDB:".\Rls/$(SMDKDLL).pdb" \
	/IMPLIB:".\Rls/$(SMDKDLL).lib" /MACHINE:X86 /LIBPATH:$(LDIR) $(OBJS) $(LIBS)
	../../smdk/devlic.exe /q /f:"c:\syscad$(SYSCAD)\bin\$(SMDKDLL).dll"


TTDynTestUM.obj : TTDynTestUM.cpp TTDynTestUM.h
TTDynTest.obj : TTDynTest.cpp TTDynTest.h
$(COMMON)/ttcommon.lib: $(COMMON)/PSD.cpp $(COMMON)/PSD.h
	make $(COMMON)/.makefile


clean:
	rm -f $(OBJS) vc70.pdb vc70.idb


list:	
	a2ps -Ma4 -olist.ps $(SMDKDLL)UM.h $(SMDKDLL)UM.cpp
