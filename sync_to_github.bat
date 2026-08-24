@echo off
chcp 65001 >nul
title 正在同步代码至 GitHub 远程仓库

echo ===================================================
echo   正在同步 Lab_Device_System 至 GitHub...
echo   目标仓库: https://github.com/zl1713033678/Lab_Device_System
echo ===================================================

git remote -v | findstr "origin" >nul 2>&1
if errorlevel 1 (
    echo [1/3] 配置远程仓库源 origin...
    git remote add origin https://github.com/zl1713033678/Lab_Device_System.git
) else (
    echo [1/3] 远程仓库源 origin 已配置
    git remote set-url origin https://github.com/zl1713033678/Lab_Device_System.git
)

echo [2/3] 暂存并提交本地最新变更...
git add .
git commit -m "feat: 智能实验室数字孪生系统，支持原生动态SVG 1F平面图、设备双向绑定与履历审计" >nul 2>&1

echo [3/3] 正在推送到远程 main 分支 (若弹出 GitHub 登录框请点击授权)...
git push -u origin main

if errorlevel 1 (
    echo.
    echo ---------------------------------------------------
    echo 若推送因远程仓库已有文件被拒绝，正在尝试同步合并后推送...
    git pull --rebase origin main
    git push -u origin main
)

echo.
echo ===================================================
echo   同步流程执行完毕！
echo ===================================================
pause
