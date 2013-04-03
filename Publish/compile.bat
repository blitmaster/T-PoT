@echo off

REM ============================================================================
REM Compiles the library and the applications
REM ----------------------------------------------------------------------------
REM Usage: compile.bat [-vs <Visual version>] <root directory> <solution> <project>+
REM where:
REM  - <Visual version> = 2003, 2005 or 2008, 2005 is chosen by default
REM  - <solution> and <project> are the names without extension
REM ============================================================================

set vs=2005

if "%1"=="-vs" (
    set vs=%2
    shift
    shift
)

set dir=%1
set solution=%2

REM ----------------------------------------------------------------------------

if %vs% EQU 2003 call "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
if %vs% EQU 2005 call "C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
if %vs% EQU 2008 call "c:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

pushd %dir%

REM ----------------------------------------------------------------------------

devenv %solution%.sln /clean Release
if ERRORLEVEL 1 exit 1

:loop
set project=%3
if "%project%" EQU "" goto :EOF
echo ### Project: %project%
devenv %solution%.sln /build Release /project %project%
if ERRORLEVEL 1 exit 1
shift
goto loop
