# Lab_Device_System 运维、上手与打包手册

> 本文档承接日常开机自愈、ESP32 模拟联调与 Windows EXE 打包说明；核心架构与当前成熟度边界请查阅 [`README.md`](../../README.md) 与 [`docs/STATUS.md`](../STATUS.md)。

---

## 1. 新机 Clone 四层自动补全与启动流程

通过 [`run.bat`](../../run.bat) 在新 Windows 主机上启动时的执行链路：

```text
[git clone 仓库] ──> [双击 run.bat]
                          │
        ┌─────────────────┴─────────────────┐
        ▼                                   ▼
 [缺失 .venv]                         [已存在 .venv]
 1. python -m venv .venv                    │
 2. pip install -r requirements.txt         │
        └─────────────────┬─────────────────┘
                          ▼
             [启动 .venv python main.py]
                          │
        ┌─────────────────┴─────────────────┐
        ▼                                   ▼
 [缺失 lab_devices.db]                [已存在数据库]
 1. check_and_migrate_db / create_all       │
 2. lifespan 预置默认房间与测试设备          │
        └─────────────────┬─────────────────┘
                          ▼
       [打开 http://localhost:8000 并广播 smartlab.local:8000]
```

> **提示**：[`run.bat`](../../run.bat) 采用 Windows 原生 GBK 编码与扁平 `goto` 控制流，编辑该脚本时切勿直接保存为带复杂中文括号块的 UTF-8 文件，以免 CMD 词法解析报错。

---

## 2. 日常运行与本地遥测烟测命令

```powershell
# 1. 启动后端与 Web 界面（单进程直启，Ctrl+C 可立即释放 8000 端口）
.\.venv\Scripts\python.exe main.py

# 2. 对默认设备 DEV-002 运行 4 阶段遥测烟测（注意会写入当前 lab_devices.db）
.\.venv\Scripts\python.exe simulate_esp32.py

# 3. 测试全新未注册硬件接入自动发现流程
.\.venv\Scripts\python.exe simulate_esp32.py --new
```

---

## 3. ESP32 固件配置与联调注意事项

1. 固件位于 [`esp32/main/main.ino`](../../esp32/main/main.ino)，烧录前务必核对第 10–21 行的 `WIFI_SSID`、`WIFI_PASS`、`FALLBACK_SERVER_IP` 与 `DEVICE_CODE`，**切勿将生产环境真实密码或私网地址提交至远程仓库**。
2. 服务端启动时通过 `zeroconf` 注册 `smartlab.local:8000`；若运行日志提示 `[mDNS] 局域网服务广播跳过` 或路由器禁用组播，ESP32 会自动回退至 `FALLBACK_SERVER_IP`。
3. 硬件端每次循环按 `delay(1500)`（1.5 秒）上报一次；后端 `heartbeat_checker_loop` 设定为 10 秒未收到遥测即判定通信中断并广播 `device_offline`。

---

## 4. PyInstaller 打包注意事项

- 打包配置位于 [`SmartLabSystem.spec`](../../SmartLabSystem.spec)。当前该文件第 5、8 行仍保留硬编码路径 `E:\code\Lab_Device_System\...`（见 [`BUG-010`](../BUGS.md)），在其它目录或新机器执行打包前需将路径替换为相对路径或当前工作目录绝对路径：

```powershell
.\.venv\Scripts\pyinstaller.exe --clean SmartLabSystem.spec
```

- 打包产物位于 `dist/SmartLabSystem.exe`（已写入 `.gitignore`），运行前需确认未有旧进程残留占用 `8000` 端口。
