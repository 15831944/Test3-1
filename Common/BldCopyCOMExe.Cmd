@echo off
copy %3\%2.tlb %1\Bin\%2.tlb
%1\Bin\%2.exe /regserver

