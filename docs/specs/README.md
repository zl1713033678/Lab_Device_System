# Lab_Device_System · 特性孵化器与设计台账 (`docs/specs/`)

> **强制维护契约（Mandatory Maintenance Protocol）**
> 1. **只记思路、按需孵化**：一特性一独立 `SPEC-XXX.md` 文件。新想法提出时先在此沉淀业务场景、架构方案与接口设计，**未经用户明确指令严禁编写或改动业务代码**。
> 2. **完成状态与时间戳强制回写**：某项特性（或子阶段）获准开发并通过验收后，**必须同步更新本索引表及对应 Spec 头部**，回填精确时间戳（`YYYY-MM-DD HH:mm:ss+08:00`）与关联代码，并刷新 [`docs/STATUS.md`](../STATUS.md)。

---

## 特性孵化总览索引表 (Spec Backlog Index)

| 编号 | 特性文档 | 核心目标摘要 | 提出日期 | 当前状态 | 完成时间戳 | 关联代码/备注 |
| :--- | :--- | :--- | :---: | :--- | :--- | :--- |
| **`SPEC-001`** | [`SPEC-001-security-auth-and-credential-hygiene.md`](./SPEC-001-security-auth-and-credential-hygiene.md) | 凭据外部化、管理端认证授权、设备签名防重放与 CI 门禁 | `2026-09-06` | `📝 规划中 (PLANNED)` | *待落地后回填* | 对应原 `SEC-001~004`、`OPS-001` |
| **`SPEC-002`** | [`SPEC-002-cryptographic-audit-hash-chain.md`](./SPEC-002-cryptographic-audit-hash-chain.md) | 履历表 SHA-256 链式哈希存证、验签接口与数据备份迁移 | `2026-08-25` | `📝 规划中 (PLANNED)` | *待落地后回填* | 对应原 `DATA-001~003` |
| **`SPEC-003`** | [`SPEC-003-twin-floor-isolation-and-bidirectional-iot.md`](./SPEC-003-twin-floor-isolation-and-bidirectional-iot.md) | 楼层空间隔离、真实遥测展示、单体拆分与 MQTT 双向控制 | `2026-08-25` | `📝 规划中 (PLANNED)` | *待落地后回填* | 对应原 `BE-001~002`、`FE-001~004`、`IOT-001~003`、`ARCH-002` |
