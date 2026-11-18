# AGENTS.md · 研发与 AI 编程协作宪章

> 任何开发者或 AI Agent 在阅读、修改本项目代码前，**必须先阅读本文档与 [`docs/STATUS.md`](./docs/STATUS.md)**，严守以下领域不变量与协作门禁。

## 1. 核心代码地图

```text
Lab_Device_System/
├── main.py                  # FastAPI 单体后端：ORM 4表、遥测状态机、WS广播、mDNS、导出
├── static/index.html        # 单文件前端：SVG 1F~3F 蓝图、双向联动、资产管理与 WS 消费
├── esp32/main/main.ino      # ESP32 固件：ACS712 RMS 采样、OLED 显示、mDNS 寻址与 HTTP 上报
├── simulate_esp32.py        # 本地 4 阶段遥测烟测脚本（注意会写入当前默认数据库）
├── run.bat                  # Windows GBK 启动脚本：.venv 自愈、依赖安装与服务启动
├── requirements.txt         # Python 运行与打包依赖清单
└── SmartLabSystem.spec      # PyInstaller 打包配置（含历史绝对路径待清理）
```

## 2. 不可违背的领域与工程红线（Hard Invariants）

1. **凭据与运行态隔离**：严禁提交真实 Wi-Fi 密码、令牌、内网生产 IP 或带业务数据的 `*.db` 文件；测试不得污染默认 `lab_devices.db`。
2. **契约与设备码锚定**：硬件逻辑绑定以 `device_code` 为唯一主键与 IP 解耦；变更 `/api/v1/telemetry/ingest`、`/ws/devices` 或 SQLite 表结构须同步校验后端、前端与固件契约。
3. **诚实评估边界（Truth Matrix）**：严禁将启发式估算、HTTP 透传适配器或未真正触发阈值的烟测脚本描述为“已生产就绪”。
4. **局部修改原则**：只修改当前任务所需文件，不顺手重构 `main.py` / `static/index.html` 无关区域（架构拆分需在测试保护下按 Spec 推进）。

## 3. 文档、排障与特性孵化流规

1. **特性孵化禁令（Spec Gate）**：[`docs/specs/`](./docs/specs/README.md) 下的 `PLANNED` 文档仅作设计储备，**未经用户明确指令严禁擅自编写业务代码**；获准落地后须回写完成时间戳并刷新 `docs/STATUS.md`。
2. **科学排障闭环（Red-Green Bug Gate）**：
   - 新缺陷先登记至 [`docs/BUGS.md`](./docs/BUGS.md)（标注 `S0~S2` 与精确 `file:line`）；
   - 修复时坚持 **先写失败测试复现（Red）→ 单一职责最小修复（Green）→ 全量回归**；
   - 验通后从 `docs/BUGS.md` 移除并归档至 [`CHANGELOG.md`](./CHANGELOG.md)。
3. **状态快照覆写规则**：[`docs/STATUS.md`](./docs/STATUS.md) 采用**覆写更新**（≤ 80 行），禁止以“末尾追加补丁”制造前后矛盾。

## 4. 构建与验证命令

```powershell
# Python 语法静态检查
.\.venv\Scripts\python.exe -m py_compile main.py simulate_esp32.py

# 依赖冲突检查
.\.venv\Scripts\python.exe -m pip check

# 启动后端并执行本地遥测烟测（会写入默认数据库，执行前注意备份）
.\.venv\Scripts\python.exe main.py
.\.venv\Scripts\python.exe simulate_esp32.py
```
