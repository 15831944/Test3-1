SYSCADDIR = C:\SysCAD90\bin

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "TTECH"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
OBJS = TTGas.obj   TTGasSM.obj TTCompress.obj


TTGas.dll: $(OBJS)
	link /OUT:"C:\SysCAD90\Bin\TTGas.dll" $(LFLAGS) /PDB:".\Rls/TTGas.pdb" /IMPLIB:".\Rls/TTGas.lib" /MACHINE:X86 $(OBJS)
	../smdk/devlic.exe /q /f:"$(SYSCADDIR)\TTGas.dll"

TTGasSM.obj : TTGasSM.cpp TTGasSM.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) TTGasSM.cpp

TTGas.obj : TTGas.cpp TTGas.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) TTGas.cpp

TTCompress.obj : TTCompress.cpp TTCompress.h
	cl /I "..\Include" $(DFLAGS) $(CFLAGS) TTCompress.cpp



list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps *cpp *h





clean:
	rm -f $(OBJS)
