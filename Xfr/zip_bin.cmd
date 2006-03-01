@echo off
setlocal

set pt=..\bin
set pt1="c:\Program Files\WinZip"
if not exist %pt% goto err 
if not exist %pt1% goto err 

rem CNM if .%1 == . goto use

if .%1 == .? goto use

if .%1 == . goto blankname

@echo on
set foldr=%1
md %foldr%
set fn=%foldr%\%1
goto continue

:blankname
echo .
echo NBNB Usage should be: zip_bin VersionName
echo Usage example: zip_bin bin114u3
echo ---- Ctrl-C to cancel ----
echo Any key to continue and place files in ScdBins folder
echo .
@echo on
set foldr=ScdBins
md %foldr%
set fn=%foldr%\ScdBins

:continue 
rem ========== SysCAD Standard Files ==========
del %fn%_SysCAD.zip
wzzip -a %fn%_SysCAD.zip ..\bin\syscad.exe
wzzip -a %fn%_SysCAD.zip ..\bin\alumina1.dll ..\bin\basic1.dll ..\bin\commn1.dll ..\bin\control1.dll
wzzip -a %fn%_SysCAD.zip ..\bin\flwlib.dll ..\bin\heatxch1.dll ..\bin\kwdb.dll ..\bin\mathand1.dll 
wzzip -a %fn%_SysCAD.zip ..\bin\mdlbase.dll ..\bin\scdcom.dll ..\bin\scddesrvr.dll ..\bin\scdif.dll 
wzzip -a %fn%_SysCAD.zip ..\bin\scdlib.dll ..\bin\scdmdl.dll ..\bin\scdopcsrv.dll ..\bin\scdslv.dll
wzzip -a %fn%_SysCAD.zip ..\bin\scdvb.dll ..\bin\scexec.dll ..\bin\schist.dll ..\bin\scopcsrv.dll 
wzzip -a %fn%_SysCAD.zip ..\bin\separ1.dll ..\bin\separ2.dll ..\bin\sizedst1.dll ..\bin\smdk1.dll 
wzzip -a %fn%_SysCAD.zip ..\bin\unzip32.dll ..\bin\xylib.dll ..\bin\zip32.dll
wzzip -a %fn%_SysCAD.zip ..\bin\power1.dll 

wzzip -a %fn%_SysCAD.zip ..\bin\regall.cmd
wzzip -a %fn%_SysCAD.zip ..\bin\scdcom.tlb ..\bin\scdif.tlb ..\bin\scdmdl.tlb
wzzip -a %fn%_SysCAD.zip ..\bin\scdslv.tlb ..\bin\syscad.tlb
wzzip -a %fn%_SysCAD.zip ..\bin\scdvb.tlb

rem ========== SysCAD Marshal ==========
del %fn%_Marshal.zip
wzzip -a %fn%_Marshal.zip ..\bin\SysCADMarshal.exe
wzzip -a %fn%_Marshal.zip ..\bin\SysCADMarshal_ps.dll
wzzip -a %fn%_Marshal.zip ..\bin\ScdIODB.exe


rem ========== Alcan ==========
del %fn%_Alcan.zip
wzzip -a %fn%_Alcan.zip ..\bin\alcanspm.dll ..\bin\alcanusr.dll
rem wzzip -a %fn%_Alcan.zip ..\bin\alcanspusr.dll

rem ========== QAL ==========
del %fn%_QAL.zip
wzzip -a %fn%_QAL.zip ..\bin\qalspmdl.dll ..\bin\qalmdls.dll ..\bin\qalprcp1.dll
rem wzzip -a %fn%_QAL.zip ..\bin\ttechqal.dll 

rem ========== CAR ==========
del %fn%_CAR.zip
wzzip -a %fn%_CAR.zip ..\bin\qalspmdl.dll ..\bin\qalmdls.dll

rem ========== Alunorte ==========
del %fn%_Alunorte.zip
wzzip -a %fn%_Alunorte.zip ..\bin\alunorte.dll

rem ========== RTTS ==========
del %fn%_RTTS.zip
wzzip -a %fn%_RTTS.zip ..\bin\rtts.dll ..\bin\ts.dll ..\bin\ts_modellib.dll
wzzip -a %fn%_RTTS.zip ..\bin\ts_prc.dll

rem ========== Worsley ==========
del %fn%_BATC.zip
wzzip -a %fn%_BATC.zip ..\bin\batc_bayer.dll

rem ========== Iluka ==========
del %fn%_Iluka.zip
wzzip -a %fn%_Iluka.zip ..\bin\control2.dll

rem ========== Simutron ==========
del %fn%_Simutron.zip
wzzip -a %fn%_Simutron.zip ..\bin\SKO.dll

rem ========== Hatch ==========
del %fn%_HatchAlcoa.zip
wzzip -a %fn%_HatchAlcoa.zip ..\bin\alcoaspmdl.dll

rem ========== Kenwalt ==========
del %fn%_Kenwalt.zip
wzzip -a %fn%_Kenwalt.zip ..\bin\scdplcopcsrv.dll ..\bin\plc5.exe
wzzip -a %fn%_Kenwalt.zip ..\bin\kw_smdk1.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\demousr.dll ..\bin\demospusr.dll ..\bin\demoalspusr.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\csiro_units.dll
rem plus all the client DLLs
wzzip -a %fn%_Kenwalt.zip ..\bin\alcanspm.dll ..\bin\alcanusr.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\qalspmdl.dll ..\bin\qalmdls.dll ..\bin\qalprcp1.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\alunorte.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\rtts.dll ..\bin\ts.dll ..\bin\ts_modellib.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\ts_prc.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\batc_bayer.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\control2.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\alcoaspmdl.dll
wzzip -a %fn%_Kenwalt.zip ..\bin\SKO.dll

@echo off

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
echo Usage: zip_bin VersionName
echo example1: zip_bin bin114
echo example2: zip_bin bin114u3

goto done

:done

