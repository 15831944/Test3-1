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
set fn=%foldr%\Bins116

:continue 
rem ========== SysCAD Standard Files ==========
set fn2=%fn%_SysCAD.zip
del %fn2%
wzzip -a %fn2% ..\bin\syscad91.exe ..\bin\syscad.exe
wzzip -a %fn2% ..\bin\alumina1.dll ..\bin\basic1.dll ..\bin\commn1.dll ..\bin\control1.dll
wzzip -a %fn2% ..\bin\flwlib.dll ..\bin\heatxch1.dll ..\bin\kwdb.dll ..\bin\mathand1.dll 
wzzip -a %fn2% ..\bin\mdlbase.dll ..\bin\scdcom.dll ..\bin\scddesrvr.dll ..\bin\scdif.dll 
wzzip -a %fn2% ..\bin\scdlib.dll ..\bin\scdmdl.dll ..\bin\scdopcsrv.dll ..\bin\scdslv.dll
wzzip -a %fn2% ..\bin\scdvb.dll ..\bin\scexec.dll ..\bin\schist.dll ..\bin\scopcsrv.dll 
wzzip -a %fn2% ..\bin\separ1.dll ..\bin\separ2.dll ..\bin\sizedst1.dll ..\bin\smdk1.dll 
wzzip -a %fn2% ..\bin\unzip32.dll ..\bin\xylib.dll ..\bin\zip32.dll
wzzip -a %fn2% ..\bin\power1.dll 

wzzip -a %fn2% ..\bin\regall.cmd
wzzip -a %fn2% ..\bin\scdif.tlb ..\bin\scdmdl.tlb
wzzip -a %fn2% ..\bin\scdslv.tlb ..\bin\syscad.tlb
wzzip -a %fn2% ..\bin\scdvb.tlb ..\bin\scdcom.tlb

wzzip -a %fn2% ..\bin\crp32001.ngn

rem ========== SysCAD Marshal ==========
set fn2=%fn%_Marshal.zip
del %fn2%
wzzip -a %fn2% ..\bin\SysCADMarshal.exe
wzzip -a %fn2% ..\bin\SysCADMarshal_ps.dll
wzzip -a %fn2% ..\bin\ScdIODB.exe


rem ========== Alcan ==========
set fn2=%fn%_Alcan.zip
del %fn2%
wzzip -a %fn2% ..\bin\alcanspm.dll 
wzzip -a %fn2% ..\bin\alcanusr.dll ..\bin\alcanbayer.dll

rem ========== QAL ==========
set fn2=%fn%_QAL.zip
del %fn2%
wzzip -a %fn2% ..\bin\qalspmdl.dll ..\bin\qalmdls.dll ..\bin\qalprcp1.dll
rem wzzip -a %fn2% ..\bin\ttechqal.dll 

rem ========== CAR ==========
set fn2=%fn%_CAR.zip
del %fn2%
wzzip -a %fn2% ..\bin\qalspmdl.dll ..\bin\qalmdls.dll

rem ========== Alunorte ==========
set fn2=%fn%_Alunorte.zip
del %fn2%
wzzip -a %fn2% ..\bin\alunorte.dll

rem ========== RTTS ==========
set fn2=%fn%_RTTS.zip
del %fn2%
wzzip -a %fn2% ..\bin\rtts.dll ..\bin\ts.dll ..\bin\ts_modellib.dll
wzzip -a %fn2% ..\bin\ts_prc.dll

rem ========== Worsley ==========
set fn2=%fn%_BATC.zip
del %fn2%
wzzip -a %fn2% ..\bin\batc_bayer.dll ..\bin\batc_brahma.dll

rem ========== Iluka ==========
set fn2=%fn%_Iluka.zip
del %fn2%
wzzip -a %fn2% ..\bin\control2.dll ..\bin\ilukaunits.dll

rem ========== Hatch ==========
set fn2=%fn%_HatchAlcoa.zip
del %fn2%
wzzip -a %fn2% ..\bin\alcoaspmdl.dll

rem ========== Simutron ==========
set fn2=%fn%_Simutron.zip
del %fn2%
wzzip -a %fn2% ..\bin\SKO.dll ..\bin\SKOMdls.dll ..\bin\SKOSpMdls.dll

rem ========== Kenwalt ==========
set fn2=%fn%_Kenwalt.zip
del %fn2%
wzzip -a %fn2% ..\bin\scdplcopcsrv.dll ..\bin\plc5.exe
wzzip -a %fn2% ..\bin\kw_smdk1.dll
wzzip -a %fn2% ..\bin\demousr.dll ..\bin\demospusr.dll ..\bin\demoalspusr.dll
wzzip -a %fn2% ..\bin\csiro_units.dll
rem plus all the client DLLs
wzzip -a %fn2% ..\bin\alcanspm.dll ..\bin\alcanusr.dll ..\bin\alcanbayer.dll
wzzip -a %fn2% ..\bin\qalspmdl.dll ..\bin\qalmdls.dll ..\bin\qalprcp1.dll
wzzip -a %fn2% ..\bin\alunorte.dll
wzzip -a %fn2% ..\bin\rtts.dll ..\bin\ts.dll ..\bin\ts_modellib.dll
wzzip -a %fn2% ..\bin\ts_prc.dll
wzzip -a %fn2% ..\bin\batc_bayer.dll ..\bin\batc_brahma.dll
wzzip -a %fn2% ..\bin\control2.dll ..\bin\ilukaunits.dll
wzzip -a %fn2% ..\bin\alcoaspmdl.dll
wzzip -a %fn2% ..\bin\SKO.dll ..\bin\SKOMdls.dll ..\bin\SKOSpMdls.dll

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
echo example1: zip_bin bin116
echo example2: zip_bin bin116u3

goto done

:done

