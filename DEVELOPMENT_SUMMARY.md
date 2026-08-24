# 智能实验室数字孪生与不可篡改履历系统 · 开发与重构总结文档

> **项目名称**：智能实验室数字孪生与不可篡改履历系统 (Smart Lab Digital Twin System)  
> **远程仓库**：[https://github.com/zl1713033678/Lab_Device_System](https://github.com/zl1713033678/Lab_Device_System)  
> **文档版本**：v2.1-ProductionReady  
> **更新日期**：2026-08-24  

---

## 一、 今日核心改造与功能里程碑

### 1. 平面底图架构升级：原生动态互动 SVG 1F 建筑蓝图
- **轻量化重构**：完全剥离了原有的第三方 Leaflet GIS 依赖与静态位图，采用纯原生 **HTML5 SVG 矢量渲染引擎**。
- **1F 一层实验楼工程蓝图**：
  - **建筑外围结构**：包含外墙厚度双线轮廓（CAD Wall Thickness）与建筑混凝土承重柱（Columns）。
  - **人流动线与大堂**：绘制了正南侧主门厅入口（`🏢 1F 实验大楼正门大堂`）、人脸门禁安防闸机通道与贯穿全楼的中央主动线走廊（`1F Central Main Corridor`）。
  - **服务核心筒**：标准绘制 `🛗 双联智能电梯间`、`🪜 消防安全疏散梯`、`⚡ 强电配电室/UPS动力机房` 与 `🚻 公共卫生间`。
  - **实验室标准功能分区**：规划了西北翼（101 智能硬件）、正北区（102 物联网传感）、东北翼（103 边缘计算）、西南区（203 光电传感）、东南区（302 芯片高频测试）等预设槽位。

### 2. 状态驱动与双向联动机制（Single Source of Truth）
- **唯一状态源管控**：全局仅维护统一的 `selectedLaboratoryId` 受控状态，杜绝状态孤岛。
- **无缝双向联动**：
  - **地图 -> 列表**：点击 SVG 平面图中的任意实验室，自动高亮右侧对应卡片并平滑滚动到视野居中，同时弹出二级设备详情抽屉。
  - **列表 -> 地图**：点击右侧实验室卡片，SVG 画布自动平滑平移至该房间中心，并同步触发呼吸微光高亮。
  - **状态呼吸微光**：房间状态根据内部设备运行情况自动变色（🟢 运行中·翡翠绿、⚪ 待机·琥珀黄、🔴 故障告警·深红呼吸脉冲）。

### 3. 底图可视化编辑与设备级联自动绑定
- **坐标自由微调**：点击【`🔓 调整位置`】后，可在 SVG 画布内任意拖拽实验室方格至指定房间槽位，松开后自动调用 `PUT /api/v1/rooms/{name}/position` 将百分比坐标持久化至 SQLite 数据库。
- **实验室名称在线修改**：
  - 支持在底图上**双击房间方格**或点击【`✏️ 改名`】标签直接重命名；
  - 重命名提交后，后端自动级联更新 `rooms` 表与该房间下**所有物理设备的归属绑定关系**，前端底图、卡片及资产列表毫秒级无感同步。

### 4. 视口控制与悬停交互（Viewport Engine）
- **视口缩放与平移**：支持鼠标滚轮以鼠标指针为中心无级缩放（0.4x ~ 3.0x），支持鼠标按住画布空白区域平移拖拽。
- **快捷工具栏**：内置 `➕ 放大`、`➖ 缩小`、`🎯 复位` 与 `🔓 调整位置` 快捷按钮。
- **轻量级 Tooltip**：鼠标悬停在房间上方即时显示设备总数、运行/待机/故障各状态统计，且悬停状态与选中状态完全解耦。

---

## 二、 关键问题排查与修复记录

| 序号 | 遇到的问题现象 | 根因深度剖析 | 最终解决方案 |
| :--- | :--- | :--- | :--- |
| **1** | 双击 `.exe` 或 `run.bat` 后界面未生效 | 旧版 `SmartLabSystem.exe` 在后台常驻占用了 8000 端口；且单文件打包未重新编译。 | 终止后台占用进程，适配 `_MEIPASS` 路径并使用 PyInstaller 重新编译。 |
| **2** | 启动后终端显示日志，界面感觉“卡住了” | Uvicorn 属于阻塞型服务进程，此前未在 Python 端自动打开浏览器。 | 在 [main.py](file:///e:/code/Lab_Device_System/main.py) 增加延迟自动调用系统默认浏览器的后台线程。 |
| **3** | 前端按钮全部失效、数据显示为 0 | `<script>` 顶部变量声明重复，触发 JS 语法错误中止了整段脚本解析。 | 清理重复声明，经 Node.js 全量语法校验确保 0 报错，全面恢复全站交互。 |
| **4** | Git 远程同步挂起超时 | 无交互子终端环境下无法弹出 GitHub 登录认证界面。 | 编写一键同步脚本 [sync_to_github.bat](file:///e:/code/Lab_Device_System/sync_to_github.bat)，通过前台交互顺利完成推送。 |
| **5** | 新电脑 Clone 缺失运行依赖与环境 | 仓库缺少 requirements 规范，新机运行可能缺少第三方库。 | 引入 [requirements.txt](file:///e:/code/Lab_Device_System/requirements.txt) 并升级 [run.bat](file:///e:/code/Lab_Device_System/run.bat) 支持开箱自动创建 venv 与补齐依赖。 |

---

## 三、 换新电脑 Clone 后的全自动补全与自愈机制

针对更换电脑后通过 `git clone` 运行项目的场景，系统已建立 **四层全自动补全闭环**：

```
                 [新电脑 git clone 仓库]
                           │
                           ▼
                 [双击运行 run.bat]
                           │
         ┌─────────────────┴─────────────────┐
         ▼                                   ▼
 [检测到缺失 .venv 虚拟环境]          [已存在 .venv 环境]
         │                                   │
 1. 自动执行 python -m venv .venv            │
 2. 自动 pip install -r requirements.txt      │
 3. 依赖全自动补齐完毕                       │
         └─────────────────┬─────────────────┘
                           ▼
                  [启动 main.py 后端]
                           │
         ┌─────────────────┴─────────────────┐
         ▼                                   ▼
 [检测到缺失 lab_devices.db 数据库]    [已存在数据库]
         │                                   │
 1. 自动建表 (init_db)                       │
 2. 自动预置 1F 房间与测试设备               │
         └─────────────────┬─────────────────┘
                           ▼
            [后台自动唤起系统浏览器访问]
               http://localhost:8000
```

1. **Python 运行环境与依赖自动补齐**：
   - 依赖清单 [requirements.txt](file:///e:/code/Lab_Device_System/requirements.txt) 规范了系统必需库；
   - 脚本 [run.bat](file:///e:/code/Lab_Device_System/run.bat) 具备环境自愈能力，新电脑首次运行会自动创建 `.venv` 并静默安装所有运行依赖。
2. **SQLite 数据库自愈初始化**：
   - [main.py](file:///e:/code/Lab_Device_System/main.py) 内置数据库自愈逻辑，首次启动自动创建 `lab_devices.db` 并预置 1F 房间与测试设备，无需手动导入 SQL 文件。
3. **前端资源即开即用**：
   - 静态资源与动态 SVG 平面图完整托管在 Git 仓库中，clone 即可直接加载。
4. **服务与浏览器全自动联动**：
   - 服务启动成功后，无需人工输入网址，后台自动打开默认浏览器。

---

## 四、 系统运行与启动指南

### 1. 独立程序运行（无需 Python 环境）
直接双击运行：
```
E:\code\Lab_Device_System\dist\SmartLabSystem.exe
```
> 启动后系统将自动唤起默认浏览器并打开 `http://localhost:8000`。

### 2. 源码脚本运行（支持新电脑全自动初始化）
双击运行：
```
E:\code\Lab_Device_System\run.bat
```
或在终端中执行：
```powershell
.\.venv\Scripts\python.exe main.py
```

### 3. 代码一键同步到 GitHub
双击运行根目录下的 [sync_to_github.bat](file:///e:/code/Lab_Device_System/sync_to_github.bat) 即可自动提交最新变更并推送到 GitHub 远程仓库。

---

## 五、 核心代码资产索引

- **后端主服务**：[main.py](file:///e:/code/Lab_Device_System/main.py)（FastAPI 路由、WebSocket 广播、SQLite 数据库自愈、PyInstaller 兼容、浏览器自动唤起）
- **前端页面与 SVG 平面图**：[static/index.html](file:///e:/code/Lab_Device_System/static/index.html)（SVG 矢量蓝图、双向联动控制器、抽屉面板、审计报表、名称重命名与绑定）
- **依赖清单文件**：[requirements.txt](file:///e:/code/Lab_Device_System/requirements.txt)（Python 第三方依赖标准定义）
- **硬件采样端代码**：[esp32/main/main.ino](file:///e:/code/Lab_Device_System/esp32/main/main.ino)（ESP32 固件逻辑）
- **硬件模拟端脚本**：[simulate_esp32.py](file:///e:/code/Lab_Device_System/simulate_esp32.py)（本地多设备遥测模拟器）
- **打包配置文件**：[SmartLabSystem.spec](file:///e:/code/Lab_Device_System/SmartLabSystem.spec)
- **环境自愈与启动脚本**：[run.bat](file:///e:/code/Lab_Device_System/run.bat)
- **GitHub 一键同步脚本**：[sync_to_github.bat](file:///e:/code/Lab_Device_System/sync_to_github.bat)
