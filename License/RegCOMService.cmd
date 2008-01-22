@echo Register the CrypKey COM Service
@rem only needed by developers using debugger with /dev commandline option

@rem For crypkey 6.5
@rem CrypKeyCOMServer.exe /service

@rem For crypkey 7.1
regsvr32 CrypKeyCOM7.dll

