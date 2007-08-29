#   Time-stamp: <2007-08-28 14:22:24 Rod Stephenson Transcritical Pty Ltd>
SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin125

#DBG = 1
COMMON = ../TTCommon/

SMDKDLL = TTechQAL

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800 /I C:/Tcl/Include
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" \
          /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG  $(COMMON)ttcommon.lib
LIBS = DevLib.lib scdlib.lib

OBJS = 	QALClassifier.obj   \
	ReversionWasher.obj \
	QALHeater.obj \
	TTechQAL.obj \
	QALHeaterU.obj \
	QALPump.obj \
	QALPipe.obj \
	QALCyclone.obj \
	QALPrecip.obj \
	QALPSD.obj \
	QALDynPrecip.obj
#	precip.obj \
#	hxutils.obj \
#	STHx.obj \

ifdef DBG
LFLAGS := $(LFLAGS)   c:/Tcl/lib/Tcl84.lib  c:/Tcl/lib/Tk84.lib
DFLAGS := $(DFLAGS) /D "TTDEBUG"
endif

IDIR = "..\..\include"
LDIR = "..\..\bin\rls"


%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<




TTechQAL.dll: $(OBJS)
	link /OUT:"c:\SysCAD$(SYSCAD)\Bin\$(SMDKDLL).dll" $(LFLAGS) /PDB:".\Rls/TTechQAL.pdb" /IMPLIB:".\Rls/TTechQAL.lib" /MACHINE:X86 /LIBPATH:$(LDIR) $(OBJS) $(LIBS)
	../../devlic.exe /q /f:"c:\syscad$(SYSCAD)\bin125\$(SMDKDLL).dll"
#	cp "$(SYSCADDIR)/$(SMDKDLL).dll" "g:/archive/syscad/syscad$(SYSCAD)"

QALClassifier.obj : QALClassifier.cpp QALClassifier.h
QALCyclone.obj : QALCyclone.cpp QALCyclone.h
ReversionWasher.obj : ReversionWasher.cpp ReversionWasher.h
QALHeater.obj : QALHeater.cpp QALHeater.h
QALHeaterU.obj : QALHeaterU.cpp QALHeater.h
QALPump.obj : QALPump.cpp QALPump.h
QALPipe.obj : QALPipe.cpp QALPipe.h
QALPrecip.obj : QALPrecip.cpp QALPrecip.h
QALPSD.obj: QALPSD.cpp QALPSD.h
QALDynPrecip.obj : QALDynPrecip.cpp QALDynPrecip.h


TTechQAL.obj : TTechQAL.cpp TTechQAL.h


clean:
	rm -f $(OBJS) vc70.pdb vc70.idb



doc:
	/cygwin/bin/a2ps -Ma4 QALHeater.cpp -oout.ps


install: 
	cp "C:\SysCAD$(SYSCAD)\Bin\$(SMDKDLL).dll" "G:\archive\syscad\SysCAD$(SYSCAD)"
