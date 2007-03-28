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
echo Usage example: zip_bin bin120u2
echo ---- Ctrl-C to cancel ----
echo Any key to continue and place files in ScdBins folder
echo .
@echo on
set foldr=ScdBins
md %foldr%
set fn=%foldr%\Bins122

:continue 
rem ========== SysCAD Standard Files ==========
set fn2=%fn%_SysCAD.zip
del %fn2%
wzzip -a %fn2% ..\bin\syscad91.exe
wzzip -a %fn2% ..\bin\alumina1.dll ..\bin\basic1.dll ..\bin\commn1.dll ..\bin\control1.dll ..\bin\flwlib.dll 
wzzip -a %fn2% ..\bin\heatxch1.dll ..\bin\kwdb.dll ..\bin\mathand1.dll ..\bin\mdlbase.dll ..\bin\scdcom.dll
wzzip -a %fn2% ..\bin\scddesrvr.dll ..\bin\scdif.dll ..\bin\scdlib.dll ..\bin\scdmdl.dll ..\bin\scdopcsrv.dll
wzzip -a %fn2% ..\bin\scdslv.dll ..\bin\scdvb.dll ..\bin\scexec.dll ..\bin\schist.dll ..\bin\scopcsrv.dll 
wzzip -a %fn2% ..\bin\separ1.dll ..\bin\separ2.dll ..\bin\sizedst1.dll ..\bin\smdk1.dll ..\bin\unzip32.dll 
wzzip -a %fn2% ..\bin\xylib.dll ..\bin\zip32.dll ..\bin\power1.dll

wzzip -a %fn2% ..\bin\regall.cmd ..\bin\syscad91.tlb ..\bin\scdif.tlb ..\bin\scdmdl.tlb ..\bin\scdslv.tlb 
wzzip -a %fn2% ..\bin\scdvb.tlb ..\bin\scdcom.tlb

rem ========== SysCAD License Files ==========
set fn2=%fn%_License.zip
del %fn2%
wzzip -a %fn2% ..\license\syscad.exe
wzzip -a %fn2% ..\license\crp32001.ngn
wzzip -a %fn2% ..\license\cks.exe ..\license\setupex.exe

rem ========== SysCAD Marshal ==========
set fn2=%fn%_Marshal.zip
del %fn2%
wzzip -a %fn2% ..\bin\SysCADMarshal.exe ..\bin\SysCADMarshal_ps.dll ..\bin\ScdIODB.exe


rem ------- Client DLLs -------

rem ========== Alcan ==========
set fn2=%fn%_c_Alcan.zip
del %fn2%
wzzip -a %fn2% ..\bin\alcanspm.dll 
wzzip -a %fn2% ..\bin\alcanusr.dll ..\bin\alcanbayer.dll

rem ========== Alunorte ==========
set fn2=%fn%_c_Alunorte.zip
del %fn2%
wzzip -a %fn2% ..\bin\alunorte.dll

rem ========== CAR ==========
set fn2=%fn%_c_CAR.zip
del %fn2%
wzzip -a %fn2% ..\bin\qalspmdl.dll ..\bin\qalmdls.dll
wzzip -a %fn2% ..\bin\ttechcar.dll

rem ========== DeBeers ==========
set fn2=%fn%_c_DeBeers.zip
del %fn2%
wzzip -a %fn2% ..\bin\limndw.dll

rem ========== Hatch ==========
set fn2=%fn%_c_HatchAlcoa.zip
del %fn2%
wzzip -a %fn2% ..\bin\alcoaspmdl.dll

rem ========== Iluka ==========
set fn2=%fn%_c_Iluka.zip
del %fn2%
wzzip -a %fn2% ..\bin\control2.dll ..\bin\ilukaunits.dll

rem ========== Maaden ==========
set fn2=%fn%_c_Maaden.zip
del %fn2%
wzzip -a %fn2% ..\bin\Alumina2.dll

rem ========== QAL ==========
set fn2=%fn%_c_QAL.zip
del %fn2%
wzzip -a %fn2% ..\bin\qalspmdl.dll ..\bin\qalmdls.dll
rem wzzip -a %fn2% ..\bin\qalprcp1.dll old
wzzip -a %fn2% ..\bin\ttechqal.dll

rem ========== RTTS ==========
set fn2=%fn%_c_RTTS.zip
del %fn2%
wzzip -a %fn2% ..\bin\rtts.dll ..\bin\ts.dll ..\bin\ts_modellib.dll
wzzip -a %fn2% ..\bin\ts_prc.dll

rem ========== Simutron ==========
set fn2=%fn%_c_Simutron.zip
del %fn2%
wzzip -a %fn2% ..\bin\SKOMdls.dll ..\bin\SKOSpMdls.dll

rem ========== Worsley ==========
set fn2=%fn%_c_BATC.zip
del %fn2%
wzzip -a %fn2% ..\bin\batc_bayer.dll ..\bin\batc_brahma.dll


rem ------ ALL Client DLLs ------
rem ========== Kenwalt ==========
set fn2=%fn%_Kenwalt.zip
del %fn2%
wzzip -a %fn2% ..\bin\scdplcopcsrv.dll ..\bin\plc5.exe
wzzip -a %fn2% ..\bin\kw_smdk1.dll
wzzip -a %fn2% ..\bin\demousr.dll ..\bin\demospusr.dll ..\bin\demoalspusr.dll
wzzip -a %fn2% ..\bin\csiro_units.dll
wzzip -a %fn2% ..\bin\Alumina2.dll
wzzip -a %fn2% ..\bin\HeatXch2.dll
wzzip -a %fn2% ..\bin\ttgas.dll
wzzip -a %fn2% ..\bin\control3.dll
rem plus all the client DLLs
wzzip -a %fn2% ..\bin\alcanspm.dll 
wzzip -a %fn2% ..\bin\alcanusr.dll ..\bin\alcanbayer.dll
wzzip -a %fn2% ..\bin\alunorte.dll
wzzip -a %fn2% ..\bin\ttechcar.dll
wzzip -a %fn2% ..\bin\alcoaspmdl.dll
wzzip -a %fn2% ..\bin\control2.dll ..\bin\ilukaunits.dll
wzzip -a %fn2% ..\bin\qalspmdl.dll ..\bin\qalmdls.dll
wzzip -a %fn2% ..\bin\ttechqal.dll
wzzip -a %fn2% ..\bin\alcoaspmdl.dll
wzzip -a %fn2% ..\bin\rtts.dll ..\bin\ts.dll ..\bin\ts_modellib.dll
wzzip -a %fn2% ..\bin\ts_prc.dll
wzzip -a %fn2% ..\bin\SKOMdls.dll ..\bin\SKOSpMdls.dll
wzzip -a %fn2% ..\bin\batc_bayer.dll ..\bin\batc_brahma.dll

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
echo example1: zip_bin bin120
echo example2: zip_bin bin120u3

goto done

:done

