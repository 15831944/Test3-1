all:
	g77 -c -mrtd -O2  -o f77dll.o f77dll.f
	dllwrap -s --export-all --output-def f77dll.def  --driver-name g77  -o f77dll.dll f77dll.o 
	cp f77dll.dll "c:/program files/Microsoft Office/Office"
