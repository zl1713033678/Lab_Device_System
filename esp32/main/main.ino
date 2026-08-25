#include <Wire.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= 1. 配置参数 (根据实际修改) =================
const char* WIFI_SSID = "木工所实验中心";
const char* WIFI_PASS = "caf888888";

// 局域网服务域名 (mDNS 自动寻址，电脑 IP 随意变动均可自动捕获)
const char* MDNS_HOST = "smartlab"; 

// 备用降级静态 IP (当个别路由器禁止 mDNS 组播时自动降级生效)
const char* FALLBACK_SERVER_IP = "10.21.11.76";
const int SERVER_PORT = 8000;

// 设备物理唯一编号 (与网页实验室空间绑定，永不失效)
const char* DEVICE_CODE = "DEV-ESP32-001";

// 引脚定义
#define ACS712_PIN  34  // ACS712 连接 ESP32 GPIO34 (ADC)
#define OLED_SCL    22  // OLED SCL 接 GPIO22
#define OLED_SDA    21  // OLED SDA 接 GPIO21

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// =============================================================

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 动态解析获得的服务端 URL
String currentServerUrl = "";
IPAddress resolvedServerIP;
unsigned long lastMDNSSearch = 0;

// 辅助函数：清屏并在 OLED 展现 4 行文字
void showOLED(String line1, String line2, String line3, String line4) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);   display.println(line1);
  display.setCursor(0, 16);  display.println(line2);
  display.setCursor(0, 32);  display.println(line3);
  display.setCursor(0, 48);  display.println(line4);
  
  display.display();
}

// 自动探测并解析电脑服务端 IP (mDNS 自动寻址)
bool updateServerEndpoint(bool forceSearch) {
  if (!forceSearch && currentServerUrl.length() > 0 && (millis() - lastMDNSSearch < 60000)) {
    return true;
  }

  Serial.println(F("[mDNS] 正在局域网搜索 smartlab.local 电脑端..."));
  IPAddress hostIp = MDNS.queryHost(MDNS_HOST, 1500);

  if (hostIp != INADDR_NONE && hostIp.toString() != "0.0.0.0") {
    resolvedServerIP = hostIp;
    currentServerUrl = "http://" + resolvedServerIP.toString() + ":" + String(SERVER_PORT) + "/api/v1/telemetry/ingest";
    Serial.printf("[mDNS] 成功捕获电脑服务端最新 IP: %s\n", resolvedServerIP.toString().c_str());
    lastMDNSSearch = millis();
    return true;
  } else {
    // 降级回退到备用默认 IP
    currentServerUrl = "http://" + String(FALLBACK_SERVER_IP) + ":" + String(SERVER_PORT) + "/api/v1/telemetry/ingest";
    Serial.printf("[mDNS] 未搜索到 smartlab.local，使用备用端点: %s\n", currentServerUrl.c_str());
    lastMDNSSearch = millis();
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  
  // 1. 配置 ESP32 ADC 衰减 (允许测量 0 ~ 3.3V，防止 2.5V 饱和溢出)
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  pinMode(ACS712_PIN, INPUT);

  // 2. 初始化 I2C 与 OLED 显示屏
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C 为 OLED 默认 I2C 地址
    Serial.println(F("SSD1306 OLED 初始化失败！"));
    for (;;); // 卡住提示检查接线
  }
  
  showOLED("Smart Lab Twin", "Initializing...", "Device: " + String(DEVICE_CODE), "Connecting WiFi...");

  // 3. 连接 Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    retry++;
    showOLED("WiFi Connecting...", "SSID: " + String(WIFI_SSID), "Attempting: " + String(retry) + "s", "Please wait...");
    Serial.print(".");
  }

  String localIP = WiFi.localIP().toString();
  Serial.println("\nWiFi 连接成功! IP: " + localIP);
  
  // 4. 启动 mDNS 响应器并首次解析服务端电脑
  if (MDNS.begin("esp32-sensor")) {
    Serial.println(F("[mDNS] 客户端 mDNS 模块启动就绪"));
  }
  
  showOLED("WiFi Connected!", "IP: " + localIP, "Searching Server", "smartlab.local...");
  updateServerEndpoint(true);

  showOLED("Smart Lab Twin", "IP: " + localIP, "Server: Ready", "System Running!");
  delay(1200);
}

void loop() {
  // 1. ACS712 交流采样：连续读取 100 次，使用方差有效值算法防止单点毛刺导致 3995 溢出
  long sum = 0;
  int samples[100];
  
  for (int i = 0; i < 100; i++) {
    samples[i] = analogRead(ACS712_PIN);
    sum += samples[i];
    delayMicroseconds(1000); // 采样间隔 1ms (捕捉 50Hz 1~2个交流周期)
  }

  int avg = sum / 100;
  long sq_sum = 0;
  for (int i = 0; i < 100; i++) {
    long diff_val = samples[i] - avg;
    sq_sum += diff_val * diff_val;
  }

  // 换算为有效波幅 Peak-to-Peak (RMS * 2.83)
  int raw_diff = (int)(sqrt((double)sq_sum / 100.0) * 2.83);

  // 2. 噪声死区过滤 (Deadband)：ESP32 ADC 底噪与环境干扰一般在 30~50 左右
  const int NOISE_FLOOR = 30; 
  int diff = (raw_diff > NOISE_FLOOR) ? (raw_diff - NOISE_FLOOR) : 0;
  
  // 3. 开关机阈值判定 (扣除底噪后 diff > 40 判定为设备运行中)
  String status_str = (diff > 40) ? "IN_USE" : "IDLE";
  String status_display = (diff > 40) ? "RUNNING" : "STANDBY";

  // 4. 换算安培电流 (ACS712 20A 约 0.008A/ADC位)
  float current_amp = (diff > 0) ? (diff * 0.008f) : 0.0f;
  float voltage_val = 220.0f;

  // 打印调试信息到串口
  Serial.printf("ADC 均值: %d | 采样幅值 raw_diff: %d | 净差值 diff: %d | 电流: %.2fA | 状态: %s\n", avg, raw_diff, diff, current_amp, status_display.c_str());

  // 5. 刷新 OLED 显示屏
  showOLED(
    "Dev: " + String(DEVICE_CODE),
    "Status: " + status_display,
    "Curr: " + String(current_amp, 2) + "A (D:" + String(diff) + ")",
    "IP: " + WiFi.localIP().toString()
  );

  // 6. 通过 HTTP POST 发送数据给电脑端 FastAPI
  if (WiFi.status() == WL_CONNECTED) {
    if (currentServerUrl.length() == 0) {
      updateServerEndpoint(false);
    }

    HTTPClient http;
    http.begin(currentServerUrl);
    http.addHeader("Content-Type", "application/json");

    // 构建 JSON 报文
    StaticJsonDocument<256> doc;
    doc["device_code"] = DEVICE_CODE;
    doc["status"] = status_str;
    doc["current"] = current_amp;
    doc["voltage"] = voltage_val;
    doc["source_type"] = "ESP32_ACS712";
    
    JsonObject rawDataObj = doc.createNestedObject("raw_data");
    rawDataObj["adc_diff"] = diff;
    rawDataObj["current"] = current_amp;
    rawDataObj["voltage"] = voltage_val;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.printf("上报成功 | 状态: %s | 差值: %d | HTTP: %d\n", status_str.c_str(), diff, httpResponseCode);
    } else {
      Serial.printf("上报失败, 错误代码: %s，触发 mDNS 重新探测电脑...\n", http.errorToString(httpResponseCode).c_str());
      // 连续失败时触发 mDNS 重新探测电脑新 IP
      updateServerEndpoint(true);
    }
    http.end(); // 释放连接
  }

  delay(1500); // 每 1.5 秒检测与上报一次
}