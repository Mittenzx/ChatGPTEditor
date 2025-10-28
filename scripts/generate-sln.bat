@echo off
rem Generate Visual Studio solution for Adastrea Unreal project (Windows)
rem Usage: double-click or run from repo/scripts directory

setlocal

rem Path to the .uproject relative to this script. Adjust if your .uproject is elsewhere.
set UPROJECT=%~dp0..\Adastrea.uproject

if not exist "%UPROJECT%" (
  echo ERROR: Adastrea.uproject not found at "%UPROJECT%"
  echo Please edit this script and set UPROJECT to the correct path.
  pause
  exit /b 1
)

rem Try common UE5.5 install location; override by setting UE_ROOT environment variable before running if needed.
if "%UE_ROOT%"=="" set UE_ROOT=%ProgramFiles%\Epic Games\UE_5.5

if not exist "%UE_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" (
  echo ERROR: Unreal Engine GenerateProjectFiles.bat not found at "%UE_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat"
  echo Set UE_ROOT to your engine installation directory, e.g. C:\Program Files\Epic Games\UE_5.5
  pause
  exit /b 2
)

echo Using UPROJECT: "%UPROJECT%"
echo Using UE_ROOT: "%UE_ROOT%"

rem Generate the Visual Studio solution
"%UE_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="%UPROJECT%" -game -engine
if %ERRORLEVEL% neq 0 (
  echo GenerateProjectFiles.bat failed with exit code %ERRORLEVEL%.
  pause
  exit /b %ERRORLEVEL%
)

echo Successfully generated Visual Studio solution files.
echo You can now open the .sln in the project root with Visual Studio.
pause
endlocal
