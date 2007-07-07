all:	lib exceldll


# Standard __cdecl DLL
dll:
	g77 -c  -o bayer.o bayer.for
	g77 -c  -o version.o version.for
	dllwrap -s --export-all --output-def amira.def  --driver-name g77 -o MurdochBayer.dll bayer.o version.o
	cp MurdochBayer.dll "c:/windows/system32"
	cp MurdochBayer.dll "c:/devel/python"



# Export Library with _cdecl DLL
lib:
	g77 -shared -o MurdochBayer.dll bayer.for version.for -Wl,--output-def,MurdochBayer.def,--out-implib,libMurdochBayer.a
	lib /machine:i386 /def:MurdochBayer.def
	cp MurdochBayer.dll "c:/syscad91/bin"      # for syscad
	cp MurdochBayer.dll "c:/windows/system32"  #for python etc
	cp MurdochBayer.dll ./demo
	cp MurdochBayer.dll "C:/Documents and Settings/Rodney Stephenson/Desktop/MBRelease"
	cp MurdochBayer.lib ../TTAmira


# __stdcall DLL for use with Excel...
exceldll:
	g77 -shared -mrtd -o MurdochBayerProps.dll bayer.for
	cp MurdochBayerProps.dll "c:/program files/microsoft office/office"
	cp MurdochBayerProps.dll "c:/windows/system32"
	cp MurdochBayerProps.dll "C:/Documents and Settings/Rodney Stephenson/Desktop/MBRelease"
	cp dlltest.xls 	"C:/Documents and Settings/Rodney Stephenson/Desktop/MBRelease"


example:
	g77 -shared -mrtd -o mcalc.dll mcalc.for
