# Lab_Device_System · 智能实验室数字孪生与设备履历系统

基于 **FastAPI + SQLite + 原生 HTML5 SVG** 的实验室设备数字孪生看板与 ESP32 遥测原型系统，支持设备/房间管理、施密特电流防抖、WebSocket 实时同步、mDNS 局域网零配置寻址及 Excel/CSV 履历导出。

> **当前阶段**：可运行原型（正处于 M0 安全加固与测试补全期），尚未达到生产发布门禁，详见 [`docs/STATUS.md`](./docs/STATUS.md)。

## 1. 核心架构与模块速览

- **后端服务（[`main.py`](./main.py)）**：FastAPI + Lifespan 后台心跳巡检（10s 离线切待机）、SQLite ORM 4 表模型、`DefaultTelemetryAdapter` 防抖状态机、WebSocket `/ws/devices` 广播、`zeroconf` mDNS 广播（`smartlab.local:8000`）与 Excel/CSV 导出。
- **前端孪生（[`static/index.html`](./static/index.html)）**：毛玻璃三段式单文件应用，集成 1F/2F/3F 原生 SVG 建筑蓝图缩放拖拽、房间与设备联动抽屉及 WebSocket 实时渲染。
- **硬件与模拟（[`esp32/main/main.ino`](./esp32/main/main.ino)、[`simulate_esp32.py`](./simulate_esp32.py)）**：ESP32 + ACS712 交流 RMS 采样及 mDNS 自动寻址固件；本地 4 阶段 HTTP 遥测模拟烟测脚本。

## 2. 技术栈与快速启动

- **后端栈**：Python 3.10+、FastAPI、Uvicorn、SQLAlchemy、Pandas/OpenPyXL、Zeroconf
- **前端 / 硬件**：原生 HTML5/CSS/JS + SVG；ESP32 Arduino (`ESPmDNS`、`HTTPClient`、`ArduinoJson`)

```powershell
# 一键自愈启动（自动创建 .venv、安装依赖、初始化 SQLite 并打开浏览器）
.\run.bat

# 或直接运行后端服务
.\.venv\Scripts\python.exe main.py

# 运行本地硬件遥测烟测（需先启动后端；支持 --new 测试自动发现）
.\.venv\Scripts\python.exe simulate_esp32.py
```

## 3. 配置与安全注意

| 配置项 | 当前实现位置 | 说明 |
| :--- | :--- | :--- |
| 数据库连接 | `main.py` (`DATABASE_URL`) | 当前固定为 `sqlite:///./lab_devices.db`，尚不支持环境变量注入 |
| 服务端端口 / mDNS | `main.py` (`8000` / `smartlab.local`) | 启动时注册 `_http._tcp.local.` 服务，失败时自动降级跳过 |
| ESP32 网络配置 | `esp32/main/main.ino` | 当前源码仍含硬编码 Wi-Fi 与备用 IP（见 `BUG-001`），烧录前须修改并禁止提交真实凭据 |

## 4. 项目文档导航地图

| 文档路径 | 用途与目标读者 |
| :--- | :--- |
| [`AGENTS.md`](./AGENTS.md) | **研发与 AI 协作宪章**：核心代码地图、领域硬红线、排障与验证命令 |
| [`docs/STATUS.md`](./docs/STATUS.md) | **实时状态快照**：各模块真实度矩阵（L1~L4）与下一步高优行动 |
| [`docs/BUGS.md`](./docs/BUGS.md) | **活跃缺陷台账**：当前未修复的 S0~S2 缺陷清单与精确代码坐标 |
| [`docs/adr/`](./docs/adr/0001-mdns-zeroconf-and-device-code-decoupling.md) | **架构决策记录 (ADR)**：mDNS 设备解耦与原生 SVG 选型权衡 |
| [`docs/specs/`](./docs/specs/README.md) | **特性孵化器 (Specs)**：待开发安全、哈希链与双向控制方案（未经指令不写代码） |
| [`docs/guides/operations-and-onboarding.md`](./docs/guides/operations-and-onboarding.md) | **运维与上手手册**：新机 Clone 自愈流程、ESP32 联调与 PyInstaller 打包指南 |
| [`CHANGELOG.md`](./CHANGELOG.md) | **版本演进归档**：历史里程碑、12 项排障修复记录与文档体系变更 |
