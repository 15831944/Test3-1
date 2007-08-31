call "C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat"
call msbuild /verbosity:quiet /nologo /target:rebuild Service\Service.sln > Service\Service.log
call msbuild /verbosity:quiet /nologo /target:rebuild SysCAD.CS.CNM.sln > SysCAD.CS.CNM.log
call msbuild /verbosity:quiet /nologo /target:rebuild SysCAD.App.8.CLR.CNM.sln > SysCAD.App.8.CLR.CNM.log
