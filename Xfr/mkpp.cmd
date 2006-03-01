@echo off
setlocal

set pt=..\bin

if .%1 == .? goto use

if .%1== . (
set fn=bin_pp
) else (
set fn=%1_pp
)

del %fn%.zip

@echo on

wzzip -a %fn%.zip ..\bin\scdslv.dll ..\bin\scdmdl.dll ..\bin\scdif.dll ..\bin\scdlib.dll ..\bin\scexec.dll ..\bin\flwlib.dll
wzzip -a %fn%.zip ..\bin\scdif.tlb ..\bin\scdmdl.tlb ..\bin\scdslv.tlb
wzzip -a %fn%.zip ..\bin\regpp.cmd

@echo off

if not .%2== . (
del %fn%.pt.z*
@echo on
wzzip -ys%2 %fn%.zip %fn%.pt.zip 
@echo off
)

goto done

:err
echo %pt% does not exist!!!
goto done

:use
echo Usage: mkpp name
echo example: mkpp bin101u3

goto done

:done



