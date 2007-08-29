DLLNAME = TTGas
SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin125

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /ZI /wd4800
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "TTECH"
LFLAGS = /INCREMENTAL /NOLOGO /DLL /DEBUG
OBJS = TTGas.obj   TTGasSM.obj TTCompress.obj TTMSCompress.obj
LIBS = DevLib.lib scdlib.lib

IDIR = "..\..\Include"
LDIR = "..\..\bin\rls"

%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<


TTGas.dll: $(OBJS)
	link /OUT:"$(SYSCADDIR)\$(DLLNAME).dll" $(LFLAGS) /PDB:".\Rls/$(DLLNAME).pdb" \
	/IMPLIB:".\Rls/$(DLLNAME).lib" /LIBPATH:$(LDIR) /MACHINE:X86 $(OBJS) $(LIBS)
	../../devlic.exe /q /f:"$(SYSCADDIR)\$(DLLNAME).dll"

TTGasSM.obj : TTGasSM.cpp TTGasSM.h

TTGas.obj : TTGas.cpp TTGas.h

TTCompress.obj : TTCompress.cpp TTCompress.h

TTMSCompress.obj : TTMSCompress.cpp TTMSCompress.h


list :  
	/cygwin/bin/a2ps -Ma4 -olist.ps *cpp *h





clean:
	rm -f $(OBJS)
