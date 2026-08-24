@echo off
chcp 65001 >nul
title 智能实验室数字孪生与不可篡改履历系统

echo ===================================================
echo   正在启动 智能实验室数字孪生与不可篡改履历系统...
echo ===================================================

set VENV_DIR=%~dp0.venv
set PYTHON_EXE=

if not exist "%VENV_DIR%\Scripts\python.exe" (
    echo [检测到环境未初始化] 正在为您自动创建 Python 虚拟环境...
    python -m venv "%VENV_DIR%" 2>nul || py -m venv "%VENV_DIR%" 2>nul
    if not exist "%VENV_DIR%\Scripts\python.exe" (
        echo [错误] 未能在当前电脑找到 Python 环境，请先安装 Python 3.10+ 并加入 PATH 环境变量。
        pause
        exit /b 1
    )
    echo [正在自动补全依赖] 正在从 requirements.txt 安装运行所需库...
    "%VENV_DIR%\Scripts\pip.exe" install -r "%~dp0requirements.txt"
    echo [环境初始化完成]
)

set PYTHON_EXE="%VENV_DIR%\Scripts\python.exe"

echo [启动中] 正在启动 FastAPI 后端服务 (http://localhost:8000)...
%PYTHON_EXE% main.py

pause
