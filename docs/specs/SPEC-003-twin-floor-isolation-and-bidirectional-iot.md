# SPEC-003 · 契约冻结、楼层空间隔离与硬件双向控制演进

- **状态**：`📝 规划中 (PLANNED) — 仅沉淀开发思路，暂不执行代码`
- **提出时间**：`2026-08-25 00:00:00+08:00`
- **开发完成时间**：`[待开发完成后回填 YYYY-MM-DD HH:mm:ss+08:00]`
- **原始需求描述**：
  > 拆分设备业务状态与硬件连接状态、修复前端楼层隔离与真实遥测显示、在测试保护下拆分前后端单体文件，并逐步评估真实 MQTT 接入、硬件双向控制与 3D 孪生演进。

---

## 1. 业务痛点与核心场景拆解
1. 当前 `main.py` 1556 行、`static/index.html` 2842 行高度集中；心跳超时直接把 `IN_USE` 覆写为 `IDLE`，混淆了“设备运行状态”与“传感器在线状态”。
2. 前端 1F/2F/3F 共用同一房间集合、锁定态忽略保存坐标、缺电流时以常量 `1.85A`/`0.05A` 估算负荷。
3. 后端 `ESP32MQTTAdapter` 与 `PLCModbusAdapter` 仅为占位透传，尚未支持真正 MQTT 订阅或继电器反向控制指令下发。

## 2. 架构与算法设计（含遵循 AGENTS.md 红线的边界约束）
- **状态正交拆分（原 `BE-001`/`BE-002`）**：`DeviceModel` 拆分为业务态（`IDLE`/`IN_USE`/`FAULT`）与连接态（`ONLINE`/`STALE`/`OFFLINE` + `last_seen_at` + `last_current`）；冻结版本化 API/WS 契约（含 `schema_version`、`event_id`、单位与统一错误响应）。
- **楼层与坐标真实化（原 `FE-001`）**：房间表增加 `floor`（`1F`/`2F`/`3F`）与持久坐标开关；无遥测设备显示“无测量数据”而非硬编码常量。
- **单体模块拆分（原 `ARCH-002`）**：仅在 `SPEC-001` 的 `pytest` 与关键浏览器 E2E 覆盖到位后，拆分为路由/领域服务/仓储与前端模块化资源。
- **双向控制与 3D 演进（原 `IOT-002`/`IOT-003`）**：若保留 MQTT/Modbus 声明则接入真实 Broker 与客户端；远程断电/通电命令需绑定鉴权、幂等 ID、ACK 超时与故障安全回滚。

## 3. 数据模型与接口扩展规划
- `rooms` 新增 `floor` 字段；`devices` 新增 `connection_state`、`last_seen_at`、`last_current`、`last_voltage`。
- 预留接口：`POST /api/devices/{device_code}/control`（依赖 `SPEC-001` 认证与硬件联锁规则）。

## 4. 分阶段实施路线图与验收打卡表 (Implementation Checklist)

- [ ] **Phase 1：状态拆分、契约冻结与前端真实性修复（原 `BE-001`、`BE-002`、`FE-001`）**
  - *计划内容*：分离在线态与业务态、统一 `raw_data.current` 阈值判定、落实房间 `floor` 隔离与锁定态持久坐标。
  - *完成状态与时间戳*：`[待完成]`
- [ ] **Phase 2：测试保护下的前后端模块化拆分与硬件验收（原 `ARCH-002`、`IOT-001`、`OPS-003`）**
  - *计划内容*：拆分 `main.py` 与 `static/index.html`、新增 `health/readiness` 探针、完成真实 ESP32 在环验收。
  - *完成状态与时间戳*：`[待完成]`
- [ ] **Phase 3：真实 MQTT/双向控制与 3D 立体孪生（原 `IOT-002`、`IOT-003`、Phase 2/3）**
  - *计划内容*：接入真实 MQTT/Modbus 协议或裁剪无效声明；按需落地带 ACK 与联锁的双向控制及 Three.js 3D 视图。
  - *完成状态与时间戳*：`[待完成]`
