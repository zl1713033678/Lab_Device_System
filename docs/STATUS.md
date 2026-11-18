# Lab_Device_System 当前工程状态快照 (Single Source of Truth)

> **最后更新**：`2026-11-18` ｜ **当前基线**：`bd98ff1` ｜ **版本库**：`main` ｜ **自动化测试**：`暂无正式 pytest 套件（仅 simulate_esp32.py 烟测）`
> **维护规则**：本文件采用**覆写式更新**（上限 80 行），始终真实反映当下代码库的成熟度边界与下一步焦点。

---

## 1. 模块成熟度真伪矩阵 (Truth Matrix)

> **成熟度定义**：`L1 [Demo模拟]` ｜ `L1.5 [混合原型]` ｜ `L2 [真实逻辑闭环]` ｜ `L3 [自动化测试与并发锁加固]` ｜ `L4 [生产就绪]`

| 模块 | 等级 | 已真实落地并验证的能力 (Real & Verified) | 仍属演示模拟或待补齐缺口 (Mock / Gap) |
| :--- | :---: | :--- | :--- |
| **后端 CRUD 与报表导出**<br>([`main.py`](../main.py)) | **L2** | SQLite 4 表与启动迁移；设备/房间 CRUD；房间改名级联；`FAULT` 人工解锁；CSV 与多 Sheet Excel 导出 | 缺认证鉴权、输入限流、CSV 公式注入防护、可注入测试 DB 配置与并发保护 |
| **遥测防抖与离线心跳**<br>([`main.py`](../main.py)) | **L1.5** | `DefaultTelemetryAdapter` 施密特滤波（`>0.5A` / `<=0.05A`）；10s 无心跳超时切待机；WS `/ws/devices` 广播 | 业务状态与在线心跳混用；`raw_data.current` 未回填至阈值判定；`ESP32MQTTAdapter`/`PLCModbusAdapter` 实为 HTTP 透传 |
| **数字孪生 SVG 看板**<br>([`static/index.html`](../static/index.html)) | **L1.5** | 原生 SVG 1F~3F 蓝图缩放拖拽；浅色 Liquid Glass 界面、原生 `<dialog>` + `inert` 抽屉与无 `innerHTML` 安全 DOM；单状态源联动与 WS 刷新 | 锁定态不应用已保存坐标；1F~3F 共用房间列表；无遥测时用常量 `1.85A`/`0.05A` 估算负荷 |
| **ESP32 固件与 mDNS**<br>([`esp32/main/main.ino`](../esp32/main/main.ino)) | **L1.5** | ACS712 100 点 RMS 采样与死区过滤；OLED 状态展示；`smartlab.local` mDNS 探测与备用 IP 降级上报 | 源码硬编码明文 Wi-Fi 与内网 IP；缺可复现配置隔离、真实硬件在环与断网长稳验收 |
| **履历存证、安全与测试**<br>([`simulate_esp32.py`](../simulate_esp32.py)) | **L1** | Append-Only `device_usage_logs` 表；`simulate_esp32.py` 4 阶段烟测流 | 无哈希链字段与 `/api/v1/audit/verify`；无登录/设备签名；`simulate_esp32.py` 传显式 `state` 绕过防抖阈值；`lab_devices.db` 仍受 Git 跟踪 |

---

## 2. 质量、底座与孵化池状态

- **数据与运行底座**：`run.bat` 支持 `.venv` 自愈与单进程启动，但 `SmartLabSystem.spec` 仍含旧绝对路径，且尚无 CI/pytest 门禁。
- **活跃缺陷单**：`S0 = 3`、`S1 = 5`、`S2 = 2`（完整坐标与触发证据见 [`docs/BUGS.md`](./BUGS.md)）。
- **待孵化特性池**：共 `3` 项处于 `📝 PLANNED` 待命态（详见 [`docs/specs/README.md`](./specs/README.md)）。

---

## 3. 下一步高优行动清单 (Next 3 Priorities)

1. **[P0 安全止血]**：清理 [`BUG-001`](./BUGS.md) 明文 Wi-Fi/内网 IP 与 Git 跟踪的 `lab_devices.db`，并由环境所有者轮换已暴露凭据。
2. **[P0 测试底座]**：解决 [`BUG-005`](./BUGS.md) 与硬编码数据库路径耦合，建立基于 `pytest` + 临时 SQLite 的隔离状态机与边界回归测试。
3. **[P1 契约修复]**：落地 [`BUG-004`](./BUGS.md)、[`BUG-006`](./BUGS.md)、[`BUG-007`](./BUGS.md)、[`BUG-008`](./BUGS.md) 修正（阈值链路、锁定态坐标、楼层隔离、真实遥测与 CSV 转义）。
