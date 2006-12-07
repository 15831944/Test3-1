dir dbg rdb rls dbg8 rdb8 rls8 /s /b > delallbins.tmp
for /f "eol=; delims=" %%i in (delallbins.tmp) do rd /s /q "%%i"

cd bin
dir *.exe *.dll *.ilk *.tlb *.exp *.lib *.pdb /s /b > delallbins.tmp
for /f "eol=; delims=" %%i in (delallbins.tmp) do del /s /q "%%i"
cd ..

cd GUIs
dir *.exe *.dll *.ilk *.tlb *.exp *.lib *.pdb /s /b > delallbins.tmp
for /f "eol=; delims=" %%i in (delallbins.tmp) do del /s /q "%%i"
cd ..

cd Interfaces
dir *.exe *.dll *.ilk *.tlb *.exp *.lib *.pdb /s /b > delallbins.tmp
for /f "eol=; delims=" %%i in (delallbins.tmp) do del /s /q "%%i"
cd ..

cd Services
dir *.exe *.dll *.ilk *.tlb *.exp *.lib *.pdb /s /b > delallbins.tmp
for /f "eol=; delims=" %%i in (delallbins.tmp) do del /s /q "%%i"
cd ..

del delallbins.tmp
