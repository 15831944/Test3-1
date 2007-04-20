all:	dll



dll:
	g77 -c  -o bayer.o bayer.for
	dllwrap -s --export-all --output-def amira.def  --driver-name g77 -o amira.dll bayer.o 
	cp amira.dll "c:/windows/system32"
	cp amira.dll "c:/devel/python"
