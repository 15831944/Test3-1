DBG = 1
COMMON = ../TTCommon/

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800 /I C:/Tcl/Include
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" \
          /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "GLADDY"	
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG  $(COMMON)ttcommon.lib
OBJS = 	QALClassifier.obj   \
	ReversionWasher.obj \
	QALHeater.obj \
	TTechQAL.obj \
	QALHeaterU.obj \
	QALPump.obj \
	QALPipe.obj \
	QALCyclone.obj \
	QALPrecip.obj
#	precip.obj \
#	hxutils.obj \
#	STHx.obj \

ifdef DBG
LFLAGS := $(LFLAGS)   c:/Tcl/lib/Tcl84.lib  c:/Tcl/lib/Tk84.lib
DFLAGS := $(DFLAGS) /D "TTDEBUG"
endif



TTechQAL.dll: $(OBJS)
	link /OUT:"..\..\SysCAD90\Bin\TTechQAL.dll" $(LFLAGS) /PDB:".\Rls/TTechQAL.pdb" /IMPLIB:".\Rls/TTechQAL.lib" /MACHINE:X86 $(OBJS)
	lic.bat

QALClassifier.obj : QALClassifier.cpp QALClassifier.h
	cl /I "..\Include" $(DFLAGS)  $(CFLAGS) QALClassifier.cpp

QALCyclone.obj : QALCyclone.cpp QALCyclone.h
	cl /I "..\Include" $(DFLAGS)  $(CFLAGS) QALCyclone.cpp

ReversionWasher.obj : ReversionWasher.cpp ReversionWasher.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) ReversionWasher.cpp

QALHeater.obj : QALHeater.cpp QALHeater.h
	cl /I "..\Include" $(DFLAGS)  $(CFLAGS) QALHeater.cpp

QALHeaterU.obj : QALHeaterU.cpp QALHeater.h
	cl /I "..\Include" $(DFLAGS)  $(CFLAGS) QALHeaterU.cpp

QALPump.obj : QALPump.cpp QALPump.h
	cl /I "..\Include" $(DFLAGS)  $(CFLAGS) QALPump.cpp

QALPipe.obj : QALPipe.cpp QALPipe.h
	cl /I "..\Include" $(DFLAGS)  $(CFLAGS) QALPipe.cpp

QALPrecip.obj : QALPrecip.cpp QALPrecip.h qaldata.cpp
	cl /I "..\Include" $(DFLAGS)  $(CFLAGS) QALPrecip.cpp

TTechQAL.obj : TTechQAL.cpp TTechQAL.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) TTechQAL.cpp




clean:
	rm -f $(OBJS)


doc:
	/cygwin/bin/a2ps -Ma4 QALHeater.cpp -oout.ps