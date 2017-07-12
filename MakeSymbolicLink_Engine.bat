@echo off

REM == 생성위치 설정 (폴더와 절대 경로를 입력한다)
cd /d %~dp0
call _makeSymbolicLink.bat TestSymbolicLinkFolder "E:\Test Test"

pause