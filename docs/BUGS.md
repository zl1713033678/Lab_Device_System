# Lab_Device_System 活跃缺陷追踪台账 (`BUGS.md`)

> **维护契约**：
> 1. 本台账仅保留**当前尚未修复（`OPEN` / `IN_PROGRESS`）**的真实代码与数据缺陷；已修复并验证通过的条目立即移入 [`CHANGELOG.md`](../CHANGELOG.md)。
> 2. 修复必须遵循 **「先写失败测试复现 (Red) → 最小原子化修复 (Green) → 回归通过 → 清零归档」** 闭环。
> 3. **严重度定义**：`S0`（越权接管 / 资损 / 数据损坏）｜ `S1`（主流程算错 / 并发丢更新）｜ `S2`（边缘偏差 / 过渡简化）｜ `S3`（UI/体验瑕疵）。

---

## 活跃缺陷总表

| 编号 | 级别 | 所属模块 | 缺陷描述与触发证据 | 代码坐标 | 状态 |
| :--- | :---: | :--- | :--- | :--- | :--- |
| **`BUG-001`** | **S0** | 固件 / 配置 | 固件源码硬编码明文 Wi-Fi SSID/密码与内网 IP `10.21.11.76`；前端亦预置该内网示例 IP | `esp32/main/main.ino:10-17`<br>`static/index.html:2132` | `OPEN` |
| **`BUG-002`** | **S0** | 后端安全 | 写/删/状态切换/遥测上报接口及 WebSocket `/ws/devices` 均无认证授权与防重放校验 | `main.py:862-1303` | `OPEN` |
| **`BUG-003`** | **S0** | 数据仓库 | `lab_devices.db` 虽已写入 `.gitignore` 但历史已被 Git 跟踪（`git ls-files` 可见） | `.gitignore:26` | `OPEN` |
| **`BUG-004`** | **S1** | 后端遥测 / 测试 | `ingest_telemetry` 未将 `raw_data.current` 回填给 `TelemetryChannel.current`，`adapt()` 在解析 `raw_data` 前执行；`simulate_esp32.py` 显式传 `state` 且仅把 `current` 放在 `raw_data` 中，导致测试未覆盖电流阈值逻辑 | `main.py:238-252`<br>`main.py:389-399`<br>`simulate_esp32.py:45-55` | `OPEN` |
| **`BUG-005`** | **S1** | 后端架构 / QA | 全局模块级 `DATABASE_URL` 在导入时即创建引擎并建表，无法隔离注入测试 DB；`simulate_esp32.py` 运行会污染默认库且失败仍返回退出码 0 | `main.py:29-32,131-132`<br>`simulate_esp32.py:160-165` | `OPEN` |
| **`BUG-006`** | **S1** | 前端孪生视图 | 仅当 `isPositionEditMode` 为真时才读取持久化 `pos_x`/`pos_y`，锁定态回退固定 `FLOOR_ARCH_SLOTS` 坐标，且 1F/2F/3F 共用 `allRooms` 未做楼层隔离 | `static/index.html:2620-2650` | `OPEN` |
| **`BUG-007`** | **S1** | 前端遥测显示 | 设备无真实电流字段时，前端以硬编码常量 `1.85A`（运行）/ `0.05A`（待机）计算房间负荷与热力图，造成伪实时展示 | `static/index.html:2658-2661,3164-3190,3328` | `OPEN` |
| **`BUG-008`** | **S1** | 后端导出 | CSV 导出未防护公式注入（`=`, `+`, `-`, `@` 前缀单元格转义）；注：前端未转义 `innerHTML` 与 `res.ok` 校验已随 UI 重构修复 | `main.py:1339-1347` | `OPEN` |
| **`BUG-009`** | **S2** | 后端状态机 | 连接态（`device_last_seen` 内存字典）与业务状态混用，超时强行将 `IN_USE` 覆写为 `IDLE`；多进程下内存心跳状态不共享 | `main.py:290-335` | `OPEN` |
| **`BUG-010`** | **S2** | 打包构建 | PyInstaller 配置硬编码旧机器绝对路径 `E:\code\Lab_Device_System\...`，跨机构建需手工修改 | `SmartLabSystem.spec:5,8` | `OPEN` |
