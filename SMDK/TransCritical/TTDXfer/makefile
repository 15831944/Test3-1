#   Time-stamp: <2007-05-24 06:41:03 Rod Stephenson Transcritical Pty Ltd>
SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin

COMMON = ../TTCommon/

SMDKDLL = TTDynTest

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" \
          /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG  $(COMMON)ttcommon.lib
LIBS = SMDK1.lib scdlib.lib

OBJS = 	TTDynTest.obj   \
	TTDX.obj


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
	link /OUT:"c:\SysCAD$(SYSCAD)\Bin\$(SMDKDLL).dll" $(LFLAGS) /PDB:".\Rls/$(SMDKDLL).pdb" /IMPLIB:".\Rls/$(SMDKDLL).lib" /MACHINE:X86 /LIBPATH:$(LDIR) $(OBJS) $(LIBS)
	../../smdk/devlic.exe /q /f:"c:\syscad$(SYSCAD)\bin\$(SMDKDLL).dll"


TTDX.obj : TTDX.cpp TTDX.h
TTDynTest.obj : TTDynTest.cpp TTDynTest.h


clean:
	rm -f $(OBJS) vc70.pdb vc70.idb



doc:
	/cygwin/bin/a2ps -Ma4 QALHeater.cpp -oout.ps


install: 
	cp "C:\SysCAD$(SYSCAD)\Bin\$(SMDKDLL).dll" "G:\archive\syscad\SysCAD$(SYSCAD)"
