@echo off

echo dir %cd%

set AndroidPluginPath="./plugins/Android"
set RelasePath=".\build\cmake\bin\release"
set WindeployqtPath=%QT5Path%/bin/windeployqt.exe

rem delete dir
rmdir /s/q .\build

call ./scripts/BuildProject.bat
call ./scripts/BuildAndroidLibs.bat
call ./scripts/CopyConfig.bat
call ./scripts/Deployqt.bat

echo finsih scripts %RelasePath%\LoliProfiler.exe

:Exit
exit /B 1