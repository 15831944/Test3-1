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
set foldr=SMDK_%1
md %foldr%
set fn=xfr\%foldr%\SMDK_%1

goto continue

:blankname
echo ..
echo NBNB Usage should be: zip_smdk VerName [size]
echo Usage example: zip_smdk bin120
echo ---- Ctrl-C to cancel ----
echo Any key to continue and place files in SMDK folder
echo ..
rem pause
@echo on
set foldr=SMDK
md %foldr%
set fn=xfr\%foldr%\SMDK_120

:continue 

pushd ..

rem ========== SMDK Units ==========
set fn2=%fn%_Units.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\examples\demousr\*.h smdk\examples\demousr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demousr\demousr.vcproj smdk\examples\demousr\demousr.sln

rem ========== SMDK Props ==========
set fn2=%fn%_Props.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\examples\demospusr\*.h smdk\examples\demospusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demospusr\demospusr.vcproj smdk\examples\demospusr\demospusr.sln

rem ========== SMDK Alumina Props ==========
set fn2=%fn%_AlProps.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\examples\demospusr\*.h smdk\examples\demospusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demospusr\demospusr.vcproj smdk\examples\demospusr\demospusr.sln
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\*.h smdk\examples\demoalspusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\demoalspusr.vcproj smdk\examples\demoalspusr\demoalspusr.sln

rem ========== SMDK Full ==========
set fn2=%fn%_Full.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\examples\demousr\*.h smdk\examples\demousr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demousr\demousr.vcproj vsmdk\examples\demousr\demousr.sln
wzzip -a -P -r %fn2% smdk\examples\demospusr\*.h smdk\examples\demospusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demospusr\demospusr.vcproj smdk\examples\demospusr\demospusr.sln
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\*.h smdk\examples\demoalspusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\demoalspusr.vcproj smdk\examples\demoalspusr\demoalspusr.sln

rem ========== SMDK Internal ==========
set fn2=%fn%_Internal.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\includeex\.h 
wzzip -a -P -r %fn2% smdk\binex\rls8\devlib.lib 
wzzip -a -P -r %fn2% smdk\binex\rls\devlib.lib 

rem ========== SMDK Alcan ==========
set fn2=%fn%_Alcan.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\examples\demousr\*.h smdk\examples\demousr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demousr\demousr.vcproj vsmdk\examples\demousr\demousr.sln
wzzip -a -P -r %fn2% smdk\examples\demospusr\*.h smdk\examples\demospusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demospusr\demospusr.vcproj smdk\examples\demospusr\demospusr.sln
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\*.h smdk\examples\demoalspusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\demoalspusr.vcproj smdk\examples\demoalspusr\demoalspusr.sln
wzzip -a -P -r %fn2% smdk\alcan\alcanusr\*.h smdk\alcan\alcanusr\*.cpp
wzzip -a -P -r %fn2% smdk\alcan\alcanusr\alcanusr.vcproj smdk\alcan\alcanusr\alcanusr.sln
wzzip -a -P -r %fn2% smdk\alcan\alcanusr\crp32d60.lib
rem wzzip -a -P -r %fn2% smdk\alcan\alcanspusr\*.h smdk\alcan\alcanspusr\*.cpp
rem wzzip -a -P -r %fn2% smdk\alcan\alcanspusr\alcanspusr.vcproj smdk\alcan\alcanspusr\alcanspusr.sln

rem ========== SMDK Simutron ==========
set fn2=%fn%_Simutron.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\examples\demousr\*.h smdk\examples\demousr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demousr\demousr.vcproj vsmdk\examples\demousr\demousr.sln
wzzip -a -P -r %fn2% smdk\examples\demospusr\*.h smdk\examples\demospusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demospusr\demospusr.vcproj smdk\examples\demospusr\demospusr.sln
wzzip -a -P -r %fn2% smdk\simutron\skomdls\*.h smdk\simutron\skomdls\*.cpp
wzzip -a -P -r %fn2% smdk\simutron\skomdls\skomdls.vcproj smdk\simutron\skomdls\skomdls.sln
wzzip -a -P -r %fn2% smdk\simutron\skospmdls\*.h smdk\simutron\skospmdls\*.cpp
wzzip -a -P -r %fn2% smdk\simutron\skospmdls\skospmdls.vcproj smdk\simutron\skospmdls\skospmdls.sln

rem ========== SMDK Kenwalt ==========
set fn2=%fn%_Kenwalt.zip
del %fn2%
wzzip -a -P -r %fn2% smdk\devlic.exe
wzzip -a -P -r %fn2% smdk\kwa.exe
wzzip -a -P -r %fn2% smdk\include\md_headers.h smdk\include\md_data.h
wzzip -a -P -r %fn2% smdk\include\md_share0.h smdk\include\md_share1.h smdk\include\md_share2.h
wzzip -a -P -r %fn2% smdk\include\md_share3.h smdk\include\md_share4.h
wzzip -a -P -r %fn2% smdk\include\md_defn.h smdk\include\md_spmodel.h smdk\include\md_vector.h smdk\include\md_method.h
wzzip -a -P -r %fn2% smdk\include\scdif.h smdk\include\scdmacros.h smdk\include\md_psd.h smdk\include\md_alumina.h
wzzip -a -P -r %fn2% smdk\bin\rls\devlib.lib smdk\bin\rls\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls\scdlib.lib smdk\bin\rls\scexec.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\devlib.lib smdk\bin\rls8\scdif.lib
wzzip -a -P -r %fn2% smdk\bin\rls8\scdlib.lib smdk\bin\rls8\scexec.lib
wzzip -a -P -r %fn2% smdk\examples\demousr\*.h smdk\examples\demousr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demousr\demousr.vcproj vsmdk\examples\demousr\demousr.sln
wzzip -a -P -r %fn2% smdk\examples\demospusr\*.h smdk\examples\demospusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demospusr\demospusr.vcproj smdk\examples\demospusr\demospusr.sln
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\*.h smdk\examples\demoalspusr\*.cpp
wzzip -a -P -r %fn2% smdk\examples\demoalspusr\demoalspusr.vcproj smdk\examples\demoalspusr\demoalspusr.sln
wzzip -a -P -r %fn2% smdk\kenwalt\kw_devlib\*.h smdk\kenwalt\kw_devlib\*.cpp
wzzip -a -P -r %fn2% smdk\kenwalt\kw_devlib\kw_devlib.vcproj vsmdk\kenwalt\kw_devlib\kw_devlib.sln
wzzip -a -P -r %fn2% smdk\kenwalt\kw_smdk2\*.h smdk\kenwalt\kw_smdk2\*.cpp
wzzip -a -P -r %fn2% smdk\kenwalt\kw_smdk2\kw_smdk2.vcproj vsmdk\kenwalt\kw_smdk2\kw_smdk2.sln
wzzip -a -P -r %fn2% smdk\includeex\*.h 
wzzip -a -P -r %fn2% smdk\binex\rls8\devlib.lib 
wzzip -a -P -r %fn2% smdk\binex\rls\devlib.lib 

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
echo Usage: zip_smdk VerName [size]
echo example1: zip_smdk bin114
echo example2: zip_smdk bin112u3 1024

goto done

:done
