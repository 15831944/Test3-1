call "C:\Program Files\Microsoft Visual Studio 9\Common7\Tools\vsvars32.bat"
call msbuild /verbosity:quiet /nologo /target:rebuild Service\Service.sln > Service\Service.log
call msbuild /verbosity:quiet /nologo /target:rebuild SysCAD.CS.CNM.sln > SysCAD.CS.CNM.log
call msbuild /verbosity:quiet /nologo /target:rebuild SysCAD.App.CLR.CNM.9.sln > SysCAD.App.CLR.CNM.9.log
