"""
====================================================================
  ESP32 硬件电流传感器模拟测试脚本 (QA / Hardware Testing)
====================================================================
  测试目标：验证 FastAPI 通用遥测接收入口 `/api/v1/telemetry/ingest`
  场景：模拟 ESP32 电流传感器对设备 DEV-GC-002 实时上报电流及防抖过滤逻辑
====================================================================
"""

import json
import os
import sys
import time
import urllib.request
import urllib.error

# 开启 Windows 控制台 ANSI 转义色彩支持
if sys.platform == "win32":
    os.system("")
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass

# ------------------------------------------------------------------
# 配置项
# ------------------------------------------------------------------
BASE_URL = "http://127.0.0.1:8000/api/v1/telemetry/ingest"
DEVICE_CODE = "DEV-002"

# 终端 ANSI 彩色显示字符
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
CYAN = "\033[96m"
BOLD = "\033[1m"
RESET = "\033[0m"


def send_telemetry(device_code: str, state: str, current: float, operator: str = "ESP32_CURRENT_SENSOR"):
    """
    使用 Python 标准库 (urllib) 发送 HTTP POST 遥测请求
    无需安装第三方 requests 依赖，保证脚本开箱即用
    """
    payload = {
        "device_code": device_code,
        "state": state,
        "source_type": "ESP32_MQTT",
        "operator": operator,
        "raw_data": {
            "current": current,
            "voltage": 220.0,
            "unit": "A"
        }
    }

    headers = {"Content-Type": "application/json"}
    data = json.dumps(payload).encode("utf-8")

    req = urllib.request.Request(BASE_URL, data=data, headers=headers, method="POST")
    try:
        with urllib.request.urlopen(req, timeout=5) as response:
            res_body = response.read().decode("utf-8")
            return response.status, json.loads(res_body)
    except urllib.error.HTTPError as e:
        res_body = e.read().decode("utf-8")
        return e.code, json.loads(res_body) if res_body else {}
    except Exception as err:
        return 500, {"error": f"无法连接到后端 API 服务: {str(err)}"}


def print_header(title: str):
    print(f"\n{CYAN}{'='*68}{RESET}")
    print(f"{CYAN}{BOLD} [QA Hardware Test] {title} {RESET}")
    print(f"{CYAN}{'='*68}{RESET}")


def run_hardware_test_suite(device_code: str = DEVICE_CODE):
    print_header("ESP32 硬件电流传感器与防抖机制自动化测试套件")
    print(f"[*] 目标 API 接口 : {BASE_URL}")
    print(f"[*] 测试设备编号 : {device_code}\n")

    # 前置准备：模拟人工在 Web 端将设备初始化为待机 (IDLE)，解除可能的 FAULT 故障锁定
    print(f"[*] 前置准备: 正在解除测试设备可能存在的 FAULT 故障锁定...")
    reset_payload = {
        "device_code": device_code,
        "state": "IDLE",
        "source_type": "MANUAL_WEB",
        "operator": "QA_AUTO_RESET",
        "raw_data": {"action": "reset_for_testing"}
    }
    try:
        req = urllib.request.Request(
            BASE_URL,
            data=json.dumps(reset_payload).encode("utf-8"),
            headers={"Content-Type": "application/json"},
            method="POST"
        )
        with urllib.request.urlopen(req, timeout=3) as resp:
            pass
        print(f"{GREEN}[OK] 设备已重置为待机就绪 (IDLE) 状态{RESET}\n")
    except Exception as e:
        print(f"{YELLOW}[!] 无法重置设备状态 (可能为全新设备): {e}{RESET}\n")


    test_steps = [
        {
            "step": "步骤 1: 待机状态采样 (current: 0.05A)",
            "state": "IDLE",
            "current": 0.05,
            "expected_status": "IDLE",
            "desc": "设备处于待机微弱电流 (0.05A)，验证系统保持待机状态 (IDLE)。"
        },
        {
            "step": "步骤 2: 设备开机启动 (current: 2.35A)",
            "state": "IN_USE",
            "current": 2.35,
            "expected_status": "IN_USE",
            "desc": "模拟设备启动上电，电流突增至 2.35A，验证系统自动将状态切为运行中 (IN_USE)。"
        },
        {
            "step": "步骤 3: 运行中电流波动干扰 (current: 0.2A)",
            "state": "IN_USE",
            "current": 0.20,
            "expected_status": "IN_USE",
            "desc": "模拟运行中电流瞬时下探至 0.20A，验证防抖过滤规则阻断误关机，保持运行中 (IN_USE)。"
        },
        {
            "step": "步骤 4: 设备关机断电 (current: 0.0A)",
            "state": "IDLE",
            "current": 0.00,
            "expected_status": "IDLE",
            "desc": "设备完全关机切断电流 (0.00A)，验证系统自动将状态切回待机 (IDLE)。"
        }
    ]

    passed_count = 0

    for idx, item in enumerate(test_steps, 1):
        print(f"{BOLD}> [Step {idx}] {item['step']}{RESET}")
        print(f"  [说明] {item['desc']}")
        print(f"  [发送] state='{item['state']}', raw_data={{'current': {item['current']}A}}")

        status_code, res_json = send_telemetry(device_code, item['state'], item['current'])

        actual_status = res_json.get("new_status")
        is_pass = (status_code == 200) and (actual_status == item['expected_status'])

        if is_pass:
            passed_count += 1
            res_tag = f"{GREEN}{BOLD}[PASS]{RESET}"
        else:
            res_tag = f"{RED}{BOLD}[FAIL]{RESET}"

        print(f"  [响应] HTTP {status_code} | Body: {json.dumps(res_json, ensure_ascii=False)}")
        print(f"  [结果] 期望状态 = {item['expected_status']} | 实际状态 = {actual_status} -> {res_tag}\n")
        time.sleep(0.5)

    print(f"{CYAN}{'='*68}{RESET}")
    if passed_count == len(test_steps):
        print(f"{GREEN}{BOLD}>>> [SUCCESS] 所有 {len(test_steps)} 个测试步骤全部通过! ({passed_count}/{len(test_steps)} PASS){RESET}")
    else:
        print(f"{RED}{BOLD}>>> [FAILURE] 测试未完全通过! (通过率: {passed_count}/{len(test_steps)}){RESET}")
    print(f"{CYAN}{'='*68}{RESET}\n")


if __name__ == "__main__":
    target_code = DEVICE_CODE
    if len(sys.argv) > 1:
        arg = sys.argv[1]
        if arg == "--new":
            target_code = f"DEV-ESP32-NEW-{int(time.time()) % 1000:03d}"
            print(f"{YELLOW}[!] 开启【全新 ESP32 硬件连入】测试模式，测试设备编号: {target_code}{RESET}")
        else:
            target_code = arg
    run_hardware_test_suite(target_code)
