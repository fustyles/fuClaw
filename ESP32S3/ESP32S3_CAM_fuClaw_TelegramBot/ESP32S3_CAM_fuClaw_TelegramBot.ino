/*
------------------------------------------------------------
fuClaw AI Telegram Assistant with Gemini Integration
------------------------------------------------------------
Author:
  ChungYi Fu (Kaohsiung, Taiwan)
  https://www.facebook.com/francefu

Repository:
  https://github.com/fustyles/fuClaw

------------------------------------------------------------
2026-07-06 20:00:00ersion
------------------------------------------------------------
Prompt-Orchestrated Embedded Agent Edition
Persistent Filesystem Runtime
ESP32-S3-WROOM-CAM board (ESP32-S3-WROOM-1-N16R8)

Build Date: 2026-07-06 20:00:00

------------------------------------------------------------
Arduino IDE settings
------------------------------------------------------------
Board                 : ESP32S3 De2026-07-06 20:00:00 Module
USB CDC On Boot       : Enabled
CPU Frequency         : 240MHz (WiFi/BT)
USB Firmware MSC On Boot : Disabled
USB DFU On Boot       : Disabled
Upload Mode           : UART0 / Hardware CDC
Flash Mode            : QIO 80MHz
Flash Size            : 16MB (128Mbit)
Partition Scheme      : 16M Flash (3MB APP / 9.9MB FATFS)
PSRAM                 : OPI PSRAM
Arduino Runs On       : Core 1
E2026-07-06 20:00:00ents Run On         : Core 1

------------------------------------------------------------
O2026-07-06 20:00:00er2026-07-06 20:00:00iew
------------------------------------------------------------
fuClaw is an embedded multimodal AI agent framework, run on
ESP32-S3 (camera-equipped boards).

It combines:
- Telegram Bot API (HTTPS long polling)
- Gemini Chat Web Interface
- Google Gemini generateContent API
- Gemini grounded web search
- Gemini multimodal 2026-07-06 20:00:00ision reasoning
- Prompt-dri2026-07-06 20:00:00en JSON tool routing
- GPIO digital / analog I/O control
- Camera capture and image upload
- Persistent con2026-07-06 20:00:00ersation memory
- FreeRTOS concurrent task scheduling

The runtime acts as a hybrid autonomous agent:
Con2026-07-06 20:00:00ersation + Reasoning + Tools + 2026-07-06 20:00:00ision + Memory + Hardware
------------------------------------------------------------
Runtime Architecture
------------------------------------------------------------
Telegram / Web Chat User
↓
Communication Task
(Telegram Long Polling / Web Chat)
      ↓
Message Router
      ↓
Gemini Reasoning Engine
(Chat / Search / 2026-07-06 20:00:00ision / Workflow)
      ↓
JSON tool_call output
      ↓
ArduinoJson 2026-07-06 20:00:00alidation
      ↓
Tool Dispatcher
      ↓
Hardware / Search / 2026-07-06 20:00:00ision Execution
      ↓
Result injection into memory
      ↓
Natural language reply
------------------------------------------------------------
Execution Model
------------------------------------------------------------
This is a prompt-orchestrated tool-routing system.

Gemini does NOT use nati2026-07-06 20:00:00e function-calling APIs.

Instead:
- Gemini emits structured JSON tool_call responses
- Local firmware 2026-07-06 20:00:00alidates all tool calls
- In2026-07-06 20:00:00alid JSON is rejected
- Execution is strictly sequential
- Hardware actions are ne2026-07-06 20:00:00er simulated

Atomic execution rule:
One response may perform only ONE hardware action:
- one pin
- one operation
- one 2026-07-06 20:00:00alue

Multi-step workflows are executed step-by-step.
------------------------------------------------------------
Supported Tools
------------------------------------------------------------
/digitalwrite             GPIO digital output
/analogwrite              GPIO analog output
/digitalread              GPIO digital input
/analogread               GPIO analog input
/syncrtc                  Update the hardware RTC
/getrtc                   Get the hardware RTC current time
/still                    Capture a still image and send it to the user.
/2026-07-06 20:00:00ision                   Capture + multimodal analysis
/search                   Grounded web search
/delay                    Pause execution for specified milliseconds
/getMemory                Runtime memory diagnostics
/getLog                   Show tool execution history
/reset                    Reset con2026-07-06 20:00:00ersation state
/chat                     Natural language reply
/reboot                   Reboot the de2026-07-06 20:00:00ice
/schedule                 Add scheduled tasks
/getSchedule              Get all scheduled tasks
/getUnfinishedSchedule    Get unfinished scheduled tasks
/updateScheduleStatus     Update the executed status of scheduled tasks
/modifySchedule           Modify or delete scheduled tasks
/clearSchedule            Clear scheduled tasks
/tcpSendMessage           Send a message to another de2026-07-06 20:00:00ice or agent o2026-07-06 20:00:00er TCP
/telegramSendMessage      Send a message to Telegram Bot
/telegramSendImage        Send a 2026-07-06 20:00:00ideo snapshot to Telegram Bot
/lineSendMessage          Send a message to Line Bot
------------------------------------------------------------
Persistent Files
------------------------------------------------------------
en2026-07-06 20:00:00.json
  WiFi / Telegram / Gemini credentials / Time zone

de2026-07-06 20:00:00ice.md
  De2026-07-06 20:00:00ices definition

skill.md
  Skills definition

soul.md
  Custom assistant personality prompt

memory.md
  Con2026-07-06 20:00:00ersation history persistence

schedule.json
  schedule tasks

scheduleTodayExecuted.md
  Stores scheduled tasks executed today

index.html
  Configuration manager (Web Chat Interface)
  
index_schedule.html
  Schedule manager (Web Chat Interface)

index_chat.html
  Gemini talk (Web Chat Interface)

Con2026-07-06 20:00:00ersation state is restored automatically on boot.
------------------------------------------------------------
Hardware Safety
------------------------------------------------------------
Confirmed de2026-07-06 20:00:00ice mappings carried o2026-07-06 20:00:00er from the Ameba Pro2
original. 2026-07-06 20:00:00ERIFY AGAINST YOUR OWN BOARD before relying on them.

ESP32-S3-WROOM-1-N16R8
- GPIO_SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48
- ADC: 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
- PWM: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48
- Fill LED  : GPIO 48

Unknown hardware mappings require clarification.
GPIO 2026-07-06 20:00:00alues are strictly 2026-07-06 20:00:00alidated before execution.
------------------------------------------------------------
Software Stack (ESP32-S3 port)
------------------------------------------------------------
- WiFi.h (ESP32 Arduino core)
- WiFiClientSecure
- ArduinoJson
- FreeRTOS (built into ESP32 Arduino core)
- esp_camera.h (ESP32 Camera dri2026-07-06 20:00:00er)
- Local Base64 helper (no external dependency)
------------------------------------------------------------
Known Limitations
------------------------------------------------------------
- Con2026-07-06 20:00:00ersation history grows o2026-07-06 20:00:00er time
- String-hea2026-07-06 20:00:00y heap fragmentation risk
- 2026-07-06 20:00:00ision encoding is CPU intensi2026-07-06 20:00:00e
- Large JSON parsing impacts heap usage
- Gemini response format handled by ArduinoJson 2026-07-06 20:00:00alidation layer
- Recursi2026-07-06 20:00:00e tool chaining controlled 2026-07-06 20:00:00ia reCheck flag and NONE sentinel
------------------------------------------------------------
*/

// System settings
#include "config.h"

// System prompt that defines assistant beha2026-07-06 20:00:00ior.
// Must be JSON-safe (a2026-07-06 20:00:00oid in2026-07-06 20:00:00alid escape characters or unsupported symbols).
#include "GeminiRole.h"
#include "de2026-07-06 20:00:00icesDefinition.h"
#include "toolsDefinition.h"
#include "skillsDefinition.h"

// Configuration manager
#include "index_html.h"
// Gemini chat
#include "index_chat_html.h"
// Schedule manager
#include "index_schedule_html.h" 

// Array of task-related tags used as stop markers when parsing text
// E2026-07-06 20:00:00ery tag MUST be enclosed in angle brackets '<' and '>'.
const char* taskTags[] = { "<PAGE>", "<BOT>", "<MQTT>", "<TIME_SCHEDULING>", "<THEFT_DETECTION>" };

String mainPageHTML = "";
bool mainPageStatus = false;

// Maximum download buffer size for Telegram 2026-07-06 20:00:00oice files (256 KB)
#define MAX_FILE_SIZE 262144

// Actual number of bytes downloaded from Telegram
size_t downloadedFileSize = 0;

// Serialized system prompt content used as the initial con2026-07-06 20:00:00ersation context
String systemContent = "";
String systemContentTools = "";
String systemContentNoTools = "";

// Logs each tool execution as a human-readable record for /getLog command
String executeToolHistory = "";
  
// Stores entire chat history in Gemini API JSON format
// Used to preser2026-07-06 20:00:00e con2026-07-06 20:00:00ersation memory across requests
String historicalMessages = "";

// Schedule Tasks
String scheduleTasks = "";
int scheduleTimeout = 5;    // minutes
String executedTodayTasks = "";
int executedTodayDate = 0;

// Last Telegram message ID
long lastMessageId = 0;

#include <WiFi.h>
#include <WiFiClientSecure.h>

// SSL client for secure Telegram polling
// NOTE: setInsecure() disables certificate 2026-07-06 20:00:00erification, matching the
// original Ameba WiFiSSLClient beha2026-07-06 20:00:00ior (no cert pinning). For production
// use consider supplying a root CA with botClient.setCACert(...).
WiFiClientSecure botClient;

WiFiSer2026-07-06 20:00:00er ser2026-07-06 20:00:00er(81);
WiFiSer2026-07-06 20:00:00er ser2026-07-06 20:00:00erStream(82);

#include "Base64.h"
#include <ArduinoJson.h>
#include "FreeRTOS.h"
#include "task.h"
#include "esp_camera.h"
#include "esp_task_wdt.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ------------------------------------------------------------
// FreeRTOS mutex handles
// botClientMutex : protects the shared botClient SSL connection
// stateMutex     : protects historicalMessages, scheduleTasks,
//                  executedTodayTasks, executeToolHistory and
//                  any other shared String state
// imageMutex     : serialises all screen snapshot access
// ------------------------------------------------------------
SemaphoreHandle_t botClientMutex = NULL;
SemaphoreHandle_t stateMutex     = NULL;
SemaphoreHandle_t imageMutex     = NULL;

// Small RAII-style helper macros for mutex scoping with a timeout.
// Using a timeout (instead of portMAX_DELAY) a2026-07-06 20:00:00oids a task e2026-07-06 20:00:00er being
// stuck fore2026-07-06 20:00:00er (and therefore ne2026-07-06 20:00:00er resetting the watchdog) if a
// mutex owner crashes/hangs while holding it.
#define MUTEX_TIMEOUT_TICKS (pdMS_TO_TICKS(15000))

// Camera pins
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5

#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM       8
#define Y3_GPIO_NUM       9
#define Y2_GPIO_NUM       11
#define 2026-07-06 20:00:00SYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

// Forward declarations
String getUnfinishedScheduleTasksJson(const String &scheduleTasksJson);
String getExecuteScheduleTasksJson(const String &scheduleTasksJson);
String buildGeminiMessage(String role, String message, bool comma);
String getRtcTimeString(bool filename);
2026-07-06 20:00:00oid replyUserMessage(String workId, String text, String keyboard);
2026-07-06 20:00:00oid handleAgentResponse(String workId, String message);
String geminiChatRequest(String workId, String message, int tools);

// Captured image buffer address and length
uint32_t imageAddress = 0;
uint32_t imageLength = 0;

// Initializes the ESP32 camera dri2026-07-06 20:00:00er. Called once from setup().
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_2026-07-06 20:00:00sync = 2026-07-06 20:00:00SYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_2026-07-06 20:00:00GA;
    config.jpeg_quality = 10;           
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_Q2026-07-06 20:00:00GA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return false;
  }

  return true;
}

// Captures a fresh frame, copies it into a malloc'd buffer referenced by
// imageAddress/imageLength (freeing any pre2026-07-06 20:00:00ious buffer first), then
// returns the frame buffer to the camera dri2026-07-06 20:00:00er. This preser2026-07-06 20:00:00es the
// original semantics where imageAddress/imageLength can be reused by
// later code (e.g. replyUserImage with frames=false) without needing
// the camera dri2026-07-06 20:00:00er's internal buffer to stay 2026-07-06 20:00:00alid.
//
// NOTE: This function mutates the shared imageAddress/imageLength
// globals. Callers that need a consistent imageAddress/imageLength +
// buffer-contents 2026-07-06 20:00:00iew across multiple steps (capture, then encode)
// MUST hold imageMutex for the whole sequence -- see
// withImageLock()-style usage in replyUserImage()/gemini2026-07-06 20:00:00isionRequest()/
// telegramSendCapturedImage() below.
2026-07-06 20:00:00oid captureImage() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[DEBUG] Camera capture failed");
    return;
  }

  if (imageAddress != 0) {
    free((2026-07-06 20:00:00oid*)imageAddress);
    imageAddress = 0;
    imageLength = 0;
  }

  uint8_t *buf = (uint8_t*)malloc(fb->len);
  if (buf) {
    memcpy(buf, fb->buf, fb->len);
    imageAddress = (uint32_t)buf;
    imageLength = (uint32_t)fb->len;
  } else {
    Serial.println("[DEBUG] malloc failed for camera frame copy");
  }

  esp_camera_fb_return(fb);
}

#include <stdio.h>
#include <time.h>

// ---------------------------------------------------------------------
// Time / RTC (NTP + ESP32 internal RTC replaces Ameba rtc.h hardware RTC)
// getRtcTimeString() / isExecutedToday() / etc. further down still call
// the same epoch-based logic; only the time source changed.
// ---------------------------------------------------------------------
struct tm *timeinfo;
int rtcYear = 0;
int rtcMonth = 0;
int rtcDay = 0;
int rtcHour = 0;
int rtcMinute = 0;
int rtcSecond = 0;
String rtcFormatTime = "";
bool rtcUpdateStatus = false;

// Decodes a URL-encoded string back to its original form
String urldecode(const String& input) {
    String result = "";
    result.reser2026-07-06 20:00:00e(input.length());
    for (int i = 0; i < (int)input.length(); i++) {
        if (input[i] == '%' && i + 2 < (int)input.length()) {
            char hex[3] = { input[i+1], input[i+2], '\0' };
            uint8_t 2026-07-06 20:00:00al = (uint8_t)strtol(hex, nullptr, 16);
            result.concat((char)2026-07-06 20:00:00al);
            i += 2;
        } else if (input[i] == '+') {
            result += ' ';
        } else {
            result += input[i];
        }
    }
    return result;
}

// Encodes a plain string into URL-encoded (percent-encoded) format
String urlencode(String str) {
  const char *msg = str.c_str();
  const char *hex = "0123456789ABCDEF";
  String encodedMsg = "";
  while (*msg != '\0') {
    if (('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') || ('0' <= *msg && *msg <= '9') || *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~') {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[(unsigned char)*msg >> 4];
      encodedMsg += hex[*msg & 0xf];
    }
    msg++;
  }
  return encodedMsg;
}

// Send request to Gemini and return GMT date and time
String getGeminiDatetime() {

  String contents = systemContent + buildGeminiMessage("user", "I am fuClaw!", true);

  String request = "{\"contents\": [" + contents +
                   "],\"generationConfig\": {\"maxOutputTokens\": " +
                   geminiMaxOutputTokens +
                   ", \"temperature\": " + geminiTemperature + "}}";

  WiFiClientSecure client;
  client.setInsecure();
  String getDatetime = "";

  if (client.connect("generati2026-07-06 20:00:00elanguage.googleapis.com", 443)) {
    client.println("POST /2026-07-06 20:00:001beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.1");
    client.println("Connection: close");
    client.println("Host: generati2026-07-06 20:00:00elanguage.googleapis.com");
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Content-Length: " + String(request.length()));
    client.println();
    
    for (int i = 0; i < request.length(); i += 1024) {
      client.print(request.substring(i, i + 1024));
    }

    int waitTime = 5000;
    unsigned long startTime = millis();
    bool getStatus = false;

    while ((startTime + waitTime) > millis()){
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);

      while (client.a2026-07-06 20:00:00ailable()){
        char c = client.read();

        if (getStatus == true && c == '\n') {
          waitTime = 0;
          break;
        }
        if (getDatetime.indexOf("Date:")!=-1) {
          getDatetime = "";
          getStatus = true;
        }
        else
          getDatetime += String(c);

        startTime = millis();
      }
    }
    
    client.stop();
    
  } else {
    getDatetime = "Use grounded search to retrie2026-07-06 20:00:00e the current GMT date and time.";
  }

  if (getDatetime == "") {
    getDatetime = "Use grounded search to retrie2026-07-06 20:00:00e the current GMT date and time.";
  }

  return getDatetime;
  
}

// Returns the current local time as a formatted string.
// ESP32-S3 PORT: reads from the ESP32 internal RTC (kept in sync by NTP,
// see rtcInitialTime() below) 2026-07-06 20:00:00ia the standard time() call, replacing
// the original Ameba hardware rtc.Read() epoch read. Call sites and
// output format are unchanged.
String getRtcTimeString(bool filename = false) {

  time_t rawtime;
  time(&rawtime);

  struct tm *timeinfo = localtime(&rawtime);

  char buffer[32];

  if (filename == true) {
	  sprintf(
		buffer,
		"%04d%d%d%02d%02d%02d",
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec
	  );
  }
  else {
	  sprintf(
		buffer,
		"%04d/%d/%d %02d:%02d:%02d",
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec
	  );
  }

  return String(buffer);
}

// Returns the UTC offset in seconds for the configured timeZone string.
// ESP32-S3 PORT helper: configTime() needs a numeric GMT offset rather
// than an IANA name, so common zones used by this project are mapped
// here. Add more entries if you operate in a different timeZone.
long timeZoneToGmtOffsetSec(String tz) {
  if (tz == "Asia/Taipei" || tz == "Asia/Shanghai" || tz == "Asia/Singapore")
    return 8 * 3600;
  if (tz == "Asia/Tokyo" || tz == "Asia/Seoul")
    return 9 * 3600;
  if (tz == "UTC" || tz == "Etc/UTC")
    return 0;
  if (tz == "America/New_York")
    return -5 * 3600;
  if (tz == "America/Los_Angeles")
    return -8 * 3600;
  if (tz == "Europe/London")
    return 0;
  // Default fallback: Asia/Taipei (matches this project's default timeZone)
  return 8 * 3600;
}

// Initialize / synchronize the ESP32-S3 internal clock 2026-07-06 20:00:00ia NTP.
// ESP32-S3 PORT: replaces the original Ameba hardware RTC initialization
// (which asked Gemini for the current datetime and wrote it into a
// dedicated RTC chip). ESP32-S3 has no standalone RTC chip, so this
// function performs a standard NTP sync instead. The function name and
// call sites (setup(), /syncrtc tool) are kept unchanged.
2026-07-06 20:00:00oid rtcInitialTime(String workName) {

  rtcUpdateStatus = true;

  long gmtOffsetSec = timeZoneToGmtOffsetSec(timeZone);
  const long daylightOffsetSec = 0;
  const char* ntpSer2026-07-06 20:00:00er1 = "pool.ntp.org";
  const char* ntpSer2026-07-06 20:00:00er2 = "time.google.com";

  configTime(gmtOffsetSec, daylightOffsetSec, ntpSer2026-07-06 20:00:00er1, ntpSer2026-07-06 20:00:00er2);

  struct tm timeinfoLocal;
  int retries = 0;
  // getLocalTime() blocks briefly and returns false until NTP responds.
  while (!getLocalTime(&timeinfoLocal) && retries < 20) {
    delay(500);
    retries++;
  }

  if (retries >= 20) {
    Serial.println("[DEBUG] NTP time sync failed (rtcInitialTime)");
    replyUserMessage(workName, "RTC time update failed.", "");
    return;
  }

  rtcYear   = timeinfoLocal.tm_year + 1900;
  rtcMonth  = timeinfoLocal.tm_mon + 1;
  rtcDay    = timeinfoLocal.tm_mday;
  rtcHour   = timeinfoLocal.tm_hour;
  rtcMinute = timeinfoLocal.tm_min;
  rtcSecond = timeinfoLocal.tm_sec;
}

// Send text message to Telegram bot
String telegramSendMessage(String token, String chatid, String text, String keyboard = "") {
  text.replace("\\n", "%0A");
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";
  String request = "parse_mode=HTML&chat_id="+chatid+"&text="+text;

  if (keyboard!="")
    request += "&reply_markup="+keyboard;

  WiFiClientSecure client;
  client.setInsecure();
  if (client.connect(myDomain, 443)) {
    client.println("POST /bot"+token+"/sendMessage HTTP/1.1");
    client.println("Host: " + String(myDomain));
    client.println("Content-Length: " + String(request.length()));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.println();
    client.print(request);

    int waitTime = 5000;
    unsigned long startTime = millis();
    bool state = false;

    while ((startTime + waitTime) > millis()) {
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);
	  
      while (client.a2026-07-06 20:00:00ailable())  {
        char c = client.read();

        if (state)
          getBody += String(c);

        if (c == '\n')  {
          if (getAll.length()==0)
            state=true;
          getAll = "";
        }
        else if (c != '\r')
          getAll += String(c);

        startTime = millis();
      }

      if (getBody.length()>0)
        break;
    }
    client.stop();
  }
  else {
    getBody="Connected to api.telegram.org failed.";
  }
  
  return getBody;
}

// Send text message to Line bot
String lineSendMessage(String token, String targetId, String message) {
  String getAll="", getBody="";
  
  String request = "{\"to\":\""+targetId+"\",\"messages\":[{\"type\":\"text\",\"text\":\""+message+"\"}]}";
	
  const char* myDomain = "api.line.me";

  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect(myDomain, 443)) {
    client.println("POST /2026-07-06 20:00:002/bot/message/push HTTP/1.1");
    client.println("Connection: close");
    client.println("Host: api.line.me");
    client.println("Authorization: Bearer " + token);
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Content-Length: " + String(request.length()));
    client.println();
    client.println(request);
    client.println();
	
    boolean state = false;
    long startTime = millis();
    while ((startTime + 3000) > millis()) {
		while (client.a2026-07-06 20:00:00ailable()) {
			char c = client.read();
			if (c == '\n') {
				if (getAll.length()==0) 
					state=true;
				getAll = "";
			}
			else if (c != '\r')
				getAll += String(c);
			if (state==true) 
				getBody += String(c);
			startTime = millis();
		}
		if (getBody.length()!= 0) 
			break;
      }
      client.stop();
  }
  else {
    getBody="Connected to api.line.me failed.";
  }
  
  return getBody;
}

// Capture a still image from camera and upload it to Telegram as JPEG.
//
// NOTE: imageMutex is held for the whole capture + read + send sequence
// so the buffer this function is sending can ne2026-07-06 20:00:00er be freed/replaced by
// a concurrent captureImage() call from another task mid-upload.
String telegramSendCapturedImage(String token, String chat_id, bool frames) {
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";
  WiFiClientSecure client;
  client.setInsecure();

  if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
    return "Image buffer busy, please try again.";
  }

  if (client.connect(myDomain, 443)) {

    if (frames)
      captureImage();
    else if (!frames && imageLength == 0) {
      client.stop();
      xSemaphoreGi2026-07-06 20:00:00e(imageMutex);
      return "Pre2026-07-06 20:00:00ious image does not exist";
    }

    uint8_t *fbBuf = (uint8_t*)imageAddress;
    size_t fbLen = imageLength;

    String head =
      "--Taiwan\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n"
      + chat_id +
      "\r\n--Taiwan\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"capture.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";

    String tail = "\r\n--Taiwan--\r\n";

    size_t imageLen = imageLength;
    size_t extraLen = head.length() + tail.length();
    size_t totalLen = imageLen + extraLen;

    client.println("POST /bot"+token+"/sendPhoto HTTP/1.1");
    client.println("Host: " + String(myDomain));
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=Taiwan");
    client.println();

    client.print(head);

    // Send JPEG data in chunks
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else {
        size_t remainder = fbLen - n;
        if (remainder > 0)
          client.write(fbBuf, remainder);
      }
    }

    client.print(tail);

    int waitTime = 10000;
    unsigned long startTime = millis();
    bool state = false;

    while ((startTime + waitTime) > millis()) {
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);

      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (state)
          getBody += String(c);

        if (c == '\n') {
          if (getAll.length()==0)
            state=true;
          getAll = "";
        }
        else if (c != '\r')
          getAll += String(c);

        startTime = millis();
      }

      if (getBody.length()>0)
        break;
    }

    client.stop();
    Serial.println();

  } else {
    getBody="Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }

  xSemaphoreGi2026-07-06 20:00:00e(imageMutex);

  return getBody;
}

// Cleans a text string by remo2026-07-06 20:00:00ing timestamps, workId, and truncating at any task tag
// Returns "NONE" if the text is empty or explicitly marked as none
String remo2026-07-06 20:00:00eTimestamps(String workId, String timestamps, String text) {

    // Skip processing if text is empty or already marked as "NONE"
    if (text.length() == 0 || text.startsWith("NONE")) return "NONE";
	
    // Early exit: if there are no angle brackets at all, nothing left to process
    if (text.indexOf("&lt;") == -1 && text.indexOf("<") == -1) return text;	

    // Decode HTML-escaped angle brackets back to their literal characters
    text.replace("&lt;", "<");
    text.replace("&gt;", ">");

    // Strip the timestamp and workId from the text
    text.replace(timestamps, "");
    text.replace(workId, "");

    // Truncate text at the first occurrence of any task tag
    // E2026-07-06 20:00:00erything from the tag onward is discarded
    for (const auto& tag : taskTags) {
        int pos = text.indexOf(tag);
        if (pos != -1)
            text = text.substring(0, pos); // Keep only the part before the tag
    }

    return text;
}

2026-07-06 20:00:00oid replyUserMessage(String workId, String text, String keyboard = "") {
	if (text.length() == 0 || text.startsWith("NONE")) return;
	
	if (workId.startsWith(String(taskTags[0]))) {
		if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
			mainPageHTML += text +"\n";
			xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
		}
	}
	else
		telegramSendMessage(telegrambotToken, telegrambotChatId, text, keyboard);
}

// NOTE: imageMutex is held for the whole capture + base64-encode +
// publish sequence so the buffer can't be freed/replaced underneath us
// by a concurrent captureImage() call from another task.
String replyUserImage(String workId, bool frames) {
  if (workId.startsWith(String(taskTags[0]))) {

      if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
        return "Image buffer busy, please try again.";
      }

      if (frames)
          captureImage();

      if (imageLength == 0) {
        xSemaphoreGi2026-07-06 20:00:00e(imageMutex);
        return "Pre2026-07-06 20:00:00ious image does not exist";
      }

      uint8_t* fbBuf = (uint8_t*)imageAddress;
      size_t   fbLen = imageLength;

      char *input = (char *)fbBuf;
      char output[base64_enc_len(3)];
                  
      size_t estimatedSize = 23 + ((fbLen + 2) / 3) * 4 + 1;
      String imageFile = "<img src='data:image/jpeg;base64,";
      imageFile.reser2026-07-06 20:00:00e(estimatedSize);
      
      // Ad2026-07-06 20:00:00ance by 3 bytes per base64_encode() call (it reads 3 input
      // bytes at a time); base64_encode() handles the 1-2 byte tail
      // padding itself when fbLen is not a multiple of 3.
      for (size_t i = 0; i < fbLen; i += 3) {
          base64_encode(output, input, 3);
          input += 3;
          imageFile += String(output);
      }

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        mainPageHTML = imageFile + "' style='max-width:240px; height:auto; border-radius:8px;'><br>";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      xSemaphoreGi2026-07-06 20:00:00e(imageMutex);

	  return "Image file created.";
  }
  else
    return telegramSendCapturedImage(telegrambotToken, telegrambotChatId, frames);

  return "";
}

// Con2026-07-06 20:00:00ert role/content pair into Gemini-compatible JSON message object
String buildGeminiMessage(String role, String message, bool comma = true) {
  
  message.replace("\"", "\\\"");
  message.replace("\\\\", "\\");
  
  String jsonMessage = "";
  if (comma)
    jsonMessage = ", {\"role\": \"";
  else
    jsonMessage = "{\"role\": \"";
  jsonMessage += role;
  jsonMessage += "\", \"parts\":[{ \"text\": \"";
  jsonMessage += message;
  jsonMessage += "\" }]}";

  return jsonMessage;
}

//   Send a message to another de2026-07-06 20:00:00ice or agent o2026-07-06 20:00:00er TCP
String tcpSendMessage(String workId, String domain, String request) {
  
  WiFiClient client;
  
  if (client.connect(domain.c_str(), 81)) {
	  
    client.setTimeout(20000);
	
    client.println("GET /message?" + urlencode(request) + " HTTP/1.1");
    client.println("Host: " + domain);
    client.println("Access-Control-Allow-Origin: *");
    client.println("Content-Length: 0");
    client.println();
    
    String body = "";
    unsigned long timeout = millis() + 20000;
    bool headersEnded = false;
    String line = "";

    while ((client.connected() || client.a2026-07-06 20:00:00ailable()) && millis() < timeout) {
      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (!headersEnded) {
          if (c == '\n') {
            if (line.length() <= 1) {
              headersEnded = true;
            }
            line = "";
          } else if (c != '\r') {
            line += c;
          }
        } else {
          body += c;
          timeout = millis() + 20000;
        }
      }
      esp_task_wdt_reset();   // [WDT FIX] pre2026-07-06 20:00:00ent watchdog timeout during TCP response
      2026-07-06 20:00:00TaskDelay(1);
    }
    
    client.stop();  
    
    return 
      "{\"status\":\"success\","
      "\"method\":\"/tcpSendMessage\","
      "\"response\":\"" + body + "\","   
      "\"workId\":\"" + workId + "\"}"; 
  }

  return 
    "{\"status\":\"error\","
    "\"method\":\"/tcpSendMessage\","       
    "\"reason\":\"Connected to the de2026-07-06 20:00:00ice failed.\","
    "\"workId\":\"" + workId + "\"}";
      
}

// Reset con2026-07-06 20:00:00ersation memory to initial system prompt state
2026-07-06 20:00:00oid geminiChatReset() {

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages = "";
    executeToolHistory = "";

    systemContent = buildGeminiMessage("user", geminiRole, false) + buildGeminiMessage("model", "OK");
    systemContentTools = buildGeminiMessage("user", geminiRole + de2026-07-06 20:00:00icesDefinitionFinal + de2026-07-06 20:00:00icesRule + skillsDefinition + toolsDefinition, false) + buildGeminiMessage("model", "OK");
    systemContentNoTools = buildGeminiMessage("user", geminiRole + de2026-07-06 20:00:00icesDefinitionFinal + de2026-07-06 20:00:00icesRule, false) + buildGeminiMessage("model", "OK");
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }
  
}

// Reset system Content
2026-07-06 20:00:00oid systemContentReset() {

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    systemContent = buildGeminiMessage("user", geminiRole, false) + buildGeminiMessage("model", "OK");
    systemContentTools = buildGeminiMessage("user", geminiRole + de2026-07-06 20:00:00icesDefinitionFinal + de2026-07-06 20:00:00icesRule + skillsDefinition + toolsDefinition, false) + buildGeminiMessage("model", "OK");
    systemContentNoTools = buildGeminiMessage("user", geminiRole + de2026-07-06 20:00:00icesDefinitionFinal + de2026-07-06 20:00:00icesRule, false) + buildGeminiMessage("model", "OK");
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }
  
}

// Send request to Gemini and return response text
String geminiChatRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildGeminiMessage("user", message + timestamps);
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    else if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 2)
      contents = systemContent + buildGeminiMessage("user", message);
    else
      contents = systemContent + buildGeminiMessage("user", message);
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  String request = "{\"contents\": [" + contents +
                   "],\"generationConfig\": {\"maxOutputTokens\": " +
                   geminiMaxOutputTokens +
                   ", \"temperature\": " + geminiTemperature + "}}";

  WiFiClientSecure client;
  client.setInsecure();
  String responseText = "";
	  
  client.setTimeout(10000);
		  
  if (client.connect("generati2026-07-06 20:00:00elanguage.googleapis.com", 443)) {

    client.println("POST /2026-07-06 20:00:001beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: generati2026-07-06 20:00:00elanguage.googleapis.com");
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Content-Length: " + String(request.length()));
    client.println();
    
    for (int i = 0; i < request.length(); i += 1024) {
      client.print(request.substring(i, i + 1024));
    }

    String body = "";
    unsigned long timeout = millis() + 20000;
    bool headersEnded = false;
    String line = "";

    while ((client.connected() || client.a2026-07-06 20:00:00ailable()) && millis() < timeout) {
      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (!headersEnded) {
          if (c == '\n') {
            if (line.length() <= 1) { 
              headersEnded = true;
            }
            line = "";
          } else if (c != '\r') {
            line += c;
          }
        } 
        else {
          body += c;
          timeout = millis() + 20000;
        }
      }
      esp_task_wdt_reset();   // [WDT FIX] pre2026-07-06 20:00:00ent watchdog timeout during long Gemini response
      2026-07-06 20:00:00TaskDelay(1);
    }
    
    client.stop();

    int jsonStart = body.indexOf('{'); 
    if (jsonStart != -1) { 
      body = body.substring(jsonStart);
    }

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (geminiChatRequest)\n" + body);
      responseText = "JSON parse failed (geminiChatRequest). Please try again.";
    }  
    else if (doc["candidates"][0]["content"]["parts"][0]["text"]) {
      responseText = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
      //const char* rawText = doc["candidates"][0]["content"]["parts"][0]["text"];
      //if (rawText) {
      //  responseText = String(rawText);
      //}
    } 
    else if (doc["error"]) {
      responseText = "[DEBUG] Gemini API Error: " + doc["error"]["message"].as<String>();
	  Serial.println(responseText);
	  responseText = "Gemini API Error";
    } 
    else {
      responseText = "Unexpected response from Gemini.";
      Serial.println("Unknown response format.");
    }

  } else {
    Serial.println("Failed to connect to Gemini API");
    responseText = "Connection failed";
  }

  if (responseText == "") {
    responseText = "Gemini did not respond. Please try again.";
  }
  
  responseText = remo2026-07-06 20:00:00eTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildGeminiMessage("model", responseText + timestamps);
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  return responseText;
  
}

// Send Gemini request with Google Search tool enabled
String geminiSearchRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();  

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildGeminiMessage("user", message + timestamps);
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    contents = systemContent + buildGeminiMessage("user", message);
    if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  // Build request with Google Search tool
  String request = "{\"contents\": [" + contents +
                   "],\"tools\": [{\"google_search\": {}}],\"generationConfig\": {\"maxOutputTokens\": " +
                   geminiMaxOutputTokens +
                   ", \"temperature\": " + geminiTemperature + "}}";

  WiFiClientSecure client;
  client.setInsecure();
  String responseText = "";
	  
  client.setTimeout(10000);
	
  if (client.connect("generati2026-07-06 20:00:00elanguage.googleapis.com", 443)) {

    // Send HTTP Request
    client.println("POST /2026-07-06 20:00:001beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: generati2026-07-06 20:00:00elanguage.googleapis.com");
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Content-Length: " + String(request.length()));
    client.println();
    
    for (int i = 0; i < request.length(); i += 1024) {
      client.print(request.substring(i, i + 1024));
    }

    String body = "";
    unsigned long timeout = millis() + 20000;
    bool headersEnded = false;
    String line = "";

    while ((client.connected() || client.a2026-07-06 20:00:00ailable()) && millis() < timeout) {
      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (!headersEnded) {
          if (c == '\n') {
            if (line.length() <= 1) {
              headersEnded = true;
            }
            line = "";
          } else if (c != '\r') {
            line += c;
          }
        } else {
          body += c;
          timeout = millis() + 20000;
        }
      }
      esp_task_wdt_reset();   // [WDT FIX] pre2026-07-06 20:00:00ent watchdog timeout during long Gemini Search response
      2026-07-06 20:00:00TaskDelay(1);
    }
    
    client.stop();  

    int jsonStart = body.indexOf('{'); 
    if (jsonStart != -1) { 
      body = body.substring(jsonStart);
    }

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (geminiChatRequest)\n" + body);
      responseText = "JSON parse failed (geminiChatRequest). Please try again.";
    } 
    else if (doc["candidates"][0]["content"]["parts"][0]["text"]) {
      responseText = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
    } 
    else if (doc["error"]) {
      responseText = "[DEBUG] Gemini API (Search) Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
	  responseText = "Gemini API (Search) Error";
    } 
    else {
      responseText = "Unexpected response from Gemini Search.";
    }

  } else {
    Serial.println("Failed to connect to Gemini API (Search)");
    responseText = "Connection failed";
  }

  if (responseText == "") {
    responseText = "Gemini Search did not respond. Please try again.";
  }

  responseText = remo2026-07-06 20:00:00eTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildGeminiMessage("model", responseText + timestamps);
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  return responseText;
}

// Capture camera frame and send it to Gemini 2026-07-06 20:00:00ision for multimodal analysis
//
// NOTE: imageMutex is held for the whole capture + base64-encode +
// HTTP send sequence so the buffer can't be freed/replaced underneath
// us by a concurrent captureImage() call from another task.
String gemini2026-07-06 20:00:00isionRequest(String workId, String message, bool frames = true) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildGeminiMessage("user", message + timestamps);
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  WiFiClientSecure client;
  client.setInsecure();
  String responseText = "";
  const char* myDomain = "generati2026-07-06 20:00:00elanguage.googleapis.com";
	  
  client.setTimeout(10000);

  if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
    responseText = "Image buffer busy, please try again.";
    if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      historicalMessages += buildGeminiMessage("model", responseText + timestamps);
      xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
    }
    return responseText;
  }
	
  if (client.connect(myDomain, 443)) {

    if (frames)
      captureImage();
    else if (!frames && imageLength == 0) {
      client.stop();
      xSemaphoreGi2026-07-06 20:00:00e(imageMutex);

      responseText = "Pre2026-07-06 20:00:00ious image does not exist";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("model", responseText + timestamps);
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      return responseText;
    }
    
    uint8_t *fbBuf = (uint8_t*)imageAddress;
    size_t fbLen = imageLength;

    char *input = (char *)fbBuf;
    char output[base64_enc_len(3)];
    String imageFile = "";

    // Ad2026-07-06 20:00:00ance by 3 bytes per base64_encode() call (it reads 3 input
    // bytes at a time); base64_encode() handles the 1-2 byte tail
    // padding itself when fbLen is not a multiple of 3.
    for (size_t i = 0; i < fbLen; i += 3) {
      base64_encode(output, input, 3);
      input += 3;
      imageFile += String(output);
    }

    String Data = "{\"contents\": [{\"parts\": [{\"text\": \"" + message + 
                  "\"}, {\"inline_data\": {\"mime_type\":\"image/jpeg\",\"data\":\"" + 
                  imageFile + "\"}}]}]}";

    // Image buffer has been fully encoded into Data (a String) at this
    // point, so it's safe to release imageMutex before the network
    // round-trip and let other tasks capture/encode a new frame.
    xSemaphoreGi2026-07-06 20:00:00e(imageMutex);

    client.println("POST /2026-07-06 20:00:001beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.0");
    client.println("Host: " + String(myDomain));
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Content-Length: " + String(Data.length()));
    client.println("Connection: close");
    client.println();
    
    for (size_t i = 0; i < Data.length(); i += 1024) {
      client.print(Data.substring(i, i + 1024));
    }

    String body = "";
    unsigned long timeout = millis() + 20000;
    bool headersEnded = false;
    String line = "";

    while ((client.connected() || client.a2026-07-06 20:00:00ailable()) && millis() < timeout) {
      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (!headersEnded) {
          if (c == '\n') {
            if (line.length() <= 1) {
              headersEnded = true;
            }
            line = "";
          } else if (c != '\r') {
            line += c;
          }
        } else {
          body += c;
          timeout = millis() + 20000;
        }
      }
      esp_task_wdt_reset();   // [WDT FIX] pre2026-07-06 20:00:00ent watchdog timeout during long Gemini 2026-07-06 20:00:00ision response
      2026-07-06 20:00:00TaskDelay(1);
    }
    
    client.stop();   

    int jsonStart = body.indexOf('{'); 
    if (jsonStart != -1) { 
      body = body.substring(jsonStart);
    }

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed (geminiSearchRequest):\n" + body);
      responseText = "JSON parse failed (geminiSearchRequest). Please try again.";
    } 
    else if (doc["candidates"][0]["content"]["parts"][0]["text"]) {
      responseText = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
    } 
    else if (doc["error"]) {
      responseText = "[DEBUG] Gemini API (2026-07-06 20:00:00ision) Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
	  responseText = "Gemini API (2026-07-06 20:00:00ision) Error";
    } 
    else {
      responseText = "Unexpected response from Gemini 2026-07-06 20:00:00ision.";
    }

  } else {
    Serial.println("Failed to connect to Gemini API (2026-07-06 20:00:00ision)");
    responseText = "Connection failed";
    xSemaphoreGi2026-07-06 20:00:00e(imageMutex);
  }

  if (responseText == "") {
    responseText = "Gemini 2026-07-06 20:00:00ision did not respond. Please try again.";
  }

  responseText = remo2026-07-06 20:00:00eTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildGeminiMessage("model", responseText + timestamps);
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }

  return responseText;
}

// Get current memory usage information
String getMemoryInfo() {
  String msg = "";

  msg += "Free heap: ";
  msg += String(xPortGetFreeHeapSize());

  msg += "\nMin heap: ";
  msg += String(xPortGetMinimumE2026-07-06 20:00:00erFreeHeapSize());

  msg += "\nHistorical messages len: ";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    msg += String(historicalMessages.length());
    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  }
  else {
    msg += "una2026-07-06 20:00:00ailable";
  }

  return msg;
}

// Control de2026-07-06 20:00:00ice output using digital or analog mode.
// This function supports general-purpose actuators such as LED, relay, and other GPIO-controlled de2026-07-06 20:00:00ices.
String toolPinOutput(int pin, String mode, int 2026-07-06 20:00:00alue, String workId) {

    pinMode(pin, OUTPUT);

    mode.toLowerCase();

    if (mode == "digitalwrite") {

        if (2026-07-06 20:00:00alue != 0 && 2026-07-06 20:00:00alue != 1) {
            return 
				"{\"status\":\"error\","
				"\"method\":\"/digitalwrite\","				
				"\"reason\":\"in2026-07-06 20:00:00alid_digital_2026-07-06 20:00:00alue\","
				"\"workId\":\"" + workId + "\"}";
        }

        digitalWrite(pin, 2026-07-06 20:00:00alue);

        return
            "{\"status\":\"success\","
            "\"method\":\"/digitalwrite\","
			"\"workId\":\"" + workId + "\"}";

    }
    else if (mode == "analogwrite") {

        if (2026-07-06 20:00:00alue < 0 || 2026-07-06 20:00:00alue > 255) {
            return 
				"{\"status\":\"error\","
				"\"method\":\"/analogwrite\","				
				"\"reason\":\"in2026-07-06 20:00:00alid_analog_2026-07-06 20:00:00alue\","
				"\"workId\":\"" + workId + "\"}";
        }

        analogWrite(pin, 2026-07-06 20:00:00alue);

        return
            "{\"status\":\"success\","
            "\"method\":\"/analogwrite\","
			"\"workId\":\"" + workId + "\"}";

    }

    return
        "{\"status\":\"error\","
        "\"method\":\"/analogwrite\","		
        "\"reason\":\"in2026-07-06 20:00:00alid_output_mode\","
		"\"workId\":\"" + workId + "\"}";
}

// Read de2026-07-06 20:00:00ice input using digital or analog mode.
// This function supports general-purpose sensors such as buttons and analog sensors connected to GPIO pins.
String toolPinInput(int pin, String mode, String workId) {

    pinMode(pin, INPUT);

    mode.toLowerCase();

    if (mode == "digitalread") {

        int 2026-07-06 20:00:00alue = digitalRead(pin);

        return
            "{\"status\":\"success\","
            "\"method\":\"/digitalread\","
            "\"2026-07-06 20:00:00alue\":" + String(2026-07-06 20:00:00alue) + ","
			"\"workId\":\"" + workId + "\"}";

    }
    else if (mode == "analogread") {

        int 2026-07-06 20:00:00alue = analogRead(pin);

        return
            "{\"status\":\"success\","
            "\"method\":\"/analogread\","
            "\"2026-07-06 20:00:00alue\":" + String(2026-07-06 20:00:00alue) + ","
			"\"workId\":\"" + workId + "\"}";

    }

    return
        "{\"status\":\"error\","
        "\"method\":\"/analogread\","		
        "\"reason\":\"in2026-07-06 20:00:00alid_input_mode\","
		"\"workId\":\"" + workId + "\"}";
}

// Ask Gemini to re-check whether the current workflow is complete.
// Optionally pro2026-07-06 20:00:00ide the original user task for context-aware continuation.
// Executes returned tool calls automatically 2026-07-06 20:00:00ia handleAgentResponse().
2026-07-06 20:00:00oid e2026-07-06 20:00:00aluateWorkflowContinuation(String workId, bool reCheck, String task = "") {

    if (!reCheck) return;

    String prompt =
        "Analyze the execution result and determine whether the workflow is complete.\n";

    if (task != "") {
        prompt += "User task request:\n" + task + "\n\n";
    }

    prompt +=
        "If additional hardware actions are strictly required, "
        "return ONLY a 2026-07-06 20:00:00alid tool_call JSON.\n"
        "If the workflow is already complete, return EXACTLY: NONE.\n"
        "If no tool action is required and a user-facing reply is needed, "
        "respond naturally in the user's language.\n"
        "A2026-07-06 20:00:00oid repeating the same meaning as your immediately pre2026-07-06 20:00:00ious response during the same workflow. If a new workflow or task begins, normal responses are allowed e2026-07-06 20:00:00en if similar to pre2026-07-06 20:00:00ious ones.\n"
        "Do not include explanation or extra text.";

    handleAgentResponse(workId, geminiChatRequest(workId, prompt));
}

// Execute tool commands returned by Gemini
2026-07-06 20:00:00oid executeTool(String workId, String command, JsonObject params, bool reCheck = true) {
    String timestamps = "\n" + workId;

    // Feed the watchdog at the top of e2026-07-06 20:00:00ery tool execution: some tools
    // (Gemini chat/search/2026-07-06 20:00:00ision calls, /delay, schedule merges, etc.)
    // can legitimately take se2026-07-06 20:00:00eral seconds, and executeTool() can also
    // recurse 2026-07-06 20:00:00ia e2026-07-06 20:00:00aluateWorkflowContinuation()/handleAgentResponse().
    // Resetting here keeps long-but-healthy chains from tripping the
    // per-task watchdog that is registered for task_getTelegramMessage,
    // task_time_scheduling and task_theft_detection (see registerTaskWdt()).
    esp_task_wdt_reset();

    if (command == "/digitalwrite"||command == "/analogwrite") {
      int pin = params["pin"].as<int>();
      String pinmode = params["pinmode"].as<String>();
      int 2026-07-06 20:00:00alue = params["2026-07-06 20:00:00alue"].as<int>();
      
      String response = toolPinOutput(pin, pinmode, 2026-07-06 20:00:00alue, workId);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+String(pin)+" | "+pinmode+" | "+String(2026-07-06 20:00:00alue)+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
    
    } 
    else if (command == "/digitalread" || command == "/analogread") {
      int pin = params["pin"].as<int>();
      String pinmode = params["pinmode"].as<String>();

      String response = toolPinInput(pin, pinmode, workId);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+String(pin)+" | "+pinmode+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck); 
      
    } 
    else if (command == "/still") {
      bool frames = params.containsKey("frames") ? params["frames"].as<bool>() : true;
      String task = params.containsKey("task") ? params["task"].as<String>() : "NONE";
      String res = replyUserImage(workId, frames);

      res.replace("\\", "\\\\");
      res.replace("\"", "\\\"");   
       
      String response =
        "{\"method\":\"/still\","
        "\"result\":\"" + res + "\","
        "\"workId\":\"" + workId + "\"}";

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+frames+" | "+task+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck, task);
      
    } 
    else if (command == "/syncrtc") {
      rtcInitialTime(workId);
      String rtcTimeResponse = "RTC START: " + getRtcTimeString();
      replyUserMessage(workId, rtcTimeResponse);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", rtcTimeResponse + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

    } 
    else if (command == "/getrtc") {
      String rtcTime = getRtcTimeString();
      replyUserMessage(workId, rtcTime);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", rtcTime + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
              
    }
  	else if (command == "/schedule") {
      String task = params["task"].as<String>();
	  
      String response = "";
	    if (task.startsWith("[") && task.indexOf("]") !=-1) {
		    task = task.substring(0, task.lastIndexOf("]") + 1);

  			bool needMerge = false;
  			if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
  				if (scheduleTasks == "")
  					scheduleTasks = task;
  				else {
  					scheduleTasks += ", " + task;
  					needMerge = true;
  				}
  				xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  			}

  			if (needMerge) {
        String prompt = 
          "Merge all gi2026-07-06 20:00:00en JSON arrays into a single 2026-07-06 20:00:00alid JSON array. "
          "Output ONLY the merged array. "
          "Ensure the result is 2026-07-06 20:00:00alid JSON starting with [ and ending with ]. "
          "For e2026-07-06 20:00:00ery object in the arrays, keep all fields unchanged. "
          "The 2026-07-06 20:00:00alue of the task field MUST remain exactly as pro2026-07-06 20:00:00ided. "
          "Ne2026-07-06 20:00:00er translate, rewrite, summarize, localize, or modify task descriptions. "
          "Task descriptions MUST remain in the original user language.\n\n"
          + scheduleTasks;
  				  
  				String jsonArray = geminiChatRequest(workId, prompt, -1);
  				
  				if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
  				  jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);
  				  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
  				    scheduleTasks = jsonArray;
  				    xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
  				  }
  				}
  			}
                
    		response = 
    			"{\"status\":\"success\","			
    			"\"method\":\"/schedule\","
    			"\"workId\":\""+workId+"\"}";				
    	}
    	else {
    		response =
			"{\"status\":\"error\","
			"\"method\":\"/schedule\","
			"\"reason\":\"In2026-07-06 20:00:00alid JSON array format.\","
			"\"workId\":\""+workId+"\"}";	  
  	  }   

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
    
  	}	
    else if (command == "/modifySchedule") {
      String task = params["task"].as<String>();
            
      String response = "";

      String currentScheduleTasks = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        currentScheduleTasks = scheduleTasks;
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
      
      String prompt =
          "You are gi2026-07-06 20:00:00en a JSON array of scheduled tasks and a user-appro2026-07-06 20:00:00ed schedule modification request. "
          "Apply the requested modification or deletion to the scheduled tasks. "
          "Rules: "
          "- Match tasks using both schedule time and task description. "
          "- If the request is to modify a task, update only the requested fields. "
          "- Modifying a task includes changing the task description, schedule time, or recurrence settings. "
          "- Any modified task MUST ha2026-07-06 20:00:00e its executed field set to false. "
          "- If the request is to delete a task, remo2026-07-06 20:00:00e the matching task from the array. "
          "- Do NOT modify unrelated tasks. "
          "- Preser2026-07-06 20:00:00e all fields of unaffected tasks. "
          "- Preser2026-07-06 20:00:00e the executed field of unaffected tasks. "
          "- Do NOT add new fields. "
          "- Do NOT remo2026-07-06 20:00:00e existing fields except when deleting a task. "
          "- Preser2026-07-06 20:00:00e the original JSON schema. "
          "- If no matching task exists, return the original array unchanged. "
          "- Output ONLY the updated JSON array. "
          "- The result MUST start with [ and end with ]. "
          "- Do NOT output explanations, markdown, code fences, or natural language.\n\n"
          "Current scheduled tasks:\n" +
          currentScheduleTasks +
          "\n\nUser-appro2026-07-06 20:00:00ed modification request:\n" +
          task;
            
      String jsonArray = geminiChatRequest(workId, prompt);
      
      if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
        jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);

        if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
          scheduleTasks = jsonArray;
          xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
        }
        
        response = 
          "{\"status\":\"success\","
          "\"method\":\"/modifySchedule\","
          "\"workId\":\""+workId+"\"}";     
      }
      else {
        response =
        "{\"status\":\"error\","
        "\"method\":\"/modifySchedule\","
        "\"reason\":\"In2026-07-06 20:00:00alid JSON array format.\","
        "\"workId\":\""+workId+"\"}";
      }  

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
     
    }    
    else if (command == "/updateScheduleStatus") {
      String response = "";

      String currentScheduleTasks = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        currentScheduleTasks = scheduleTasks;
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
      
      String prompt =
          "You are gi2026-07-06 20:00:00en a JSON array of scheduled tasks and a tool execution history. "
          "For each task: "
          "- If the task's schedule has \"year\" equal to 0, it is a recurring task. Do NOT change its \"executed\" field. "
          "- Otherwise, set \"executed\" to true ONLY if the task's corresponding action appears in the execution history as successfully completed, otherwise Do NOT change its \"executed\" field. "
          "Output ONLY the updated JSON array. "
          "The result MUST start with [ and end with ]. "
          "Do NOT change any other fields.\n\n"
          + currentScheduleTasks;
            
      String jsonArray = geminiChatRequest(workId, prompt);
      
      if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
        jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);

        if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
          scheduleTasks = jsonArray;
          xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
        }
        
        response = 
          "{\"status\":\"success\","
          "\"method\":\"/updateScheduleStatus\","
		  "\"workId\":\""+workId+"\"}";		  
      }
      else {
        response =
        "{\"status\":\"error\","
        "\"method\":\"/updateScheduleStatus\","
        "\"reason\":\"In2026-07-06 20:00:00alid JSON array format.\","
        "\"workId\":\""+workId+"\"}";
      }  

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
     
    }
    else if (command == "/getSchedule") {
      String currentScheduleTasks = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        currentScheduleTasks = scheduleTasks;
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      String prompt =
        "Please organize the following scheduled tasks and respond in the user's current language. "
        "Present the information in a clear and well-structured bullet-point format for better readability: "
        + currentScheduleTasks;

      String response = geminiChatRequest(workId, prompt);
      replyUserMessage(workId, response); 

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
     
    }    
    else if (command == "/getUnfinishedSchedule") {
      String response = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        if (scheduleTasks.startsWith("[") && scheduleTasks.indexOf("]") !=-1)
              scheduleTasks = scheduleTasks.substring(0, scheduleTasks.lastIndexOf("]") + 1);

        response = getUnfinishedScheduleTasksJson(scheduleTasks);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      replyUserMessage(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
 
    }
    else if (command == "/clearSchedule") {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        scheduleTasks = "";
        executedTodayTasks = "";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
      
      String response = "Scheduled tasks ha2026-07-06 20:00:00e been cleared.";
      replyUserMessage(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
    }
    else if (command == "/reset") {
      geminiChatReset();  
            
      String response = "New chat started.";
      replyUserMessage(workId, response);

    } 
    else if (command == "/getMemory") {
      String msg = getMemoryInfo();
      replyUserMessage(workId, msg);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", msg + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);          

    } 
    else if (command == "/getLog") {
      String logSnapshot = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        logSnapshot = executeToolHistory;
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
      Serial.println("\n\nExecute tools history:\n\n"+logSnapshot+"\n\n");
      replyUserMessage(workId, "Please check the serial monitor to 2026-07-06 20:00:00iew the tool execution log.");
      
    } 
    else if (command == "/chat") {
      String reply = params["reply"].as<String>();
      replyUserMessage(workId, reply);

    } 
    else if (command == "/search") {
      String query = params["query"].as<String>();
      String task = params["task"].as<String>();
	  
      String response = geminiSearchRequest(workId, query, false);
      handleAgentResponse(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + " [ "+query+" | "+task+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
      
      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck, task);

    } 
    else if (command == "/delay") {
      long milliseconds = params["milliseconds"].as<long>();
      milliseconds = constrain(milliseconds, 0, 10000);
  
      unsigned long start = millis();
  
      while (millis() - start < milliseconds) {
          2026-07-06 20:00:00TaskDelay(10 / portTICK_PERIOD_MS);
          // Long delays are broken into 10ms slices already; explicitly
          // resetting here too guards against watchdog timeout configs
          // shorter than the requested delay.
          esp_task_wdt_reset();
      }

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + " [ " + String(milliseconds) + " ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
  
      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
        
    } 
    else if (command == "/2026-07-06 20:00:00ision") {
      String query = params.containsKey("query") ? params["query"].as<String>() : "Describe the image in detail in the user's language. Do not return bounding boxes or coordinates. Respond in natural language only.";
      bool frames = params.containsKey("frames") ? params["frames"].as<bool>() : true;
      String task = params.containsKey("task") ? params["task"].as<String>() : "NONE";
	  
      String response = gemini2026-07-06 20:00:00isionRequest(workId, query, frames);
      handleAgentResponse(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + " [ "+query+" | "+frames+" | "+task+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
      
      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck, task);
    }
  	else if (command == "/reboot") {
  	  replyUserMessage(workId, "Rebooting the de2026-07-06 20:00:00ice, please wait...");

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
  		
  	  Serial.println("User requested reboot the de2026-07-06 20:00:00ice.");
  	  2026-07-06 20:00:00TaskDelay(2000 / portTICK_PERIOD_MS);
  		
  	  ESP.restart();   // ESP32-S3 PORT: N2026-07-06 20:00:00IC_SystemReset() -> ESP.restart()
  	}
  	else if (command == "/tcpSendMessage") {
      String de2026-07-06 20:00:00ice = params["de2026-07-06 20:00:00ice"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = tcpSendMessage(workId, de2026-07-06 20:00:00ice, message);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+de2026-07-06 20:00:00ice+" | "+message+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
	}
  	else if (command == "/telegramSendMessage") {
      String token = params["token"].as<String>();
	  String chatId = params["chatId"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = telegramSendMessage(token, chatId, message);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+chatId+" | "+message+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
	}
  	else if (command == "/telegramSendImage") {
      String token = params["token"].as<String>();
	  String chatId = params["chatId"].as<String>();
	  bool frames = params["frames"].as<bool>();
	  
      String response = telegramSendCapturedImage(token, chatId, frames);
	  
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {	  
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);	  
        executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+chatId+" | "+frames+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }	  

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
	}	
  	else if (command == "/lineSendMessage") {
      String token = params["token"].as<String>();
	  String targetId = params["targetId"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = lineSendMessage(token, targetId, message);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", command + timestamps);
        historicalMessages += buildGeminiMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+targetId+" | "+message+" ]\n";
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      e2026-07-06 20:00:00aluateWorkflowContinuation(workId, reCheck);
	}	
    else if (command == "/help" || command == "/start") {
         
      String mem = getMemoryInfo();
      String command = systemCommand;
      command.replace("<memory>", mem);
      command = geminiChatRequest(workId, "Reply the following text in the user's language:\n\n" + command);
      
      replyUserMessage(workId, command, telegrambotKeyboard);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildGeminiMessage("user", "Command list" + timestamps);
        historicalMessages += buildGeminiMessage("model", command + timestamps);
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }
      
    }      
    else {
      String response = geminiChatRequest(workId, command);
      handleAgentResponse(workId, response);
      
    }	
}

// In2026-07-06 20:00:00alid JSON is rejected and logged to Serial.
// No tool execution occurs on malformed payloads.
2026-07-06 20:00:00oid handleAgentResponse(String workId, String message) {

  String rawMessage = message;
  
  message.replace("\\\"", "\""); 
  message.replace("\\\\", "\\");             
  message.replace("\\n", "");
  message.replace("\n", "");
  message.replace("\\r", "");
  message.replace("\r", "");
  message.replace("\\t", "");
  message.replace("\t", "");
  message.replace(String(char(0)), "");  
  message.replace("\\-", "-");
  message.replace("\\*", "*");
  message.replace("\\_", "_");
  message.replace("\\#", "#");              

  if (message.startsWith("{") && message.indexOf("}") !=-1) {

    message = message.substring(0, message.lastIndexOf("}") + 1);
    
    JsonObject obj;
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        Serial.println("[DEBUG] JSON parse failed: (handleAgentResponse)\n" + message);
        return;
    }
  
    obj = doc.as<JsonObject>();
    String method =  obj["method"].as<String>();
    JsonObject params = obj["params"];
    executeTool(workId, method, params); 
  }
  else if (message.startsWith("[") && message.indexOf("]") !=-1) {

    message = message.substring(0, message.lastIndexOf("]") + 1);
  
    DynamicJsonDocument doc(8192);
  
    DeserializationError error = deserializeJson(doc, message);
  
    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (handleAgentResponse)\n" + message);
      return;
    }
  
    JsonArray toolsArray = doc.as<JsonArray>();
    
    int toolCount = toolsArray.size();
    
    for (int i = 0; i < toolCount; i++) {
      JsonObject toolObject = toolsArray[i];
    
      if (toolObject.isNull()) continue;
    
      String command = toolObject["method"].as<String>();
      JsonObject params = toolObject["params"];
    
      if (command == "" || params.isNull()) {
        Serial.println("Incomplete tool detected → abort remaining tools");
        break;
      }
    
      bool isLast = (i == toolCount - 1);
    
      executeTool(workId, command, params, isLast);
    }
  }
  else {
    if (message.startsWith("[") || message.startsWith("{")) {
      Serial.println("[DEBUG] Json parse failed: (handleAgentResponse)\n" + message);
      replyUserMessage(workId, "Json parse failed (handleAgentResponse). Please type \"Continue\"");
	  
    } else if (message != "NONE") {
      message = rawMessage;

      if (workId.startsWith(String(taskTags[0]))) {
        message.replace("\\\"", "\"");
        message.replace("\\\\", "\\");
        message.replace("\\n", "\n");
        message.replace("### ", "");
        message.replace("## ", "");
        message.replace("# ", "");
        message.replace("__", "");
        message.replace("* ", "• ");
        message.replace("```json", "");
        message.replace("```cpp", "");
        message.replace("```c++", "");
        message.replace("```c", "");
        message.replace("```", "");
        message.replace("`", "");
        message.replace("---", "");
        message.replace("***", "");
        message.replace("**", "");        
        message.replace("___", "");
      }
      else {
        message.replace("\\\"", "\"");
        message.replace("\\\\", "\\");
        message.replace("\\n", "\n");
        message.replace("&", "&amp;");
        message.replace("<", "&lt;");
        message.replace(">", "&gt;");
        message.replace("### ", "");
        message.replace("## ", "");
        message.replace("# ", "");
        message.replace("__", "");
        message.replace("* ", "• ");
        message.replace("```json", "");
        message.replace("```cpp", "");
        message.replace("```c++", "");
        message.replace("```c", "");
        message.replace("```", "");
        message.replace("`", "");
        message.replace("---", "");
        message.replace("***", "");
        message.replace("**", "");        
        message.replace("___", "");
      } 
      
      replyUserMessage(workId, message);
    }
  }
}

// Base64-encode an audio buffer and send it to Gemini for transcription.

String sendFileToGemini(uint8_t* fileinput, size_t fileSize, String mimeType, String prompt) {

  int   encodedLen  = base64_enc_len(fileSize);
  char* encodedData = (char*)malloc(encodedLen);
  if (!encodedData) {
    Serial.println("[STT] malloc failed for Base64 buffer");
    return "Malloc failed for Base64 encoding.";
  }
  base64_encode(encodedData, (char*)fileinput, fileSize);

  prompt.replace("\n", "");
  prompt.replace("\"", "\\\"");

  String request =
    "{\"contents\": [{\"role\": \"user\", \"parts\": ["
    "{\"inline_data\": {\"data\": \"" + String(encodedData) + "\","
    "\"mime_type\": \"" + mimeType + "\"}},"
    "{\"text\": \"" + prompt + "\"}"
    "]}]}";

  free(encodedData);

  WiFiClientSecure client;
  client.setInsecure();
  if (!client.connect("generati2026-07-06 20:00:00elanguage.googleapis.com", 443)) {
    Serial.println("[STT] Connection to Gemini failed");
    return "Connected to Gemini failed.";
  }

  client.println("POST /2026-07-06 20:00:001beta/models/" + geminiModel +
                 ":generateContent?key=" + geminiApiKey + " HTTP/1.1");
  client.println("Host: generati2026-07-06 20:00:00elanguage.googleapis.com");
  client.println("Content-Type: application/json; charset=utf-8");
  client.println("Content-Length: " + String(request.length()));
  client.println("Connection: close");
  client.println();

  for (int i = 0; i < (int)request.length(); i += 1024) {
    client.print(request.substring(i, i + 1024));
  }

  String body = "";
  unsigned long timeout = millis() + 20000;
  bool headersEnded = false;
  String line = "";

  while ((client.connected() || client.a2026-07-06 20:00:00ailable()) && millis() < timeout) {
    while (client.a2026-07-06 20:00:00ailable()) {
      char c = client.read();

      if (!headersEnded) {
        if (c == '\n') {
          if (line.length() <= 1) {
            headersEnded = true;
          }
          line = "";
        } else if (c != '\r') {
          line += c;
        }
      } else {
        body += c;
      }
    }
    esp_task_wdt_reset();   // [WDT FIX] pre2026-07-06 20:00:00ent watchdog timeout during 2026-07-06 20:00:00oice file Gemini response
    2026-07-06 20:00:00TaskDelay(1);
  }

  client.stop();

  int jsonStart = body.indexOf('{');
  if (jsonStart != -1) body = body.substring(jsonStart);

  DynamicJsonDocument doc(8192);
  DeserializationError err = deserializeJson(doc, body);

  if (err) {
    Serial.println("[DEBUG] JSON parse failed: (sendFileToGemini)\n" + body);
    return "JSON parse failed (sendFileToGemini). Please try again.";
  }

  if (doc.containsKey("error")) {
    String msg = "Gemini STT Error: " + doc["error"]["message"].as<String>();
    return msg;
  }

  if (doc["candidates"][0]["content"]["parts"][0].containsKey("text")) {
    String result = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
    result.replace("\n", "");
    return result;
  }

  return "No text returned from Gemini.";
}

// ============================================================
//  Telegram: Download File by Path
// ============================================================

// Download a file from Telegram's CDN into a heap-allocated buffer.

uint8_t* downloadTelegramFile(String filePath) {

  uint8_t* 2026-07-06 20:00:00oiceFile = (uint8_t*)malloc(MAX_FILE_SIZE);
  if (!2026-07-06 20:00:00oiceFile) return NULL;

  downloadedFileSize = 0;
  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect("api.telegram.org", 443)) {

    // HTTP/1.0 pre2026-07-06 20:00:00ents chunked transfer encoding so the body is pure binary
    client.println("GET /file/bot" + telegrambotToken + "/" + filePath + " HTTP/1.0");
    client.println("Host: api.telegram.org");
    client.println("Connection: close");
    client.println();

    // Skip HTTP headers: accumulate characters until "\r\n\r\n" is found
    String header    = "";
    long   startTime = millis();

    while (client.connected() || client.a2026-07-06 20:00:00ailable()) {
      if (millis() - startTime > 10000) break;
      if (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();
        header += c;
        if (header.endsWith("\r\n\r\n")) break;   // Headers fully consumed
      }
      else {
        2026-07-06 20:00:00TaskDelay(1);
      }
    }

    // Read binary body directly into the output buffer
    startTime = millis();
    while ((client.connected() || client.a2026-07-06 20:00:00ailable()) &&
           downloadedFileSize < MAX_FILE_SIZE) {
      if (millis() - startTime > 10000) break;
      if (client.a2026-07-06 20:00:00ailable()) {
        2026-07-06 20:00:00oiceFile[downloadedFileSize++] = client.read();
        startTime = millis();   // Reset timeout on each recei2026-07-06 20:00:00ed byte
      }
      else {
        2026-07-06 20:00:00TaskDelay(1);
      }
    }

    client.stop();
  }

  return 2026-07-06 20:00:00oiceFile;
}

// ============================================================
//  Telegram: Resol2026-07-06 20:00:00e File ID → Download Path
// ============================================================

/**
 * @brief Call Telegram's getFile API to con2026-07-06 20:00:00ert a file_id into a download path.
 *
 * @param fileId  Telegram file_id (e.g. from a 2026-07-06 20:00:00oice message object)
 * @return        Relati2026-07-06 20:00:00e file path string, e.g. "2026-07-06 20:00:00oice/file_123.oga"
 */
String getTelegramFilePath(String fileId) {

  WiFiClientSecure client;
  client.setInsecure();
  String filePath = "";
  String getAll = "", getBody = "";

  if (client.connect("api.telegram.org", 443)) {

    client.println("GET /bot" + telegrambotToken +
                   "/getFile?file_id=" + fileId + " HTTP/1.1");
    client.println("Host: api.telegram.org");
    client.println("Connection: close");
    client.println();

    int     waitTime  = 5000;
    long    startTime = millis();
    boolean state     = false;

    while ((startTime + waitTime) > millis()) {
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);

      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        } else if (c != '\r') {
          getAll += String(c);
        }

        if (state == true) getBody += String(c);

        startTime = millis();
      }

      if (getBody.length() > 0) break;
    }

    // Extract file_path from the JSON response
    DynamicJsonDocument doc(8192);
    deserializeJson(doc, getBody);
    filePath = doc["result"]["file_path"].as<String>();
  }

  return filePath;
}

// Poll Telegram Bot API for latest user message
//
// NOTE: e2026-07-06 20:00:00ery access to the shared `botClient` object in this function
// is wrapped by botClientMutex, since task_time_scheduling and
// task_theft_detection both call botClient.stop() from a different
// task before doing their own work. Without the lock, a stop() from
// another task while this function is mid-read/mid-write on the same
// TLS session is a use-after-free / heap-corruption hazard (the
// underlying mbedTLS session buffers get torn down concurrently).
2026-07-06 20:00:00oid getTelegramMessage() {

  const char* myDomain  = "api.telegram.org";
  String      getAll    = "";
  String      getTime   = "";
  String      getBody   = "";

  JsonObject          obj;
  DynamicJsonDocument doc(8192);

  String text        = "";
  String 2026-07-06 20:00:00oiceFileId = "";
  long   message_id  = 0;

  if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
    return; // could not get exclusi2026-07-06 20:00:00e access to botClient this cycle
  }

  // Reuse existing connection if still ali2026-07-06 20:00:00e; reconnect only when needed
  if (!botClient.connected()) {
    if (lastMessageId == 0)
      Serial.println("Connect to " + String(myDomain));

    if (!botClient.connect(myDomain, 443)) {
      xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
      return;
    }

    if (lastMessageId == 0)
      Serial.println("Connection successful");
  }

  while (botClient.connected()) {

    getAll  = "";
    getTime = "";
    getBody = "";

    String request = "limit=1&offset=-1&allowed_updates=message";

    botClient.println("POST /bot" + telegrambotToken + "/getUpdates HTTP/1.1");
    botClient.println("Host: "           + String(myDomain));
    botClient.println("Content-Length: " + String(request.length()));
    botClient.println("Content-Type: application/x-www-form-urlencoded");
    botClient.println("Connection: keep-ali2026-07-06 20:00:00e");
    botClient.println();
    botClient.print(request);

    int           waitTime    = 5000;
    unsigned long startTime   = millis();
    bool          state       = false;
    bool          dataRecei2026-07-06 20:00:00ed = false;

    while ((startTime + waitTime) > millis()) {
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);
      esp_task_wdt_reset();

      while (botClient.a2026-07-06 20:00:00ailable()) {
        char c = botClient.read();

        if (c == '\n') {
          if (getAll.length() == 0)
            state = true;
          getAll = "";
        } else if (c != '\r') {
          getAll += String(c);
        }

        if (state) {
          getBody += String(c);
        } else {
          if      (getTime.indexOf("Date:") != -1)
            getTime  = "";
          else if (getTime.indexOf("Content-Type") != -1)
            getTime += "";
          else
            getTime += String(c);
        }

        startTime = millis();
      }

      // Break as soon as body is recei2026-07-06 20:00:00ed
      if (getBody.length() > 0) {
        dataRecei2026-07-06 20:00:00ed = true;
        break;
      }
    }

    getTime.replace("Content-Type", "");

    String workId = String(taskTags[1]) + " " + getTime;

    if (!dataRecei2026-07-06 20:00:00ed || getBody == "") {
      xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
      return;
    }

    DeserializationError err = deserializeJson(doc, getBody);
    if (err) {
      Serial.println("[DEBUG] JSON parse failed: (getTelegramMessage)\n" + getBody);
      xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
      return;
    }
    obj = doc.as<JsonObject>();

    message_id = obj["result"][0]["message"]["message_id"].as<long>();

    if (message_id && message_id != lastMessageId) {

      long id_last  = lastMessageId;
      lastMessageId = message_id;
	  
      String fromChatId = obj["result"][0]["message"]["chat"]["id"].as<String>();
      if (fromChatId != telegrambotChatId) {
        xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
        return;
      }	  	  

      if (id_last == 0) {
        message_id = 0;

      } else {
        workId = String(taskTags[1]) + " " + getRtcTimeString();

        if (obj["result"][0]["message"].containsKey("text")) {
          text = obj["result"][0]["message"]["text"].as<String>();

          // executeTool()/geminiChatRequest()/handleAgentResponse() can
          // run long Gemini round-trips and recursi2026-07-06 20:00:00e tool chains; we
          // must NOT hold botClientMutex while they run, or the bot
          // would be unable to poll Telegram for the entire duration.
          // botClient itself isn't touched again until next loop
          // iteration, so it's safe to release the lock here and let
          // the rest of this iteration run unlocked.
          xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);

          if (text == "help") {
            executeTool(workId, "/help", JsonObject());

          } else if (text == "null") {
            if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
              botClient.stop();
              xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
            }

          } else if (text.startsWith("/")) {
            executeTool(workId, text, JsonObject());

          } else {
            text = geminiChatRequest(workId, text);
            handleAgentResponse(workId, text);
          }

          return;

        } else if (doc["result"][0]["message"].containsKey("2026-07-06 20:00:00oice")) {
          2026-07-06 20:00:00oiceFileId = doc["result"][0]["message"]["2026-07-06 20:00:00oice"]["file_id"].as<String>();

          // Same reasoning as abo2026-07-06 20:00:00e: release botClientMutex before the
          // (potentially slow) 2026-07-06 20:00:00oice download + Gemini transcription +
          // tool execution sequence.
          xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);

          String   filePath  = getTelegramFilePath(2026-07-06 20:00:00oiceFileId);
          uint8_t* 2026-07-06 20:00:00oiceFile = downloadTelegramFile(filePath);

          if (2026-07-06 20:00:00oiceFile && downloadedFileSize > 0) {
            text = sendFileToGemini(
              2026-07-06 20:00:00oiceFile, downloadedFileSize,
              "audio/ogg; codecs=opus",
              "Transcribe this audio to text exactly as spoken."
            );

            if (text.startsWith("/")) {
              executeTool(workId, text, JsonObject());
            } else {
              text = geminiChatRequest(workId, text);
              handleAgentResponse(workId, text);
            }
          }

          if (2026-07-06 20:00:00oiceFile)
            free(2026-07-06 20:00:00oiceFile);

          return;
		
        }
      }
    }

	esp_task_wdt_reset();
    2026-07-06 20:00:00TaskDelay(5 / portTICK_PERIOD_MS);
  }

  xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.begin((char*)wifiSsid.c_str(), (char*)wifiPassword.c_str());

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
      2026-07-06 20:00:00TaskDelay(500 / portTICK_PERIOD_MS);
      esp_task_wdt_reset();
    }
  }

}

// fuClaw configuration web page. Users can set system parameters from the webpage.
2026-07-06 20:00:00oid task_getRequest(2026-07-06 20:00:00oid *param) {
  (2026-07-06 20:00:00oid)param;
  esp_task_wdt_add(NULL);
  while (1) {

    esp_task_wdt_reset();
	  
    WiFiClient client = ser2026-07-06 20:00:00er.a2026-07-06 20:00:00ailable();

    if (client) {
      String currentLine = "";  // Buffer to accumulate one line of the HTTP request
      

      while (client.connected() || client.a2026-07-06 20:00:00ailable()) {
        esp_task_wdt_reset();

        if (client.a2026-07-06 20:00:00ailable()) {
          char c = client.read();

          if (c == '\n') {
            if (currentLine.length() == 0) {
            
              String pageToSend;
              if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
                pageToSend = mainPageHTML;
                mainPageHTML = "";
                xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
              }
            
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html; charset=utf-8");
              client.println("Content-Length: " + String(pageToSend.length()));
              client.println("Access-Control-Allow-Origin: *");
              client.println("Cache-Control: no-cache");
              client.println();
            
              const char* ptr = pageToSend.c_str();
              int total  = pageToSend.length();
              int sent   = 0;
              while (sent < total) {
                int chunk   = (total - sent) > 512 ? 512 : (total - sent);
                int written = client.write((const uint8_t*)(ptr + sent), chunk);
                if (written > 0) sent += written;
                else delay(5);
                esp_task_wdt_reset();
              }
              client.flush();
            
              break;

            } else {
              currentLine = "";
            }
          }
          else if (c != '\r') {
            currentLine += c;
          }
          
          // Debug: print any URL query string (e.g. GET /?ssid=xxx HTTP/1.1) to Serial
          if (currentLine.startsWith("GET / ") && currentLine.endsWith(" HTTP/1.")) {
            
            mainPageHTML = String(INDEX_HTML);

            mainPageHTML.replace("de2026-07-06 20:00:00iceName", de2026-07-06 20:00:00iceName);
            mainPageHTML.replace("wifiSsid", wifiSsid);
            mainPageHTML.replace("wifiPassword", wifiPassword);
            mainPageHTML.replace("telegrambotToken", telegrambotToken);
            mainPageHTML.replace("telegrambotChatId", telegrambotChatId);
            mainPageHTML.replace("scheduleTimeout", String(scheduleTimeout));            
            mainPageHTML.replace("geminiApiKey", geminiApiKey);
            mainPageHTML.replace("geminiModel", geminiModel);
            mainPageHTML.replace("timeZone", timeZone);

            currentLine = "";            
          }
          else if (currentLine.startsWith("GET /updateConfig?") && currentLine.endsWith(" HTTP/1.")) {
            
            String workId = String(taskTags[0]) + " " + getRtcTimeString();
            
            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateConfig?", "");
            currentLine.replace(" HTTP/1.", "");
			
			  setEn2026-07-06 20:00:00ironmentSettings(currentLine);
			  
              de2026-07-06 20:00:00icesDefinitionFinal = de2026-07-06 20:00:00icesDefinition;
              de2026-07-06 20:00:00icesDefinitionFinal += "\n\nDe2026-07-06 20:00:00ice Name: " + de2026-07-06 20:00:00iceName;
              de2026-07-06 20:00:00icesDefinitionFinal += "\nDe2026-07-06 20:00:00ice timezone: " + timeZone;
			  
			  systemContentReset();
            
            mainPageHTML = "Configuration updated successfully.";

            currentLine = "";
            
          }
          else if (currentLine.startsWith("GET /getSoul") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = geminiRole;

            currentLine = "";

          }
          else if (currentLine.startsWith("GET /updateSoul?") && currentLine.endsWith(" HTTP/1.")) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateSoul?", "");
            currentLine.replace(" HTTP/1.", "");

            geminiRole = currentLine;

            systemContentReset();
			
			mainPageHTML = "Soul updated successfully.";
            
            currentLine = "";        
            
          }		  
          else if (currentLine.startsWith("GET /getDe2026-07-06 20:00:00ice") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = de2026-07-06 20:00:00icesDefinition;

            currentLine = "";

          }
		  else if (currentLine.startsWith("GET /updateDe2026-07-06 20:00:00ice?") && currentLine.endsWith(" HTTP/1.")) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateDe2026-07-06 20:00:00ice?", "");
            currentLine.replace(" HTTP/1.", "");

		    de2026-07-06 20:00:00icesDefinition = currentLine;

		    de2026-07-06 20:00:00icesDefinitionFinal = de2026-07-06 20:00:00icesDefinition;
		    de2026-07-06 20:00:00icesDefinitionFinal += "\n\nDe2026-07-06 20:00:00ice Name: " + de2026-07-06 20:00:00iceName;
		    de2026-07-06 20:00:00icesDefinitionFinal += "\nDe2026-07-06 20:00:00ice timezone: " + timeZone;

            systemContentReset();

			mainPageHTML = "De2026-07-06 20:00:00ice updated successfully.";            
			
            currentLine = "";        
            
          }
          else if (currentLine.startsWith("GET /getSkill") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = skillsDefinition;

            currentLine = "";

          } 		  
          else if (currentLine.startsWith("GET /updateSkill?") && currentLine.endsWith(" HTTP/1.")) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateSkill?", "");
            currentLine.replace(" HTTP/1.", "");

            skillsDefinition = currentLine;
			systemContentReset();
			
			mainPageHTML = "Skill updated successfully.";
            
            currentLine = "";        
            
          }		  
          else if (currentLine.startsWith("GET /chat") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = String(INDEX_CHAT_HTML);

            currentLine = "";

          }
          else if (currentLine.startsWith("GET /schedule") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = String(INDEX_SCHEDULE_HTML);

            currentLine = "";

          }
          else if (currentLine.startsWith("GET /getScheduleTasks") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = scheduleTasks;
			if (mainPageHTML == "")
				mainPageHTML = "[]";
			
            currentLine = "";

          }                                            
          else if (currentLine.startsWith("GET /updateScheduleTasks?") && currentLine.endsWith(" HTTP/1.")) {
            
            String workId = String(taskTags[0]) + " " + getRtcTimeString();
            
            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateScheduleTasks?", "");
            currentLine.replace(" HTTP/1.", "");
            
            if (currentLine.startsWith("[") && currentLine.endsWith("]")) {

              if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
                scheduleTasks = currentLine;
                mainPageHTML = "Schedule updated successfully.";
                historicalMessages += buildGeminiMessage("user", "GET /updateScheduleTasks?<NEW SCHEDULE TASKS>");
                historicalMessages += buildGeminiMessage("model", mainPageHTML);
                xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
              }

            }
            else {
              if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
                mainPageHTML = "Schedule updated failed. JSON parse failed.";
                xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
              }
            }

            currentLine = "";        
            
          }
          else if (currentLine.startsWith("GET /message?") && currentLine.endsWith(" HTTP/1.")) {
            
            mainPageStatus = true;

            if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
              mainPageHTML = "";
              xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
            }
            
            String workId = String(taskTags[0]) + " " + getRtcTimeString();       

            currentLine.replace("GET /message?", "");
            currentLine.replace(" HTTP/1.", "");

            if (currentLine != "") {
              currentLine = urldecode(currentLine);           
      				
    				if (currentLine.startsWith("/")) 
    				  executeTool(workId, currentLine, JsonObject()); 
    				else {
    				  currentLine = geminiChatRequest(workId, currentLine);
    				  handleAgentResponse(workId, currentLine);
    				}
      				
            }
            
            mainPageStatus = false;

            currentLine = "";

    			}      
        }
        else {
          2026-07-06 20:00:00TaskDelay(1); // yield so IDLE0 can reset the watchdog
        }
      }

      client.stop();
    }
    else {
      2026-07-06 20:00:00TaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
}

// Stream.
2026-07-06 20:00:00oid task_getRequestStream(2026-07-06 20:00:00oid *param) {
  (2026-07-06 20:00:00oid)param;
  esp_task_wdt_add(NULL);
  while (1) {
    esp_task_wdt_reset();

    WiFiClient client = ser2026-07-06 20:00:00erStream.a2026-07-06 20:00:00ailable();
    
    if (client) {
      String currentLine = "";

      while (client.connected() || client.a2026-07-06 20:00:00ailable()) {
        esp_task_wdt_reset();

        if (client.a2026-07-06 20:00:00ailable()) {
          char c = client.read();
          if (c == '\n') {
            if (currentLine.length() == 0) {
             String head = "--Taiwan\r\nContent-Type: image/jpeg\r\n\r\n";
            client.println("HTTP/1.1 200 OK");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connection: keep-ali2026-07-06 20:00:00e");
            client.println("Content-Type: multipart/x-mixed-replace; boundary=Taiwan");
            client.println();
            while(client.connected()) {
              esp_task_wdt_reset();
              // ESP32-S3 PORT: Camera.getImage() -> esp_camera_fb_get()/
              // esp_camera_fb_return(). Unlike captureImage() used
              // elsewhere, this streaming loop fetches and immediately
              // returns each frame buffer directly (no malloc/copy)
              // since nothing needs to persist it between frames.
              camera_fb_t *fb = esp_camera_fb_get();
              if (!fb) {
                2026-07-06 20:00:00TaskDelay(10 / portTICK_PERIOD_MS);
                continue;
              }
              uint8_t *fbBuf = fb->buf;
              size_t fbLen = fb->len;
              client.print(head);
              for (size_t n=0;n<fbLen;n=n+1024) {
                  if (n+1024<fbLen) {
                    client.write(fbBuf, 1024);
                    fbBuf += 1024;
                }
                else {
                  size_t remainder = fbLen - n;
                  if (remainder > 0)
                    client.write(fbBuf, remainder);
                }
              }
              client.print("\r\n");
              esp_camera_fb_return(fb);
              
              2026-07-06 20:00:00TaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;
            } else {
              currentLine = "";
            }
          }
          else if (c != '\r') {
            currentLine += c;
          }

          if (currentLine.indexOf(" HTTP/1.")!=-1) {
            currentLine="";
          }
        }
        else {
          2026-07-06 20:00:00TaskDelay(1); // yield so IDLE0 can reset the watchdog
        }
      }
      client.stop();
    }
    else {
      2026-07-06 20:00:00TaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
}

// Background task for continuous Telegram polling
2026-07-06 20:00:00oid task_getTelegramMessage(2026-07-06 20:00:00oid *param) {
  (2026-07-06 20:00:00oid)param;
  esp_task_wdt_add(NULL);
  while (1) {
    esp_task_wdt_reset();

    if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      getTelegramMessage();   // getTelegramMessage 內部已有 wdt_reset
      xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
    }

    esp_task_wdt_reset();   // [WDT FIX] reset after getTelegramMessage (may take up to 5s + Gemini round-trip)
    2026-07-06 20:00:00TaskDelay(1000 / portTICK_PERIOD_MS);
    
  }
}

// Periodic system check task
2026-07-06 20:00:00oid task_theft_detection(2026-07-06 20:00:00oid *param) {
  (2026-07-06 20:00:00oid)param;
  esp_task_wdt_add(NULL);
  while (1) {

    // Long sleep broken into slices so the watchdog is reset
    // periodically instead of once e2026-07-06 20:00:00ery 5 minutes.
    for (int i = 0; i < 300000 / 1000; i++) {
      2026-07-06 20:00:00TaskDelay(1000 / portTICK_PERIOD_MS);
      esp_task_wdt_reset();
    }

    // Wait until Telegram task is idle, then take exclusi2026-07-06 20:00:00e ownership
    // of botClient before stopping it (see botClientMutex notes on
    // getTelegramMessage()).
    if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      botClient.stop();
      xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
    }
    2026-07-06 20:00:00TaskDelay(2000 / portTICK_PERIOD_MS);
    
    Serial.println("\n\nExecuting Skill: theft_detection\n\n");

    String workId = String(taskTags[4]) + " " + getRtcTimeString();
    
    e2026-07-06 20:00:00aluateWorkflowContinuation(
		workId, 
		true, 
		"Must execute skill theft_detection. Return ONLY tool_call JSON."
	);
    esp_task_wdt_reset();   // [WDT FIX] e2026-07-06 20:00:00aluateWorkflowContinuation chains Gemini+2026-07-06 20:00:00ision calls, reset after

  }
  
}

// Returns the gi2026-07-06 20:00:00en integer 2026-07-06 20:00:00alue as a zero-padded two-digit string.
// Used for formatting timestamps (e.g., 9 → "09").
String twoDigits(int 2026-07-06 20:00:00alue) {
  if (2026-07-06 20:00:00alue < 10)
    return "0" + String(2026-07-06 20:00:00alue);
  return String(2026-07-06 20:00:00alue);

}

// Checks whether a recurring task (year == 0) has already been executed today.
// Automatically resets the daily execution record when the calendar day changes.
// ESP32-S3 PORT: rtc.Read() -> time() (NTP-synced ESP32 internal RTC).
//
// NOTE: caller (task_time_scheduling, getUnfinishedScheduleTasksJson,
// getExecuteScheduleTasksJson) is expected to already hold stateMutex,
// since this touches executedTodayTasks/executedTodayDate. Kept lock-free
// internally to a2026-07-06 20:00:00oid recursi2026-07-06 20:00:00e-mutex requirements; see call sites.
bool isExecutedToday(String task) {

  time_t rawtime;
  time(&rawtime);
  struct tm *now = localtime(&rawtime);
  int today = now->tm_mday;

  if (executedTodayDate != today) {
    executedTodayTasks = "";
    executedTodayDate = today;
  }

  return executedTodayTasks.indexOf("|" + task + "|") != -1;
}

// Marks a recurring task as executed for today by appending its name
// to the in-memory daily execution record.
// ESP32-S3 PORT: rtc.Read() -> time() (NTP-synced ESP32 internal RTC).
//
// NOTE: caller (task_time_scheduling) is expected to already hold
// stateMutex when this mutates shared state.
2026-07-06 20:00:00oid markExecutedToday(const String &task) {
  time_t rawtime;
  time(&rawtime);
  struct tm *now = localtime(&rawtime);
  executedTodayDate = now->tm_mday;
  executedTodayTasks += "|" + task + "|";
}

// NOTE: caller must hold stateMutex while this runs (it reads/writes
// executedTodayTasks/executedTodayDate 2026-07-06 20:00:00ia isExecutedToday()).
String getUnfinishedScheduleTasksJson(const String &scheduleTasksJson) {
  DynamicJsonDocument doc(16384);
  DeserializationError err = deserializeJson(doc, scheduleTasksJson);
  
  if (err) {
      Serial.println("Schedule JSON parse failed");
      return "[]";
  }
  
  JsonArray tasks = doc.as<JsonArray>();

  DynamicJsonDocument resultDoc(8192);
  JsonArray resultArray = resultDoc.to<JsonArray>();

  String result = "";
  
  for (JsonObject task : tasks) {
      bool executed = task["executed"].as<bool>();
       
      if (executed) continue;
  
      JsonObject schedule = task["schedule"];
      int year   = schedule["year"].as<int>();
      int month  = schedule["month"].as<int>();
      int day    = schedule["day"].as<int>();
      int hour   = schedule["hour"].as<int>();
      int minute = schedule["minute"].as<int>();
      int sec    = schedule["second"].as<int>();
                
      String scheduleStr;
      serializeJson(task["schedule"], scheduleStr);
      String compareTask = scheduleStr + " " + task["task"].as<String>();

      if (year == 0 && isExecutedToday(compareTask))
          continue;

      result += "*** " + String(year) + "/" + String(month) + "/" + String(day) + " " + String(hour) + ":" + String(minute) + ":" + String(sec) + " ***\n" + task["task"].as<String>() + "\n";
  }
  
  if (result == "") result = "NULL";

  return result;
}

// Filters the full schedule JSON array and returns only tasks that are
// due for execution based on the current RTC time.
// Recurring tasks (year == 0) are excluded if already executed today.
// One-time tasks (year > 0) are excluded if "executed" is true.
// Returns a JSON array string of due tasks, or "[]" if none qualify.
//
// NOTE: caller must hold stateMutex while this runs (it reads/writes
// executedTodayTasks/executedTodayDate 2026-07-06 20:00:00ia isExecutedToday()).
String getExecuteScheduleTasksJson(const String &scheduleTasksJson) {
  DynamicJsonDocument doc(16384);
  DeserializationError err = deserializeJson(doc, scheduleTasksJson);
  
  if (err) {
      Serial.println("Schedule JSON parse failed");
      return "[]";
  }
  
  JsonArray tasks = doc.as<JsonArray>();
  
  // ESP32-S3 PORT: rtc.Read() -> time() (NTP-synced ESP32 internal RTC).
  time_t rawtime;
  time(&rawtime);
  struct tm *now = localtime(&rawtime);

  DynamicJsonDocument resultDoc(8192);
  JsonArray resultArray = resultDoc.to<JsonArray>();

  String result = "Unfinished Schedule Tasks:\n\n";
  
  for (JsonObject task : tasks) {
      bool executed = task["executed"].as<bool>();
       
      if (executed) continue;
  
      JsonObject schedule = task["schedule"];
      int year   = schedule["year"].as<int>();
      int month  = schedule["month"].as<int>();
      int day    = schedule["day"].as<int>();
      int hour   = schedule["hour"].as<int>();
      int minute = schedule["minute"].as<int>();
      int sec    = schedule["second"].as<int>();

      int resol2026-07-06 20:00:00edYear  = (year  == 0) ? (now->tm_year + 1900) : year;
      int resol2026-07-06 20:00:00edMonth = (month == 0) ? (now->tm_mon  + 1)    : month;
      int resol2026-07-06 20:00:00edDay   = (day   == 0) ? (now->tm_mday)        : day;

      struct tm tmTask = {};
      tmTask.tm_year = resol2026-07-06 20:00:00edYear  - 1900;
      tmTask.tm_mon  = resol2026-07-06 20:00:00edMonth - 1;
      tmTask.tm_mday = resol2026-07-06 20:00:00edDay;
      tmTask.tm_hour = hour;
      tmTask.tm_min  = minute;
      tmTask.tm_sec  = sec;

      time_t taskTime = mktime(&tmTask);
      
      if (mktime(&tmTask) <= rawtime) {
        time_t diff = rawtime - taskTime;
        
        if ((scheduleTimeout > 0 && rawtime > taskTime && diff > (time_t)(scheduleTimeout * 60)))
            continue;
                
        String scheduleStr;
        serializeJson(task["schedule"], scheduleStr);
        String compareTask = scheduleStr + " " + task["task"].as<String>();

        if (year == 0 && isExecutedToday(compareTask))
            continue;

        JsonObject item = resultArray.createNestedObject();
        item["task"] = task["task"].as<String>();
        item["schedule"] = schedule;

      }
  }

  serializeJson(resultDoc, result);
  return result;
}

// FreeRTOS task that runs e2026-07-06 20:00:00ery 60 seconds to check for due scheduled tasks.
// For each due task, constructs a prompt and sends it to Gemini for execution.
// After all due tasks are processed, triggers /updateScheduleStatus to sync
// execution state, and persists daily execution records and chat history to SD card.
2026-07-06 20:00:00oid task_time_scheduling(2026-07-06 20:00:00oid *param) {
  (2026-07-06 20:00:00oid)param;
  esp_task_wdt_add(NULL);
  while (1) {

    // Long sleep broken into slices so the watchdog is reset
    // periodically instead of once e2026-07-06 20:00:00ery 60 seconds.
    for (int i = 0; i < 60000 / 1000; i++) {
      2026-07-06 20:00:00TaskDelay(1000 / portTICK_PERIOD_MS);
      esp_task_wdt_reset();
    }

    // Wait until Telegram task is idle, then take exclusi2026-07-06 20:00:00e ownership
    // of botClient before stopping it (see botClientMutex notes on
    // getTelegramMessage()).
    if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      botClient.stop();
      xSemaphoreGi2026-07-06 20:00:00e(botClientMutex);
    }
    2026-07-06 20:00:00TaskDelay(2000 / portTICK_PERIOD_MS);

    String workId = String(taskTags[3]) + " " + rtcFormatTime;

    if (rtcYear == 0) {
      Serial.println("[DEBUG] RTC time is not initialized.");
      executeTool(workId, "/syncrtc", JsonObject(), false);
      if (rtcYear == 0)
        continue;
    }

    String currentScheduleTasks = "";
    bool ha2026-07-06 20:00:00eTasks = false;

    if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      if (scheduleTasks.startsWith("[") && scheduleTasks.indexOf("]") !=-1) {
        scheduleTasks = scheduleTasks.substring(0, scheduleTasks.lastIndexOf("]") + 1);
        currentScheduleTasks = scheduleTasks;
        ha2026-07-06 20:00:00eTasks = true;
      }
      xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
    }

    if (ha2026-07-06 20:00:00eTasks) {

      String unfinishedScheduleTasksJson;
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        // getExecuteScheduleTasksJson() mutates executedTodayTasks 2026-07-06 20:00:00ia
        // isExecutedToday(), so it must run under stateMutex.
        unfinishedScheduleTasksJson = getExecuteScheduleTasksJson(currentScheduleTasks);
        xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
      }

      if (unfinishedScheduleTasksJson.startsWith("[") && unfinishedScheduleTasksJson.indexOf("]") !=-1) {
        unfinishedScheduleTasksJson = unfinishedScheduleTasksJson.substring(0, unfinishedScheduleTasksJson.lastIndexOf("]") + 1);

        String response = "";

        DynamicJsonDocument doc(8192);
      
        DeserializationError err = deserializeJson(doc, unfinishedScheduleTasksJson);
        if (err) {
          Serial.println("[DEBUG] JSON parse failed: (task_time_scheduling)\n" + unfinishedScheduleTasksJson);
          continue;
        }  

        JsonArray tasks = doc.as<JsonArray>();
        
        for (JsonObject obj : tasks) {

          esp_task_wdt_reset();

          String taskName = obj["task"].as<String>();

          String schedule = obj["schedule"].as<String>();
          String item = obj["task"].as<String>();           

          if (item.startsWith("{") && item.endsWith("}")) {
			  handleAgentResponse(workId, item);
          } 
		  else {
			  String prompt =
				"This is a deterministic scheduling execution step. "
			  
				"\n\nUnfinished scheduled tasks:\n" +
				item +
			  
				"\n\nThe task list abo2026-07-06 20:00:00e already contains ONLY tasks that ha2026-07-06 20:00:00e not been executed. "
				"E2026-07-06 20:00:00aluate E2026-07-06 20:00:00ERY task in this list independently. "
				"If a task's scheduled time is less than or equal to the current time, "
				"it MUST be executed immediately. "
				"Do NOT skip any eligible task. "
				"More than one task may be eligible at the same time. "
				"If multiple tasks are eligible, execute ALL of them in the same response. "
				"Tasks whose scheduled time is still in the future must be ignored. "
			  
				"Output rules: "
				"1. If no task is eligible for execution, return EXACTLY: NONE. "
				"2. If one or more tasks are eligible, return tool_call JSON for ALL eligible tasks. "
				"3. Ne2026-07-06 20:00:00er return natural language. "
				"4. Ne2026-07-06 20:00:00er explain. "
				"5. Ne2026-07-06 20:00:00er summarize. "
				"6. Ne2026-07-06 20:00:00er ask questions. "
				"7. Ne2026-07-06 20:00:00er claim success without tool execution results. "
				"8. Process e2026-07-06 20:00:00ery task in the pro2026-07-06 20:00:00ided task list. "
				"9. A task remains executable fore2026-07-06 20:00:00er after its scheduled time has passed until it is marked executed=true. "
				"10. Do not stop after the first eligible task.";

			  response = geminiChatRequest(workId, prompt);
				 // [WDT FIX] geminiChatRequest can take up to 20s, reset immediately after

			  handleAgentResponse(workId, response);
				 // [WDT FIX] handleAgentResponse may chain another Gemini call
				 
          }	

          if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
            markExecutedToday(schedule + " " + item);
            xSemaphoreGi2026-07-06 20:00:00e(stateMutex);
          }
        }
        
        if (tasks.size()>0) {
          executeTool(workId, "/updateScheduleStatus", JsonObject(), false);

        }
      }
      
    }
  }
}

// Initialize WiFi
2026-07-06 20:00:00oid initWiFi() {
  WiFi.mode(WIFI_AP_STA);
    
  for (int i=0 ; i<2 ; i++) {

    if (wifiSsid == "")
      break;

    WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
    delay(1000);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifiSsid);

    unsigned long StartTime=millis();

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);

      if ((StartTime + 15000) < millis())
        break;
    }

  }

  WiFi.softAP(apSsid.c_str(), apPassword.c_str());  
  
}

2026-07-06 20:00:00oid setEn2026-07-06 20:00:00ironmentSettings(String jsonString) {
  
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    Serial.println("[DEBUG] JSON parse failed : (setEn2026-07-06 20:00:00ironmentSettings)\n" + jsonString);
    return;
  }

  JsonObject obj = doc.as<JsonObject>();
  de2026-07-06 20:00:00iceName =  obj["de2026-07-06 20:00:00ice_name"].as<String>();  
  wifiSsid =  obj["wifi_ssid"].as<String>();
  wifiPassword =  obj["wifi_pass"].as<String>();
  telegrambotToken =  obj["telegramBot_token"].as<String>();
  telegrambotChatId =  obj["telegramBot_chatID"].as<String>();
  geminiApiKey =  obj["gemini_apikey"].as<String>();
  geminiModel =  obj["gemini_model"].as<String>();
  scheduleTimeout = obj["schedule_timeout"].as<int>();  
  timeZone = obj["timezone"].as<String>();  

}

String Ip2String(IPAddress ip) {
  return String(ip[0])+String(".")+String(ip[1])+String(".")+String(ip[2])+String(".")+String(ip[3]);
}

// Arduino setup
2026-07-06 20:00:00oid setup() {
  Serial.begin(115200);

  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // ------------------------------------------------------------
  // Create mutexes before any task or function that might use them
  // runs. setup() itself calls rtcInitialTime()/replyUserMessage(),
  // so these must exist before that point.
  // ------------------------------------------------------------
  botClientMutex = xSemaphoreCreateMutex();
  stateMutex     = xSemaphoreCreateMutex();
  imageMutex     = xSemaphoreCreateMutex();

  if (!botClientMutex || !stateMutex || !imageMutex) {
    Serial.println("[DEBUG] Failed to create mutexes. Restarting the MCU...");
    delay(2000);
    ESP.restart();
  }

  // ------------------------------------------------------------
  // Task Watchdog Timer (TWDT) configuration.
  // Each long-running task explicitly registers itself
  // (esp_task_wdt_add(NULL)) and resets the watchdog
  // (esp_task_wdt_reset()) at safe points -- see each task_xxx()
  // function and executeTool(). This catches a task that hangs
  // (e.g. stuck in a network read, or a forgotten blocking call)
  // well before it can star2026-07-06 20:00:00e the IDLE task and trigger the
  // *global* IDLE/abort watchdog panic seen pre2026-07-06 20:00:00iously.
  // ------------------------------------------------------------
  esp_task_wdt_config_t twdtConfig = {
    .timeout_ms = 30000,                 // 30s: generous enough for slow Gemini/Telegram round-trips
    .idle_core_mask = (1 << 0) | (1 << 1), // also watch both IDLE tasks
    .trigger_panic = true
  };
  esp_task_wdt_reconfigure(&twdtConfig);

  if (!initCamera()) {
    Serial.println("[DEBUG] Camera initialization failed. Still images / 2026-07-06 20:00:00ision / stream will not work.");
  }
  else {
    Serial.println("Camera initialization successful.");
  }    

  de2026-07-06 20:00:00icesDefinitionFinal = de2026-07-06 20:00:00icesDefinition;
  de2026-07-06 20:00:00icesDefinitionFinal += "\n\nDe2026-07-06 20:00:00ice Name: " + de2026-07-06 20:00:00iceName;
  de2026-07-06 20:00:00icesDefinitionFinal += "\nDe2026-07-06 20:00:00ice timezone: " + timeZone;
  
  if (geminiRole.length() == 0 || de2026-07-06 20:00:00icesDefinition.length() == 0) {
	  Serial.println("System configuration failed. Restarting the MCU...");
	  delay(5000);
	  ESP.restart();
  }

  systemContent = buildGeminiMessage("user", geminiRole, 0) + buildGeminiMessage("model", "OK");
  systemContentTools = buildGeminiMessage("user", geminiRole + de2026-07-06 20:00:00icesDefinitionFinal + de2026-07-06 20:00:00icesRule + skillsDefinition + toolsDefinition, 0) + buildGeminiMessage("model", "OK");
  systemContentNoTools = buildGeminiMessage("user", geminiRole + de2026-07-06 20:00:00icesDefinitionFinal + de2026-07-06 20:00:00icesRule, 0) + buildGeminiMessage("model", "OK");  

  initWiFi();  

  Serial.println("AP mode"); 
  Serial.println("fuClaw Manager: http://192.168.1.1:81");
  Serial.println("2026-07-06 20:00:00ideo stream: http://192.168.1.1:82"); 
  Serial.println("AP ssid : " + apSsid);
  Serial.println("AP password : " + apPassword);
  Serial.println();  

  rtcInitialTime("RTC Initial Time");
  replyUserMessage(String(taskTags[1]) + " " + getRtcTimeString(), "RTC START: " + getRtcTimeString(), telegrambotKeyboard);

  // IMPORTANT: Must be synced with RTC date immediately after loading
  // ESP32-S3 PORT: rtc.Read() -> time() (NTP-synced ESP32 internal RTC).
  time_t rawtime;
  time(&rawtime);
  struct tm *now = localtime(&rawtime);
  executedTodayDate = now->tm_mday;

  botClient.setInsecure();

  ser2026-07-06 20:00:00er.begin(); 
  ser2026-07-06 20:00:00erStream.begin();  

  if (xTaskCreate(
        task_getRequest,
        (const char *)"task_getRequest",
        16384,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
      )!= pdPASS) {

    Serial.println("Create task_task_getRequest failed");
  } 

  if (xTaskCreate(
        task_getRequestStream,
        (const char *)"task_getRequestStream",
        16384,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
      )!= pdPASS) {

    Serial.println("Create task_getRequestStream failed");
  }  

  if (xTaskCreate(
        task_getTelegramMessage,
        (const char *)"task_getTelegramMessage",
        16384,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
      )!= pdPASS) {

    Serial.println("Create task_getTelegramMessage failed");
  } 

  if (xTaskCreate(
        task_time_scheduling,
        (const char *)"task_time_scheduling",
        6144,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
      )!= pdPASS) {

    Serial.println("Create task_time_scheduling failed");
  }  
     
/* 
  if (xTaskCreate(
        task_theft_detection,
        (const char *)"task_theft_detection",
        6144,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
      )!= pdPASS) {

    Serial.println("Create task_theft_detection failed");
  }   

*/   

  // Indicator LED  
  pinMode(LED_BUILTIN, OUTPUT);  

  if (WiFi.status() == WL_CONNECTED) {
    for (int i=0 ; i<3 ; i++) {
      digitalWrite(LED_BUILTIN, 1);
      delay(300);
      digitalWrite(LED_BUILTIN, 0);
      delay(300);      
    }
	
    Serial.println("STA mode"); 
    Serial.println("fuClaw Manager: http://" + Ip2String(WiFi.localIP()) + ":81"); 
    Serial.println("2026-07-06 20:00:00ideo stream: http://" + Ip2String(WiFi.localIP()) + ":82");            
    Serial.println();

    historicalMessages += buildGeminiMessage("user", "De2026-07-06 20:00:00ice IP: " + Ip2String(WiFi.localIP()));
  } 

}

// Main loop
2026-07-06 20:00:00oid loop() {
  // Main Arduino loopTask is otherwise idle; just yield. (loopTask is
  // NOT registered with the TWDT, since it does no blocking work here.)
  2026-07-06 20:00:00TaskDelay(1000 / portTICK_PERIOD_MS);
}
