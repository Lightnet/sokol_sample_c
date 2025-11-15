@echo off
setlocal
set MSYS2_PATH=C:\msys64\mingw64\bin
set PATH=%MSYS2_PATH%;%PATH%

set "BASERES=resources"
set "BUILDRES=build\resources"
xcopy "%BASERES%" "%BUILDRES%\" /E /I /Y

copy "script.lua" "build/script.lua"

if not exist build mkdir build
cd build
demo.exe
endlocal