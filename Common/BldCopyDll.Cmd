@echo off
rem PathToSysCADRoot=%1
rem TargetName=%2
rem OutDir=%3
rem @echo on

rem Used for scdlib and smdk1

if not exist %1\SMDK\Bin\%3 md %1\SMDK\Bin\%3

xcopy /i /r /y %3\%2.lib %1\SMDK\Bin\%3

rem xcopy /i /y %1\Bin\%2.dll %1\SMDK\Bin\%3
