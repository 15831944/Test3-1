all:	lib


# Standard __cdecl DLL
dll:
	g77 -c  -o bayer.o bayer.for
	dllwrap -s --export-all --output-def amira.def  --driver-name g77 -o amira.dll bayer.o 
	cp amira.dll "c:/windows/system32"
	cp amira.dll "c:/devel/python"



# Export Library with _cdecl DLL
lib:
	g77 -shared -o amira.dll bayer.for -Wl,--output-def,amira.def,--out-implib,libamira.a
	lib /machine:i386 /def:amira.def
	cp amira.dll "c:/syscad91/bin"      # for syscad
	cp amira.dll "c:/windows/system32"  #for python etc
	cp amira.lib ../TTAmira


# __stdcall DLL for use with Excel...
exceldll:
	g77 -shared -mrtd -o amiraxl.dll bayer.for
	cp amiraxl.dll "c:/program files/microsoft office/office"
	cp amiraxl.dll "c:/windows/system32"
	cp amiraxl.dll "c:/devel/python"
