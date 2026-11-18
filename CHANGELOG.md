# Changelog · 版本演进与修复归档

本项目的重要功能里程碑与已完成的安全、逻辑、UI 修复均归档于此（遵循 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/) 精神）。当前未修复问题请查阅 [`docs/BUGS.md`](./docs/BUGS.md)。

---

## [Docs Governance & Liquid Glass UI] - 2026-11-18

### Changed
- **Apple Liquid Glass 界面与现代 Web 平台升级（`static/index.html`）**：
  - 引入晴空浅色液态玻璃（Daylight Frosted Liquid Glass）光学令牌：明亮流体渐变基底配合高饱和磨砂折射（`backdrop-filter: blur(30px) saturate(200%) brightness(1.05)`）、亮白棱边高光、同心胶囊圆角、浅色 `color-scheme` 声明与浅色半透明 SVG 建筑蓝图，并补齐 `prefers-reduced-transparency`、`forced-colors` 与窄屏响应式布局。
  - 4 个模态框升级为原生 `<dialog>`（`showModal()`、`closedby` 回退、`overscroll-behavior: contain` 与 `@starting-style` 动效），二级抽屉支持 `inert` 与焦点归还。
  - 消除全文件 8 处 `innerHTML = ''`（改用 `replaceChildren()`）、3 处裸 `::-webkit-scrollbar`（改用标准 `scrollbar-color`/`scrollbar-width` + `@supports` 回退）及全部动态 `innerHTML` XSS 注入点，并统一通过 `fetchJson` 校验 `res.ok`。
- **Docs-as-Code 体系重构**：基于 Code-First 代码审计重建 `README.md`、`AGENTS.md`、`docs/STATUS.md`、`docs/BUGS.md`、`docs/adr/`、`docs/specs/`、`docs/guides/` 与 `CHANGELOG.md`。
- **旧文档无损归档**：将原 `DEVELOPMENT_SUMMARY.md` 与 `2026-09-06` 协作套件（`WORKLOG.md`、`docs/BACKLOG.md`、`docs/TEAM.md`、`docs/coordination/`、`docs/progress/`）原样移存至 `docs/_archive/` 并加入 `.gitignore`。
- **状态纠偏**：移除原总结文档中与当前代码不符的“99% 生产就绪”“哈希链已上线”“MQTT/Modbus 已实现”等失真声明，以 `Truth Matrix (L1~L2)` 重新定级。

---

## [2026-09-06] - 协作基线盘点 (`bd98ff1` 之后)

### Added
- 完成项目首次代码只读盘点，识别出明文 Wi-Fi 凭据、无鉴权、模拟脚本未覆盖电流阈值、前端楼层/坐标/热力展示偏差及无正式测试套件等 P0 风险（记录已归档至 `docs/_archive/coordination-v1/`，活跃缺陷已转入 [`docs/BUGS.md`](./docs/BUGS.md)）。

---

## [v2.5-proto / bd98ff1] - 2026-08-25

### Added
- **原生 SVG 建筑蓝图与视口交互**：在 `static/index.html` 剥离静态位图与 Leaflet 依赖，上线 1F/2F/3F 矢量蓝图、滚轮指针缩放与画布平移拖拽。
- **单状态源联动与在线编辑**：引入 `selectedLaboratoryId` 联动底图、底部负荷栏与二级设备抽屉；新增房间位置拖拽持久化（`PUT /api/v1/rooms/{room_name}/position`）与重命名级联更新设备绑定（`PUT /api/v1/rooms/{old_room_name}`）。
- **mDNS 零配置局域网寻址**：后端 `lifespan` 集成 `zeroconf` 广播 `smartlab.local:8000`；固件 `esp32/main/main.ino` 集成 `<ESPmDNS.h>` 自动探测服务端 IP 并保留备用 IP 降级。
- **自愈启动与环境标准化**：新增 `requirements.txt` 与支持自动建构 `.venv`、安装依赖的 `run.bat`，以及 GitHub 同步脚本 `sync_to_github.bat`。

### Fixed（历史排障归档）
- **FastAPI 生命周期与防缓存**：`main.py` 从弃用的 `@app.on_event` 迁移至 `@asynccontextmanager` `lifespan`；根路由改用 `HTMLResponse` 强制注入 `No-Cache` 响应头消除浏览器旧页面缓存。
- **Uvicorn Windows 进程残留**：改为单进程直启 `uvicorn.run(app, ...)`，解决 `Ctrl+C` 后子进程占用 `8000` 端口引发的 `[WinError 10013]` 问题，并在 `main.py` 集成延迟自动打开浏览器线程。
- **PyInstaller 资源路径兼容**：`main.py` 通过 `getattr(sys, '_MEIPASS', ...)` 动态定位 `static` 目录。
- **前端脚本与交互修复**：清理 `static/index.html` 顶部重复变量声明恢复脚本执行；新增房间右上角【改名】标签并阻止双击事件冒泡；迁移 1F/2F/3F 切换栏至视口工具栏。
- **CMD 脚本编码兼容**：`run.bat` 改用原生 GBK 编码与扁平 `goto` 控制流并锁定 `.venv\Scripts\python.exe`，解决中文注释导致的词法截断与 `ModuleNotFoundError`。
