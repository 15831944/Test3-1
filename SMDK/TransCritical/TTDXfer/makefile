#   Generic makefile for compiling SMDK using GNU tools and free MS Compilers
#   Time-stamp: <2007-05-27 18:06:46 Rod Stephenson Transcritical Pty Ltd>
SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin

COMMON = ../TTCommon/

SMDKDLL = TTDynTest

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /Zi /wd4800 /YX"stdafx.h"
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" \
          /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"
LFLAGS = /INCREMENTAL:NO /NOLOGO /DLL /DEBUG  $(COMMON)ttcommon.lib
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


