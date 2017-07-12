@echo off

set PROJECTROOT=%~dp0

cd /D %PROJECTROOT%\Engine\Binaries\Win64

UE4Editor-cmd.exe %PROJECTROOT%\PBNeo\PBNeo.uproject -run=cook -targetplatform=PS4 -cookonthefly

pause