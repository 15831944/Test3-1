rem ===== required for dubugging SMDK source code =====

if exist CrypKeyCOMServer.exe CrypKeyCOMServer.exe /service

if exist CrypKeyCOM.dll regsvr32 /s CrypKeyCOM.dll

rem pause

