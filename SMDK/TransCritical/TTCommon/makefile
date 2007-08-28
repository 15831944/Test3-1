CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800

OBJS =  sthx.obj hxutils.obj precip.obj utils.obj psd.obj

LFLAGS = /INCREMENTAL /NOLOGO /DEBUG user32.lib

DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" \
          /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"


all:  ttcommon.lib

client.exe: client.cpp
	cl client.cpp wsock32.lib

ttdbg.obj: ttdbg.cpp ttdbg.h
	cl  $(CFLAGS) ttdbg.cpp 

dbgtest.obj: dbgtest.cpp
	cl $(CFLAGS) dbgtest.cpp ttdbg.obj


sthx.obj : sthx.cpp sthx.h
	cl $(CFLAGS) sthx.cpp

hxutils.obj : hxutils.cpp hxutils.h
	cl $(CFLAGS) hxutils.cpp

utils.obj : utils.cpp utils.h
	cl $(CFLAGS) utils.cpp

precip.obj : precip.cpp precip.h
	cl $(CFLAGS) precip.cpp

psd.obj    : psd.cpp psd.h
	cl $(CFLAGS) $(DFLAGS) /I "..\..\Include" psd.cpp


dbgtest.exe: dbgtest.obj ttdbg.obj
	link $(LFLAGS) dbgtest.obj ttdbg.obj 


ttcommon.lib: $(OBJS)
	lib /out:ttcommon.lib $(OBJS)


test: test.cpp
	cl test.cpp ttcommon.lib


clean:
	rm -f $(OBJS)


