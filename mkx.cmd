@echo off
rem echo NBNB DSP and DSW files not backed up

setlocal

rem pushd ..

set pt=.
if not exist %pt% goto err 
if not exist %pt%\mkexc.lst goto err 
if not exist %pt%\mkinc.lst goto err 

if .%1 == .? goto use

if .%1== . (
set what=
) else (
set what=-td%1
)

if .%2== . (
set fn=4xfer
) else (
set fn=%2
)

if .%3== . (
set sz=
) else (
set sz=%3
)

del %pt%\%fn%.zip
del %pt%\%fn%.pt.z*

@echo on
wzzip -skwa -a -P -r -jhrs %what% %pt%\%fn%.zip @%pt%\mkinc.lst -x@%pt%\mkexc.lst 

if .%sz%== . (
rem
) else (
wzzip -ys%sz% %pt%\%fn%.zip %pt%\%fn%.pt.zip 
)
@echo off

goto done

:err
echo %pt% or mkexc.lst or mkinc.lst does not exist!!! (Edit MKX.CMD?)
goto done

:use
echo Usage: mkx numdays name [size]
echo example1: mkx 5 4xfr
echo example2: mkx 60 4xfr 1024

goto done

:done

rem popd