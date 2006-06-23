rem assumes wzunzip and regsvr32 is in path 

rem =====================================================
rem System Files
Set WSPath=%windir%\SYSTEM32\
wzunzip -e -d -n Install_WinSys %WSPath%
rem  register OCX
regsvr32 /s %WSPath%ssdw3b32.ocx


rem =====================================================
rem Main System Files
wzunzip -e -d Install_ScdBins
wzunzip -e -d Install_MarshalBins
wzunzip -e -d Install_BaseFiles
wzunzip -e -d Install_License

rem  SetUp License Server
pushd license
setupex.exe 
regsvr32 /s crypkeycom.dll
popd

rem  Register SysCAD COM
pushd bin 
regall.cmd
popd

rem =====================================================
rem Extra Files

wzunzip -e -d Install_Documentation
wzunzip -e -d Install_Examples
