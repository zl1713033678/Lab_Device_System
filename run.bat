@echo off
chcp 65001 >nul
title 智能实验室数字孪生与不可篡改履历系统

echo ===================================================
echo   正在启动 智能实验室数字孪生与不可篡改履历系统...
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

echo [1/2] 正在启动 FastAPI 后端服务 (http://localhost:8000)...
start http://localhost:8000
%PYTHON_EXE% main.py

pause
