CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "TTECH"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
OBJS = TTQALBayer.obj   TTQALBayerSM.obj


TTQALBayer.dll: $(OBJS)
	link /OUT:"C:\SysCAD90\Bin\TTQALBayer.dll" $(LFLAGS) /PDB:".\Rls/TTQALBayer.pdb" /IMPLIB:".\Rls/TTQALBayer.lib" /MACHINE:X86 $(OBJS)
	lic.bat

TTQALBayerSM.obj : TTQALBayerSM.cpp TTQALBayerSM.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) TTQALBayerSM.cpp

TTQALBayer.obj : TTQALBayer.cpp TTQALBayer.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) TTQALBayer.cpp



list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps TTQALBayerSM.h TTQALBayerSM.cpp \
	    TTQALBayer.h TTQALBayer.cpp





clean:
	rm -f $(OBJS)
