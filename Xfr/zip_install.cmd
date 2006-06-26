@echo off
setlocal

set pt=..\bin
set pt1="c:\Program Files\WinZip"
if not exist %pt% goto err 
if not exist %pt1% goto err 

rem if .%1 == . goto use
if .%1 == .? goto use

if .%1 == . goto blankname

@echo on
set foldr=Install_%1
md %foldr%
set fn=xfr\%foldr%\Install_%1

goto continue

:blankname
echo ..
echo NBNB Usage should be: zip_install VerName [size]
echo Usage example: zip_install bin116
echo ---- Ctrl-C to cancel ----
echo Any key to continue and place zip files in install folder
echo ..
rem pause
@echo on
set foldr=Install
md %foldr%
set fn=xfr\%foldr%\Install


:continue 

copy DoInstall.cmd %foldr%\DoInstall.cmd

pushd ..

rem ========== required ==========
set fn2=%fn%_WinSys.zip
del %fn2%
wzzip -a %fn2% xfr\winsysdir\*.*

rem ========== required ==========
rem ----------- ScdBins
set fn2=%fn%_ScdBins.zip
del %fn2%
wzzip -a -P -r %fn2% bin\syscad91.exe
wzzip -a -P -r %fn2% bin\alumina1.dll bin\basic1.dll bin\commn1.dll bin\control1.dll
wzzip -a -P -r %fn2% bin\flwlib.dll bin\heatxch1.dll bin\kwdb.dll bin\mathand1.dll 
wzzip -a -P -r %fn2% bin\mdlbase.dll bin\scddesrvr.dll bin\scdif.dll 
wzzip -a -P -r %fn2% bin\scdlib.dll bin\scdmdl.dll bin\scdopcsrv.dll bin\scdslv.dll
wzzip -a -P -r %fn2% bin\scexec.dll bin\schist.dll bin\scopcsrv.dll 
wzzip -a -P -r %fn2% bin\separ1.dll bin\separ2.dll bin\sizedst1.dll bin\smdk1.dll 
wzzip -a -P -r %fn2% bin\unzip32.dll bin\xylib.dll bin\zip32.dll
wzzip -a -P -r %fn2% bin\power1.dll 
wzzip -a -P -r %fn2% bin\regall.cmd
wzzip -a -P -r %fn2% bin\scdif.tlb bin\scdmdl.tlb
wzzip -a -P -r %fn2% bin\scdslv.tlb bin\syscad91.tlb

rem ----------- MarshalBins
set fn2=%fn%_MarshalBins.zip
del %fn2%
wzzip -a -P -r %fn2% bin\scdcom.dll bin\scdvb.dll 
wzzip -a -P -r %fn2% bin\scdcom.tlb bin\scdvb.tlb
wzzip -a -P -r %fn2% bin\SysCADMarshal.exe bin\SysCADMarshal_ps.dll
wzzip -a -P -r %fn2% bin\ScdIODB.exe

rem ----------- BaseFiles
set fn2=%fn%_BaseFiles.zip
del %fn2%
wzzip -a -P -r %fn2% basefiles\default.mdb basefiles\tagdescdata.mdb
wzzip -a -P -r %fn2% basefiles\symbols\*.*
wzzip -a -P -r %fn2% basefiles\fonts\*.*

rem ----------- License
set fn2=%fn%_License.zip
del %fn2%
wzzip -a -P %fn2% license\syscad.exe
wzzip -a -P %fn2% license\cks.exe
wzzip -a -P %fn2% license\setupex.exe
wzzip -a -P %fn2% license\crp32001.ngn
wzzip -a -P %fn2% license\crypkeycom.dll

rem ========== Extra ==========
rem ----------- Documentation
set fn2=%fn%_Documentation.zip
del %fn2%
wzzip -a -P -r %fn2% "basefiles\Models Help 9.0\*.*"
wzzip -a -P -r %fn2% "basefiles\PGM Help 9.0\*.*"
wzzip -a -P -r %fn2% "basefiles\User Manual 9.0\*.*"

rem ----------- Examples
set fn2=%fn%_Examples.zip
del %fn2%
wzzip -a -P -r %fn2% "Tutorial\GeneralTutorial\*.*"
wzzip -a -P -r %fn2% "Examples\General Examples\*.*"

rem ----------- All In One
set fn2=%fn%_Essentials.zip
del %fn2%

wzzip -a %fn2% xfr\DoInstall.cmd
wzzip -a %fn2% %fn%_ScdBins.zip
wzzip -a %fn2% %fn%_MarshalBins.zip
wzzip -a %fn2% %fn%_BaseFiles.zip
wzzip -a %fn2% %fn%_WinSys.zip
wzzip -a %fn2% %fn%_License.zip

@echo off

popd

if not .%2== . (
del %fn%.pt.z*
@echo on
wzzip -ys%2 %fn%.zip %fn%.pt.zip 
@echo off
)

goto done

:err
echo %pt% or %pt1% does not exist!!!
goto done

:use
echo Usage: zip_install VerName [size]
echo example1: zip_install bin115
echo example2: zip_install bin116u3 1024

goto done

:done
