@echo off
title 智能实验室数字孪生与不可篡改履历系统

cd /d "%~dp0"

echo ===================================================
echo   正在启动 智能实验室数字孪生与不可篡改履历系统...
echo ===================================================

:: 1. 检查 Python 虚拟环境
if exist "%~dp0.venv\Scripts\python.exe" goto START_SERVER

echo [1/2] 首次运行：正在为您自动创建 .venv 虚拟环境...
python -m venv "%~dp0.venv" 2>nul
if not exist "%~dp0.venv\Scripts\python.exe" py -m venv "%~dp0.venv" 2>nul

if not exist "%~dp0.venv\Scripts\python.exe" goto ENV_ERROR

echo [1/2] 首次运行：正在安装运行所需依赖...
"%~dp0.venv\Scripts\pip.exe" install -r "%~dp0requirements.txt"
goto START_SERVER

:ENV_ERROR
echo [错误] 未能找到 Python 环境，请先安装 Python 3.10+ 并加入 PATH 环境变量。
pause
exit /b 1

:START_SERVER
echo [2/2] 正在启动 FastAPI 后端服务并自动打开网页 (http://localhost:8000)...
start "" "http://localhost:8000"
"%~dp0.venv\Scripts\python.exe" "%~dp0main.py"

pause
