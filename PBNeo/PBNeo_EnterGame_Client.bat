@echo game start

@echo off
@echo 127.0.0.1?Team=0 -game -Windowed ?ResX=600 ?ResY=400
%~dp0../Engine/Binaries/Win64/UE4Editor.exe ../../../PBNeo/PBNeo.uproject 127.0.0.1?Team=1 -game -Windowed ?ResX=600 ?ResY=400