@echo off

set PROJECTROOT=%~dp0

cd /D %PROJECTROOT%\Engine\Build\BatchFiles

call Rebuild.bat PBNeo PS4 Shipping -waitmutex

pause
