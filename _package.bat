@echo off

set PROJECTROOT=%~dp0
set GAMENAME=%1
REM %2 is the configuration name (Shipping, Development)
set CONFIG=%2

cd /d %PROJECTROOT%\Engine\Build\BatchFiles

call RunUAT.bat BuildCookRun -project="%PROJECTROOT%\%GAMENAME%\%GAMENAME%.uproject" -ps4 -cook -stage -pak -package -build -clientconfig=%CONFIG% 