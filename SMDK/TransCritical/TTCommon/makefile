CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800 /I C:/Tcl/Include

OBJS = ttdbg.obj sthx.obj hxutils.obj precip.obj utils.obj

LFLAGS = /INCREMENTAL /NOLOGO /DEBUG  c:/Tcl/lib/Tcl84.lib  c:/Tcl/lib/Tk84.lib user32.lib


all: ttdbg.obj dbgtest.exe ttcommon.lib

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



dbgtest.exe: dbgtest.obj ttdbg.obj
	link $(LFLAGS) dbgtest.obj ttdbg.obj 


ttcommon.lib: $(OBJS)
	lib /out:ttcommon.lib $(OBJS)


test: test.cpp
	cl test.cpp ttcommon.lib


clean:
	rm -f $(OBJS)


