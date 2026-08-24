@echo off
chcp 65001 >nul
title ESP32 硬件遥测与防抖测试

echo ===================================================
echo   正在运行 ESP32 硬件电流传感器模拟测试套件...
echo ===================================================

set PYTHON_EXE=

if exist "%~dp0.venv\Scripts\python.exe" (
    set PYTHON_EXE="%~dp0.venv\Scripts\python.exe"
) else (
    py -3.13 -c "import fastapi" >nul 2>&1
    if not errorlevel 1 (
        set PYTHON_EXE=py -3.13
    ) else (
        set PYTHON_EXE=python
    )
)

%PYTHON_EXE% simulate_esp32.py
pause
