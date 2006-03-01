@echo off
rem PathToSysCADRoot=%1
rem TargetName=%2
rem OutDir=%3
rem @echo on

rem Used for scdif

if not exist %1\SMDK\Bin\%3 md %1\SMDK\Bin\%3

xcopy /i /r /y %2.tlb %1\Bin
xcopy /i /r /y %3\%2.lib %1\SMDK\Bin\%3
xcopy /i /r /y %2.h %1\SMDK\Include	

rem xcopy /i /r /y %1\Bin\%2.dll %1\SMDK\Bin\%3 	
rem xcopy /i /r /y %1\Bin\%2.tlb %1\SMDK\Bin\%3  
rem xcopy /i /r /y %2_i.c %1\SMDK\Include
