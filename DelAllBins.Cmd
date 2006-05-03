erase bin\*.exe 
erase bin\*.dll 
erase bin\*.ilk 
erase bin\*.tlb 
erase bin\*.exp 
erase bin\*.lib 
erase bin\secure\*.exe 
erase bin\secure\*.dll 
erase bin\secure\*.ilk 
erase bin\secure\*.exp 
erase bin\secure\*.tlb
erase bin\secure\*.lib 
rem erase /s /q smdk\bin\dbg\*.*
rem erase /s /q smdk\bin\rdb\*.*
rem erase /s /q smdk\bin\rls\*.*

dir dbg /s /b > delallbins.tmp
dir rdb /s /b >> delallbins.tmp
dir rls /s /b >> delallbins.tmp
dir dbg8 /s /b >> delallbins.tmp
dir rdb8 /s /b >> delallbins.tmp
dir rls8 /s /b >> delallbins.tmp

for /f "eol=; tokens=1* delims=, " %%i in (delallbins.tmp) do rd /s /q %%i
del delallbins.tmp
