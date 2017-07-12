@echo off

set PROJECTROOT=%~dp0

cd /D %PROJECTROOT%\Engine\Build\BatchFiles

call Build.bat PBNeo PS4 Development -waitmutex

pause
