regsvr32 /s ScdCOM.dll
regsvr32 /s ScdVB.dll
regsvr32 /s ScdIF.dll
regsvr32 /s ScdMdl.dll
regsvr32 /s ScdSlv.dll
regsvr32 /s CrypKeyCOM.dll

if exist SysCADMarshal_ps.dll regsvr32 /s SysCADMarshal_ps.dll
if exist ScdIODB.exe        ScdIODB      /regserver
if exist SysCADMarshal.exe  SysCADMarshal /regserver

SysCAD91      /regserver

rem pause