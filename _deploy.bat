@echo off

REM %2 is the configuration name

set PROJECTROOT=%~dp0
set GAMENAME=%1
set USERIP=%2
set CONFIG=%3

cd /d %PROJECTROOT%\Engine\Build\BatchFiles

call RunUAT.bat BuildCookRun -project="%PROJECTROOT%\%GAMENAME%\%GAMENAME%.uproject" -ps4 -cook -stage -pak -deploy -device=%USERIP% -clientconfig=%CONFIG%

pause