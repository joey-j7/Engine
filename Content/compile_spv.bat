@echo off
cd %~dp0

setlocal enabledelayedexpansion

call :treeProcess
goto :eof

:treeProcess

for %%f in (*.glsl) do (
    set string=%%f
	
    REM glslangValidator.exe %%f -G -DAPI_GLSL -o !string:~0,-4!gspv
    glslangValidator.exe %%f -V -DAPI_VULKAN -o !string:~0,-4!vspv
)

for /D %%d in (*) do (
    cd %%d
    call :treeProcess
    cd ..
)

endlocal

pause
exit /b