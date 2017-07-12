@echo off

REM === %2 은 절대 경로입니다 ====
set TARGET_FOLDERNAME=%1
set TARGET_DIR=%2
set TARGET_FULLPATH=%TARGET_DIR%\%TARGET_FOLDERNAME%
REM === "" 기호 있으면 생략
set TARGET_FULLPATH_NORMAL=%TARGET_FULLPATH:"=%

if Exist "%TARGET_FULLPATH_NORMAL%" (
echo Target folder is already exist. make sure delete %TARGET_FOLDERNAME% folder
goto ERROR
)

set SOURCE_DIR=%~dp0

echo Source Directory : "%SOURCE_DIR%"
echo Target Directory : "%TARGET_DIR%"

REM ===========================================================================
REM 심볼릭 링크 만들기
REM ===========================================================================
mklink /d "%TARGET_FULLPATH_NORMAL%" "%SOURCE_DIR%\%TARGET_FOLDERNAME%"

REM ===========================================================================
REM 성공
REM ===========================================================================
:SUCCESS
cd /d "%SOURCE_DIR%"
echo --------------------------------------------------------------------------
echo Success
@pause
exit /b 0

REM ===========================================================================
REM 에러 처리
REM ===========================================================================
:ERROR
cd /d "%SOURCE_DIR%"
echo --------------------------------------------------------------------------
echo Failed
@pause
cd /d "%SOURCE_DIR%"
exit /b -1