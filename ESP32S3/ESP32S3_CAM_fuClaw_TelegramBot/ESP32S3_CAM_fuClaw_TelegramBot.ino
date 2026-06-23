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
Version
------------------------------------------------------------
Prompt-Orchestrated Embedded Agent Edition
Persistent Filesystem Runtime
ESP32-S3 Port (ESP32-S3-WROOM-CAM board)

Build Date: 2026-06-23 00:00:00

------------------------------------------------------------
Overview
------------------------------------------------------------
fuClaw is an embedded multimodal AI agent framework, run on
ESP32-S3 (camera-equipped boards).

It combines:
- Telegram Bot API (HTTPS long polling)
- Gemini Chat Web Interface
- Google Gemini generateContent API
- Gemini grounded web search
- Gemini multimodal vision reasoning
- Prompt-driven JSON tool routing
- GPIO digital / analog I/O control
- Camera capture and image upload
- Persistent conversation memory
- FreeRTOS concurrent task scheduling

The runtime acts as a hybrid autonomous agent:
Conversation + Reasoning + Tools + Vision + Memory + Hardware
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
(Chat / Search / Vision / Workflow)
      ↓
JSON tool_call output
      ↓
ArduinoJson validation
      ↓
Tool Dispatcher
      ↓
Hardware / Search / Vision Execution
      ↓
Result injection into memory
      ↓
Natural language reply
------------------------------------------------------------
Execution Model
------------------------------------------------------------
This is a prompt-orchestrated tool-routing system.

Gemini does NOT use native function-calling APIs.

Instead:
- Gemini emits structured JSON tool_call responses
- Local firmware validates all tool calls
- Invalid JSON is rejected
- Execution is strictly sequential
- Hardware actions are never simulated

Atomic execution rule:
One response may perform only ONE hardware action:
- one pin
- one operation
- one value

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
/still                    Capture image
/vision                   Capture + multimodal analysis
/search                   Grounded web search
/delay                    Pause execution for specified milliseconds
/getMemory                Runtime memory diagnostics
/getLog                   Show tool execution history
/reset                    Reset conversation state
/chat                     Natural language reply
/reboot                   Reboot the device
/schedule                 Add scheduled tasks
/getSchedule              Get all scheduled tasks
/getUnfinishedSchedule    Get unfinished scheduled tasks
/updateScheduleStatus     Update the executed status of scheduled tasks
/modifySchedule           Modify or delete scheduled tasks
/clearSchedule            Clear scheduled tasks
/tcpSendMessage           Send a message to another device or agent over TCP
/telegramSendMessage      Send a message to Telegram Bot
/lineSendMessage          Send a message to Line Bot
------------------------------------------------------------
Persistent Files
------------------------------------------------------------
env.json
  WiFi / Telegram / Gemini credentials / Time zone

device.md
  Devices definition

skill.md
  Skills definition

soul.md
  Custom assistant personality prompt

memory.md
  Conversation history persistence

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

Conversation state is restored automatically on boot.
------------------------------------------------------------
Hardware Safety
------------------------------------------------------------
Confirmed device mappings carried over from the Ameba Pro2
original. VERIFY AGAINST YOUR OWN BOARD before relying on them.

ESP32-S3-WROOM-1-N16R8
GPIO_SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48
ADC: 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
PWM: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,38,39,40,41,42,43,44,45,46,47,48

Unknown hardware mappings require clarification.
GPIO values are strictly validated before execution.
------------------------------------------------------------
Software Stack (ESP32-S3 port)
------------------------------------------------------------
- WiFi.h (ESP32 Arduino core)
- WiFiClientSecure
- ArduinoJson
- FreeRTOS (built into ESP32 Arduino core)
- esp_camera.h (ESP32 Camera driver)
- Local Base64 helper (no external dependency)
------------------------------------------------------------
Known Limitations
------------------------------------------------------------
- Conversation history grows over time
- String-heavy heap fragmentation risk
- Vision encoding is CPU intensive
- Large JSON parsing impacts heap usage
- Gemini response format handled by ArduinoJson validation layer
- Recursive tool chaining controlled via reCheck flag and NONE sentinel
------------------------------------------------------------
*/

// System settings
#include "config.h"

// System prompt that defines assistant behavior.
// Must be JSON-safe (avoid invalid escape characters or unsupported symbols).
#include "GeminiRole.h"
#include "devicesDefinition.h"
#include "toolsDefinition.h"
#include "skillsDefinition.h"

// Configuration manager
#include "index_html.h"
// Gemini chat
#include "index_chat_html.h"
// Schedule manager
#include "index_schedule_html.h" 

// Array of task-related tags used as stop markers when parsing text
// Every tag MUST be enclosed in angle brackets '<' and '>'.
const char* taskTags[] = { "<PAGE>", "<BOT>", "<MQTT>", "<TIME_SCHEDULING>", "<THEFT_DETECTION>" };

String mainPageHTML = "";
bool mainPageStatus = false;

// Maximum download buffer size for Telegram voice files (256 KB)
#define MAX_FILE_SIZE 262144

// Actual number of bytes downloaded from Telegram
size_t downloadedFileSize = 0;

// Serialized system prompt content used as the initial conversation context
String systemContent = "";
String systemContentTools = "";
String systemContentNoTools = "";

// Logs each tool execution as a human-readable record for /getLog command
String executeToolHistory = "";
  
// Stores entire chat history in Gemini API JSON format
// Used to preserve conversation memory across requests
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
// NOTE: setInsecure() disables certificate verification, matching the
// original Ameba WiFiSSLClient behavior (no cert pinning). For production
// use consider supplying a root CA with botClient.setCACert(...).
WiFiClientSecure botClient;

WiFiServer server(81);
WiFiServer serverStream(82);

#include "Base64.h"
#include <ArduinoJson.h>
#include "FreeRTOS.h"
#include "task.h"
#include "esp_camera.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

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
#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

// Forward declarations
String getUnfinishedScheduleTasksJson(const String &scheduleTasksJson);
String getExecuteScheduleTasksJson(const String &scheduleTasksJson);
String buildGeminiMessage(String role, String message, bool comma);
String getRtcTimeString(bool filename);
void replyUserMessage(String workId, String text, String keyboard);
void handleAgentResponse(String workId, String message);
String geminiChatRequest(String workId, String message, int tools);

// Captured image buffer address and length
uint32_t imageAddress = 0;
uint32_t imageLength = 0;

// Initializes the ESP32 camera driver. Called once from setup().
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
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;           
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
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
// imageAddress/imageLength (freeing any previous buffer first), then
// returns the frame buffer to the camera driver. This preserves the
// original semantics where imageAddress/imageLength can be reused by
// later code (e.g. replyUserImage with frames=false) without needing
// the camera driver's internal buffer to stay valid.
void captureImage() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[DEBUG] Camera capture failed");
    return;
  }

  if (imageAddress != 0) {
    free((void*)imageAddress);
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
    result.reserve(input.length());
    for (int i = 0; i < (int)input.length(); i++) {
        if (input[i] == '%' && i + 2 < (int)input.length()) {
            char hex[3] = { input[i+1], input[i+2], '\0' };
            uint8_t val = (uint8_t)strtol(hex, nullptr, 16);
            result.concat((char)val);
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

  if (client.connect("generativelanguage.googleapis.com", 443)) {
    client.println("POST /v1beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.1");
    client.println("Connection: close");
    client.println("Host: generativelanguage.googleapis.com");
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
      vTaskDelay(100 / portTICK_PERIOD_MS);

      while (client.available()){
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
    getDatetime = "Use grounded search to retrieve the current GMT date and time.";
  }

  if (getDatetime == "") {
    getDatetime = "Use grounded search to retrieve the current GMT date and time.";
  }

  return getDatetime;
  
}

// Returns the current local time as a formatted string.
// ESP32-S3 PORT: reads from the ESP32 internal RTC (kept in sync by NTP,
// see rtcInitialTime() below) via the standard time() call, replacing
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

// Initialize / synchronize the ESP32-S3 internal clock via NTP.
// ESP32-S3 PORT: replaces the original Ameba hardware RTC initialization
// (which asked Gemini for the current datetime and wrote it into a
// dedicated RTC chip). ESP32-S3 has no standalone RTC chip, so this
// function performs a standard NTP sync instead. The function name and
// call sites (setup(), /syncrtc tool) are kept unchanged.
void rtcInitialTime(String workName) {

  rtcUpdateStatus = true;

  long gmtOffsetSec = timeZoneToGmtOffsetSec(timeZone);
  const long daylightOffsetSec = 0;
  const char* ntpServer1 = "pool.ntp.org";
  const char* ntpServer2 = "time.google.com";

  configTime(gmtOffsetSec, daylightOffsetSec, ntpServer1, ntpServer2);

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
      vTaskDelay(100 / portTICK_PERIOD_MS);
	  
      while (client.available())  {
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
    client.println("POST /v2/bot/message/push HTTP/1.1");
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
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length()==0) state=true;
           getAll = "";
        }
        else if (c != '\r')
          getAll += String(c);
          if (state==true) getBody += String(c);
          startTime = millis();
        }
        if (getBody.length()!= 0) break;
      }
      client.stop();
  }
  else {
    getBody="Connected to api.line.me failed.";
  }
  
  return getBody;
}

// Capture a still image from camera and upload it to Telegram as JPEG.
String telegramSendCapturedImage(String token, String chat_id, bool frames) {
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";
  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect(myDomain, 443)) {

    if (frames)
      captureImage();
    else if (!frames && imageLength == 0) {
      client.stop();
      return "Previous image does not exist";
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
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
      }
    }

    client.print(tail);

    int waitTime = 10000;
    unsigned long startTime = millis();
    bool state = false;

    while ((startTime + waitTime) > millis()) {
      vTaskDelay(100 / portTICK_PERIOD_MS);

      while (client.available()) {
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

  return getBody;
}

// Cleans a text string by removing timestamps, workId, and truncating at any task tag
// Returns "NONE" if the text is empty or explicitly marked as none
String removeTimestamps(String workId, String timestamps, String text) {

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
    // Everything from the tag onward is discarded
    for (const auto& tag : taskTags) {
        int pos = text.indexOf(tag);
        if (pos != -1)
            text = text.substring(0, pos); // Keep only the part before the tag
    }

    return text;
}

void replyUserMessage(String workId, String text, String keyboard = "") {
	if (text.length() == 0 || text.startsWith("NONE")) return;
	
	if (workId.startsWith(String(taskTags[0])))
		mainPageHTML += text +"\n";
	else
		telegramSendMessage(telegrambotToken, telegrambotChatId, text, keyboard);
}

String replyUserImage(String workId, bool frames) {
  if (workId.startsWith(String(taskTags[0]))) {
      if (frames)
          captureImage();
          
      uint8_t* fbBuf = (uint8_t*)imageAddress;
      size_t   fbLen = imageLength;

      char *input = (char *)fbBuf;
      char output[base64_enc_len(3)];
                  
      size_t estimatedSize = 23 + ((fbLen + 2) / 3) * 4 + 1;
      String imageFile = "<img src='data:image/jpeg;base64,";
      imageFile.reserve(estimatedSize);
      
      for (int i = 0; i < fbLen; i++) {
          base64_encode(output, (input++), 3);
          if (i % 3 == 0) imageFile += String(output);
      }
      mainPageHTML = imageFile + "' style='max-width:240px; height:auto; border-radius:8px;'><br>";
	  
	  return "Image file created.";
  }
  else
    return telegramSendCapturedImage(telegrambotToken, telegrambotChatId, frames);

  return "";
}

// Convert role/content pair into Gemini-compatible JSON message object
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

//   Send a message to another device or agent over TCP
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

    while ((client.connected() || client.available()) && millis() < timeout) {
      while (client.available()) {
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
      vTaskDelay(1);
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
    "\"reason\":\"Connected to the device failed.\","
    "\"workId\":\"" + workId + "\"}";
      
}

// Reset conversation memory to initial system prompt state
void geminiChatReset() {

  historicalMessages = "";
  executeToolHistory = "";
  
  systemContent = buildGeminiMessage("user", geminiRole, false) + buildGeminiMessage("model", "OK");
  systemContentTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule + skillsDefinition + toolsDefinition, false) + buildGeminiMessage("model", "OK");
  systemContentNoTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule, false) + buildGeminiMessage("model", "OK");
  
}

// Reset system Content
void systemContentReset() {

  systemContent = buildGeminiMessage("user", geminiRole, false) + buildGeminiMessage("model", "OK");
  systemContentTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule + skillsDefinition + toolsDefinition, false) + buildGeminiMessage("model", "OK");
  systemContentNoTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule, false) + buildGeminiMessage("model", "OK");
  
}

// Send request to Gemini and return response text
String geminiChatRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();
  
  historicalMessages += buildGeminiMessage("user", message + timestamps);

  String contents = "";
  if (tools == 0)
    contents = systemContentNoTools + historicalMessages;
  else if (tools == 1)
    contents = systemContentTools + historicalMessages;
  else if (tools == 2)
    contents = systemContent + buildGeminiMessage("user", message);
  else
	contents = systemContent + buildGeminiMessage("user", message);

  String request = "{\"contents\": [" + contents +
                   "],\"generationConfig\": {\"maxOutputTokens\": " +
                   geminiMaxOutputTokens +
                   ", \"temperature\": " + geminiTemperature + "}}";

  WiFiClientSecure client;
  client.setInsecure();
  String responseText = "";
	  
  client.setTimeout(10000);
		  
  if (client.connect("generativelanguage.googleapis.com", 443)) {

    client.println("POST /v1beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: generativelanguage.googleapis.com");
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

    while ((client.connected() || client.available()) && millis() < timeout) {
      while (client.available()) {
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
      vTaskDelay(1);
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
  
  responseText = removeTimestamps(workId, timestamps, responseText);
  
  historicalMessages += buildGeminiMessage("model", responseText + timestamps);

  return responseText;
  
}

// Send Gemini request with Google Search tool enabled
String geminiSearchRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();  
  
  historicalMessages += buildGeminiMessage("user", message + timestamps);

  String contents = systemContent + buildGeminiMessage("user", message);
  if (tools == 1)
    contents = systemContentTools + historicalMessages;
  else if (tools == 0)
    contents = systemContentNoTools + historicalMessages;

  // Build request with Google Search tool
  String request = "{\"contents\": [" + contents +
                   "],\"tools\": [{\"google_search\": {}}],\"generationConfig\": {\"maxOutputTokens\": " +
                   geminiMaxOutputTokens +
                   ", \"temperature\": " + geminiTemperature + "}}";

  WiFiClientSecure client;
  client.setInsecure();
  String responseText = "";
	  
  client.setTimeout(10000);
	
  if (client.connect("generativelanguage.googleapis.com", 443)) {

    // Send HTTP Request
    client.println("POST /v1beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: generativelanguage.googleapis.com");
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

    while ((client.connected() || client.available()) && millis() < timeout) {
      while (client.available()) {
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
      vTaskDelay(1);
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

  responseText = removeTimestamps(workId, timestamps, responseText);
  
  historicalMessages += buildGeminiMessage("model", responseText + timestamps);

  return responseText;
}

// Capture camera frame and send it to Gemini Vision for multimodal analysis
String geminiVisionRequest(String workId, String message, bool frames = true) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();
  
  historicalMessages += buildGeminiMessage("user", message + timestamps);

  WiFiClientSecure client;
  client.setInsecure();
  String responseText = "";
  const char* myDomain = "generativelanguage.googleapis.com";
	  
  client.setTimeout(10000);
	
  if (client.connect(myDomain, 443)) {

    if (frames)
      captureImage();
    else if (!frames && imageLength == 0) {
      client.stop();
      
      responseText = "Previous image does not exist";
      historicalMessages += buildGeminiMessage("model", responseText + timestamps);

      return responseText;
    }
    
    uint8_t *fbBuf = (uint8_t*)imageAddress;
    size_t fbLen = imageLength;

    char *input = (char *)fbBuf;
    char output[base64_enc_len(3)];
    String imageFile = "";
    
    for (size_t i = 0; i < fbLen; i++) {
      base64_encode(output, (input++), 3);
      if (i % 3 == 0) imageFile += String(output);
    }

    String Data = "{\"contents\": [{\"parts\": [{\"text\": \"" + message + 
                  "\"}, {\"inline_data\": {\"mime_type\":\"image/jpeg\",\"data\":\"" + 
                  imageFile + "\"}}]}]}";

    client.println("POST /v1beta/models/"+geminiModel+":generateContent?key="+geminiApiKey+" HTTP/1.0");
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

    while ((client.connected() || client.available()) && millis() < timeout) {
      while (client.available()) {
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
      vTaskDelay(1);
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
      responseText = "[DEBUG] Gemini API (Vision) Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
	  responseText = "Gemini API (Vision) Error";
    } 
    else {
      responseText = "Unexpected response from Gemini Vision.";
    }

  } else {
    Serial.println("Failed to connect to Gemini API (Vision)");
    responseText = "Connection failed";
  }

  if (responseText == "") {
    responseText = "Gemini Vision did not respond. Please try again.";
  }

  responseText = removeTimestamps(workId, timestamps, responseText);
  
  historicalMessages += buildGeminiMessage("model", responseText + timestamps);

  return responseText;
}

// Get current memory usage information
String getMemoryInfo() {
  String msg = "";

  msg += "Free heap: ";
  msg += String(xPortGetFreeHeapSize());

  msg += "\nMin heap: ";
  msg += String(xPortGetMinimumEverFreeHeapSize());

  msg += "\nHistorical messages len: ";
  msg += String(historicalMessages.length());

  return msg;
}

// Control device output using digital or analog mode.
// This function supports general-purpose actuators such as LED, relay, and other GPIO-controlled devices.
String toolPinOutput(int pin, String mode, int value, String workId) {

    pinMode(pin, OUTPUT);

    mode.toLowerCase();

    if (mode == "digitalwrite") {

        if (value != 0 && value != 1) {
            return 
				"{\"status\":\"error\","
				"\"method\":\"/digitalwrite\","				
				"\"reason\":\"invalid_digital_value\","
				"\"workId\":\"" + workId + "\"}";
        }

        digitalWrite(pin, value);

        return
            "{\"status\":\"success\","
            "\"method\":\"/digitalwrite\","
			"\"workId\":\"" + workId + "\"}";

    }
    else if (mode == "analogwrite") {

        if (value < 0 || value > 255) {
            return 
				"{\"status\":\"error\","
				"\"method\":\"/analogwrite\","				
				"\"reason\":\"invalid_analog_value\","
				"\"workId\":\"" + workId + "\"}";
        }

        analogWrite(pin, value);

        return
            "{\"status\":\"success\","
            "\"method\":\"/analogwrite\","
			"\"workId\":\"" + workId + "\"}";

    }

    return
        "{\"status\":\"error\","
        "\"method\":\"/analogwrite\","		
        "\"reason\":\"invalid_output_mode\","
		"\"workId\":\"" + workId + "\"}";
}

// Read device input using digital or analog mode.
// This function supports general-purpose sensors such as buttons and analog sensors connected to GPIO pins.
String toolPinInput(int pin, String mode, String workId) {

    pinMode(pin, INPUT);

    mode.toLowerCase();

    if (mode == "digitalread") {

        int value = digitalRead(pin);

        return
            "{\"status\":\"success\","
            "\"method\":\"/digitalread\","
            "\"value\":" + String(value) + ","
			"\"workId\":\"" + workId + "\"}";

    }
    else if (mode == "analogread") {

        int value = analogRead(pin);

        return
            "{\"status\":\"success\","
            "\"method\":\"/analogread\","
            "\"value\":" + String(value) + ","
			"\"workId\":\"" + workId + "\"}";

    }

    return
        "{\"status\":\"error\","
        "\"method\":\"/analogread\","		
        "\"reason\":\"invalid_input_mode\","
		"\"workId\":\"" + workId + "\"}";
}

// Ask Gemini to re-check whether the current workflow is complete.
// Optionally provide the original user task for context-aware continuation.
// Executes returned tool calls automatically via handleAgentResponse().
void evaluateWorkflowContinuation(String workId, bool reCheck, String task = "") {

    if (!reCheck) return;

    String prompt =
        "Analyze the execution result and determine whether the workflow is complete.\n";

    if (task != "") {
        prompt += "User task request:\n" + task + "\n\n";
    }

    prompt +=
        "If additional hardware actions are strictly required, "
        "return ONLY a valid tool_call JSON.\n"
        "If the workflow is already complete, return EXACTLY: NONE.\n"
        "If no tool action is required and a user-facing reply is needed, "
        "respond naturally in the user's language.\n"
        "Avoid repeating the same meaning as your immediately previous response during the same workflow. If a new workflow or task begins, normal responses are allowed even if similar to previous ones.\n"
        "Do not include explanation or extra text.";

    handleAgentResponse(workId, geminiChatRequest(workId, prompt));
}

// Execute tool commands returned by Gemini
void executeTool(String workId, String command, JsonObject params, bool reCheck = true) {
    String timestamps = "\n" + workId;

    if (command == "/digitalwrite"||command == "/analogwrite") {
      int pin = params["pin"].as<int>();
      String pinmode = params["pinmode"].as<String>();
      int value = params["value"].as<int>();
      
      String response = toolPinOutput(pin, pinmode, value, workId);
    
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + " [ "+String(pin)+" | "+pinmode+" | "+String(value)+" ]\n";	  

      evaluateWorkflowContinuation(workId, reCheck);
    
    } 
    else if (command == "/digitalread" || command == "/analogread") {
      int pin = params["pin"].as<int>();
      String pinmode = params["pinmode"].as<String>();

      String response = toolPinInput(pin, pinmode, workId);

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + " [ "+String(pin)+" | "+pinmode+" ]\n";	  

      evaluateWorkflowContinuation(workId, reCheck); 
      
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
		
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + " [ "+frames+" | "+task+" ]\n";

      evaluateWorkflowContinuation(workId, reCheck, task);
      
    } 
    else if (command == "/syncrtc") {
      rtcInitialTime(workId);
      String rtcTimeResponse = "RTC START: " + getRtcTimeString();
      replyUserMessage(workId, rtcTimeResponse);

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", rtcTimeResponse + timestamps);

      executeToolHistory += workId + " " + command + "\n";

    } 
    else if (command == "/getrtc") {
      String rtcTime = getRtcTimeString();
      replyUserMessage(workId, rtcTime);

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", rtcTime + timestamps);

      executeToolHistory += workId + " " + command + "\n";
              
    }
  	else if (command == "/schedule") {
      String task = params["task"].as<String>();
	  
      String response = "";
	    if (task.startsWith("[") && task.indexOf("]") !=-1) {
		    task = task.substring(0, task.lastIndexOf("]") + 1);
  			if (scheduleTasks == "")
  				scheduleTasks = task;
  			else {
  				scheduleTasks += ", " + task;
  
        String prompt = 
          "Merge all given JSON arrays into a single valid JSON array. "
          "Output ONLY the merged array. "
          "Ensure the result is valid JSON starting with [ and ending with ]. "
          "For every object in the arrays, keep all fields unchanged. "
          "The value of the task field MUST remain exactly as provided. "
          "Never translate, rewrite, summarize, localize, or modify task descriptions. "
          "Task descriptions MUST remain in the original user language.\n\n"
          + scheduleTasks;
  				  
  				String jsonArray = geminiChatRequest(workId, prompt, -1);
  				
  				if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
  				  jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);
  				  scheduleTasks = jsonArray;
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
			"\"reason\":\"Invalid JSON array format.\","
			"\"workId\":\""+workId+"\"}";	  
  	  }   

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + "\n";

      evaluateWorkflowContinuation(workId, reCheck);
    
  	}	
    else if (command == "/modifySchedule") {
      String task = params["task"].as<String>();
            
      String response = "";
      
      String prompt =
          "You are given a JSON array of scheduled tasks and a user-approved schedule modification request. "
          "Apply the requested modification or deletion to the scheduled tasks. "
          "Rules: "
          "- Match tasks using both schedule time and task description. "
          "- If the request is to modify a task, update only the requested fields. "
          "- Modifying a task includes changing the task description, schedule time, or recurrence settings. "
          "- Any modified task MUST have its executed field set to false. "
          "- If the request is to delete a task, remove the matching task from the array. "
          "- Do NOT modify unrelated tasks. "
          "- Preserve all fields of unaffected tasks. "
          "- Preserve the executed field of unaffected tasks. "
          "- Do NOT add new fields. "
          "- Do NOT remove existing fields except when deleting a task. "
          "- Preserve the original JSON schema. "
          "- If no matching task exists, return the original array unchanged. "
          "- Output ONLY the updated JSON array. "
          "- The result MUST start with [ and end with ]. "
          "- Do NOT output explanations, markdown, code fences, or natural language.\n\n"
          "Current scheduled tasks:\n" +
          scheduleTasks +
          "\n\nUser-approved modification request:\n" +
          task;
            
      String jsonArray = geminiChatRequest(workId, prompt);
      
      if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
        jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);
    
        scheduleTasks = jsonArray;
        
        response = 
          "{\"status\":\"success\","
          "\"method\":\"/modifySchedule\","
          "\"workId\":\""+workId+"\"}";     
      }
      else {
        response =
        "{\"status\":\"error\","
        "\"method\":\"/modifySchedule\","
        "\"reason\":\"Invalid JSON array format.\","
        "\"workId\":\""+workId+"\"}";
      }  

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + "\n";

      evaluateWorkflowContinuation(workId, reCheck);
     
    }    
    else if (command == "/updateScheduleStatus") {
      String response = "";
      
      String prompt =
          "You are given a JSON array of scheduled tasks and a tool execution history. "
          "For each task: "
          "- If the task's schedule has \"year\" equal to 0, it is a recurring task. Do NOT change its \"executed\" field. "
          "- Otherwise, set \"executed\" to true ONLY if the task's corresponding action appears in the execution history as successfully completed, otherwise Do NOT change its \"executed\" field. "
          "Output ONLY the updated JSON array. "
          "The result MUST start with [ and end with ]. "
          "Do NOT change any other fields.\n\n"
          + scheduleTasks;
            
      String jsonArray = geminiChatRequest(workId, prompt);
      
      if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
        jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);
		
        scheduleTasks = jsonArray;
        
        response = 
          "{\"status\":\"success\","
          "\"method\":\"/updateScheduleStatus\","
		  "\"workId\":\""+workId+"\"}";		  
      }
      else {
        response =
        "{\"status\":\"error\","
        "\"method\":\"/updateScheduleStatus\","
        "\"reason\":\"Invalid JSON array format.\","
        "\"workId\":\""+workId+"\"}";
      }  

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + "\n";

      evaluateWorkflowContinuation(workId, reCheck);
     
    }
    else if (command == "/getSchedule") {
      String prompt =
        "Please organize the following scheduled tasks and respond in the user's current language. "
        "Present the information in a clear and well-structured bullet-point format for better readability: "
        + scheduleTasks;

      String response = geminiChatRequest(workId, prompt);
      replyUserMessage(workId, response); 
          
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + "\n";
     
    }    
    else if (command == "/getUnfinishedSchedule") {
      if (scheduleTasks.startsWith("[") && scheduleTasks.indexOf("]") !=-1)
            scheduleTasks = scheduleTasks.substring(0, scheduleTasks.lastIndexOf("]") + 1);
            
      String response = getUnfinishedScheduleTasksJson(scheduleTasks);
      replyUserMessage(workId, response);

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + "\n";
 
    }
    else if (command == "/clearSchedule") {
      scheduleTasks = "";
      executedTodayTasks = "";
      
      String response = "Scheduled tasks have been cleared.";
      replyUserMessage(workId, response);

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);

      executeToolHistory += workId + " " + command + "\n";           
    }
    else if (command == "/reset") {
      geminiChatReset();  
            
      String response = "New chat started.";
      replyUserMessage(workId, response);

    } 
    else if (command == "/getMemory") {
      String msg = getMemoryInfo();
      replyUserMessage(workId, msg);

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", msg + timestamps);

      executeToolHistory += workId + " " + command + "\n";

      evaluateWorkflowContinuation(workId, reCheck);          

    } 
    else if (command == "/getLog") {
      Serial.println("\n\nExecute tools history:\n\n"+executeToolHistory+"\n\n");
      replyUserMessage(workId, "Please check the serial monitor to view the tool execution log.");

      executeToolHistory += workId + " " + command + "\n";
      
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
	  
      executeToolHistory += workId + " " + command + " [ "+query+" | "+task+" ]\n";
      
      evaluateWorkflowContinuation(workId, reCheck, task);

    } 
    else if (command == "/delay") {
      long milliseconds = params["milliseconds"].as<long>();
      milliseconds = constrain(milliseconds, 0, 10000);
  
      unsigned long start = millis();
  
      while (millis() - start < milliseconds) {
          vTaskDelay(10 / portTICK_PERIOD_MS);
      }
  
      executeToolHistory += workId + " " + command + " [ " + String(milliseconds) + " ]\n";
  
      evaluateWorkflowContinuation(workId, reCheck);
        
    } 
    else if (command == "/vision") {
      String query = params.containsKey("query") ? params["query"].as<String>() : "Describe the image in detail in the user's language. Do not return bounding boxes or coordinates. Respond in natural language only.";
      bool frames = params.containsKey("frames") ? params["frames"].as<bool>() : true;
      String task = params.containsKey("task") ? params["task"].as<String>() : "NONE";
	  
      String response = geminiVisionRequest(workId, query, frames);
      handleAgentResponse(workId, response);
	  
      executeToolHistory += workId + " " + command + " [ "+query+" | "+frames+" | "+task+" ]\n";
      
      evaluateWorkflowContinuation(workId, reCheck, task);
    }
  	else if (command == "/reboot") {
  	  replyUserMessage(workId, "Rebooting the device, please wait...");

      executeToolHistory += workId + " " + command + "\n";
  		
  	  Serial.println("User requested reboot the device.");
  	  vTaskDelay(2000 / portTICK_PERIOD_MS);
  		
  	  ESP.restart();   // ESP32-S3 PORT: NVIC_SystemReset() -> ESP.restart()
  	}
  	else if (command == "/tcpSendMessage") {
      String device = params["device"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = tcpSendMessage(workId, device, message);
	  
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);	  

      executeToolHistory += workId + " " + command + " [ "+device+" | "+message+" ]\n";

      evaluateWorkflowContinuation(workId, reCheck);
	}
  	else if (command == "/telegramSendMessage") {
      String token = params["token"].as<String>();
	  String chatId = params["chatId"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = telegramSendMessage(token, chatId, message);
	  
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);	  

      executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+chatId+" | "+message+" ]\n";

      evaluateWorkflowContinuation(workId, reCheck);
	}
  	else if (command == "/lineSendMessage") {
      String token = params["token"].as<String>();
	  String targetId = params["targetId"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = lineSendMessage(token, targetId, message);
	  
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);	  

      executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+targetId+" | "+message+" ]\n";

      evaluateWorkflowContinuation(workId, reCheck);
	}	
    else if (command == "/help" || command == "/start") {
         
      String mem = getMemoryInfo();
      String command = systemCommand;
      command.replace("<memory>", mem);
      command = geminiChatRequest(workId, "Reply the following text in the user's language:\n\n" + command);
      
      replyUserMessage(workId, command, telegrambotKeyboard);

      historicalMessages += buildGeminiMessage("user", "Command list" + timestamps);
      historicalMessages += buildGeminiMessage("model", command + timestamps);
      
    }      
    else {
      String response = geminiChatRequest(workId, command);
      handleAgentResponse(workId, response);
      
    }	
}

// Invalid JSON is rejected and logged to Serial.
// No tool execution occurs on malformed payloads.
void handleAgentResponse(String workId, String message) {

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
  if (!client.connect("generativelanguage.googleapis.com", 443)) {
    Serial.println("[STT] Connection to Gemini failed");
    return "Connected to Gemini failed.";
  }

  client.println("POST /v1beta/models/" + geminiModel +
                 ":generateContent?key=" + geminiApiKey + " HTTP/1.1");
  client.println("Host: generativelanguage.googleapis.com");
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

  while (client.connected() && millis() < timeout) {
    while (client.available()) {
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

  uint8_t* voiceFile = (uint8_t*)malloc(MAX_FILE_SIZE);
  if (!voiceFile) return NULL;

  downloadedFileSize = 0;
  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect("api.telegram.org", 443)) {

    // HTTP/1.0 prevents chunked transfer encoding so the body is pure binary
    client.println("GET /file/bot" + telegrambotToken + "/" + filePath + " HTTP/1.0");
    client.println("Host: api.telegram.org");
    client.println("Connection: close");
    client.println();

    // Skip HTTP headers: accumulate characters until "\r\n\r\n" is found
    String header    = "";
    long   startTime = millis();

    while (client.connected() || client.available()) {
      if (millis() - startTime > 10000) break;
      if (client.available()) {
        char c = client.read();
        header += c;
        if (header.endsWith("\r\n\r\n")) break;   // Headers fully consumed
      }
    }

    // Read binary body directly into the output buffer
    startTime = millis();
    while ((client.connected() || client.available()) &&
           downloadedFileSize < MAX_FILE_SIZE) {
      if (millis() - startTime > 10000) break;
      if (client.available()) {
        voiceFile[downloadedFileSize++] = client.read();
        startTime = millis();   // Reset timeout on each received byte
      }
    }

    client.stop();
  }

  return voiceFile;
}

// ============================================================
//  Telegram: Resolve File ID → Download Path
// ============================================================

/**
 * @brief Call Telegram's getFile API to convert a file_id into a download path.
 *
 * @param fileId  Telegram file_id (e.g. from a voice message object)
 * @return        Relative file path string, e.g. "voice/file_123.oga"
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
      vTaskDelay(100 / portTICK_PERIOD_MS);

      while (client.available()) {
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
void getTelegramMessage() {

  const char* myDomain  = "api.telegram.org";
  String      getAll    = "";
  String      getTime   = "";
  String      getBody   = "";

  JsonObject          obj;
  DynamicJsonDocument doc(8192);

  String text        = "";
  String voiceFileId = "";
  long   message_id  = 0;

  // Reuse existing connection if still alive; reconnect only when needed
  if (!botClient.connected()) {
    if (lastMessageId == 0)
      Serial.println("Connect to " + String(myDomain));

    if (!botClient.connect(myDomain, 443))
      return;

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
    botClient.println("Connection: keep-alive");
    botClient.println();
    botClient.print(request);

    int           waitTime    = 5000;
    unsigned long startTime   = millis();
    bool          state       = false;
    bool          dataReceived = false;

    while ((startTime + waitTime) > millis()) {
      vTaskDelay(100 / portTICK_PERIOD_MS);

      while (botClient.available()) {
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

      // Break as soon as body is received
      if (getBody.length() > 0) {
        dataReceived = true;
        break;
      }
    }

    getTime.replace("Content-Type", "");

    String workId = String(taskTags[1]) + " " + getTime;

    if (!dataReceived || getBody == "") return;

    DeserializationError err = deserializeJson(doc, getBody);
    if (err) {
      Serial.println("[DEBUG] JSON parse failed: (getTelegramMessage)\n" + getBody);
      return;
    }
    obj = doc.as<JsonObject>();

    message_id = obj["result"][0]["message"]["message_id"].as<long>();

    if (message_id && message_id != lastMessageId) {

      long id_last  = lastMessageId;
      lastMessageId = message_id;
	  
      String fromChatId = obj["result"][0]["message"]["chat"]["id"].as<String>();
      if (fromChatId != telegrambotChatId) {
        return;
      }	  	  

      if (id_last == 0) {
        message_id = 0;

      } else {
        workId = String(taskTags[1]) + " " + getRtcTimeString();

        if (obj["result"][0]["message"].containsKey("text")) {
          text = obj["result"][0]["message"]["text"].as<String>();

          if (text == "help") {
            executeTool(workId, "/help", JsonObject());

          } else if (text == "null") {
            botClient.stop();

          } else if (text.startsWith("/")) {
            executeTool(workId, text, JsonObject());

          } else {
            text = geminiChatRequest(workId, text);
            handleAgentResponse(workId, text);
          }

        } else if (doc["result"][0]["message"].containsKey("voice")) {
          voiceFileId = doc["result"][0]["message"]["voice"]["file_id"].as<String>();

          String   filePath  = getTelegramFilePath(voiceFileId);
          uint8_t* voiceFile = downloadTelegramFile(filePath);

          if (voiceFile && downloadedFileSize > 0) {
            text = sendFileToGemini(
              voiceFile, downloadedFileSize,
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

          if (voiceFile)
            free(voiceFile);
		
        }
      }
    }

    vTaskDelay(5 / portTICK_PERIOD_MS);
  }

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.begin((char*)wifiSsid.c_str(), (char*)wifiPassword.c_str());

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
      vTaskDelay(500 / portTICK_PERIOD_MS);
  }

}

// fuClaw configuration web page. Users can set system parameters from the webpage.
void task_getRequest(void *param) {
  (void)param;
  while (1) {
	  
    WiFiClient client = server.available();

    if (client) {
      String currentLine = "";  // Buffer to accumulate one line of the HTTP request
      

      while (client.connected()) {
        if (client.available()) {
          char c = client.read();

          if (c == '\n') {
            if (currentLine.length() == 0) {
            
              String pageToSend = mainPageHTML;
              mainPageHTML = "";
            
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
          if ((currentLine.indexOf("GET / ") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {
            
            mainPageHTML = String(INDEX_HTML);
			
            mainPageHTML.replace("deviceName", deviceName);
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
          else if ((currentLine.indexOf("GET /updateConfig?") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {
            
            String workId = String(taskTags[0]) + " " + getRtcTimeString();
            
            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateConfig?", "");
            currentLine.replace(" HTTP/1.", "");
            
            if (currentLine.startsWith("{") && currentLine.endsWith("}")) {
              
              mainPageHTML = "Configuration updated successfully.";
              executeTool(workId, "/reboot", JsonObject());
              
            }
            else
              mainPageHTML = "Configuration updated failed. JSON parse failed.";

            currentLine = "";
            
          }
          else if ((currentLine.indexOf("GET /getSoul") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = geminiRole;

            currentLine = "";

          }
          else if ((currentLine.indexOf("GET /updateSoul?") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateSoul?", "");
            currentLine.replace(" HTTP/1.", "");
            
            geminiRole = currentLine;
            systemContentReset();
            
            currentLine = "";        
            
          }		  
          else if ((currentLine.indexOf("GET /getDevice") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = devicesDefinition;

            currentLine = "";

          }
		      else if ((currentLine.indexOf("GET /updateDevice?") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateDevice?", "");
            currentLine.replace(" HTTP/1.", "");
            
            devicesDefinition = currentLine;
			
            devicesDefinitionFinal = devicesDefinition;
            devicesDefinitionFinal += "\n\nDevice Name: " + deviceName;
            devicesDefinitionFinal += "\nDevice timezone: " + timeZone;
			
            systemContentReset();            
			
            currentLine = "";        
            
          }
          else if ((currentLine.indexOf("GET /getSkill") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = skillsDefinition;

            currentLine = "";

          } 		  
          else if ((currentLine.indexOf("GET /updateSkill?") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateSkill?", "");
            currentLine.replace(" HTTP/1.", "");
            
            skillsDefinition = currentLine;
            systemContentReset();
            
            currentLine = "";        
            
          }		  
          else if ((currentLine.indexOf("GET /chat") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = String(INDEX_CHAT_HTML);

            currentLine = "";

          }
          else if ((currentLine.indexOf("GET /schedule") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = String(INDEX_SCHEDULE_HTML);

            currentLine = "";

          }
          else if ((currentLine.indexOf("GET /getScheduleTasks") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = scheduleTasks;

            currentLine = "";

          }                                            
          else if ((currentLine.indexOf("GET /updateScheduleTasks?") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {
            
            String workId = String(taskTags[0]) + " " + getRtcTimeString();
            
            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateScheduleTasks?", "");
            currentLine.replace(" HTTP/1.", "");
            
            if (currentLine.startsWith("[") && currentLine.endsWith("]")) {
              scheduleTasks = currentLine;
              
              mainPageHTML = "Schedule updated successfully.";
              
              historicalMessages += buildGeminiMessage("user", "GET /updateScheduleTasks?<NEW SCHEDULE TASKS>");
              historicalMessages += buildGeminiMessage("model", mainPageHTML);  

            }
            else
              mainPageHTML = "Schedule updated failed. JSON parse failed.";

            currentLine = "";        
            
          }
          else if ((currentLine.indexOf("GET /message?") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {
            
            mainPageStatus = true;

            mainPageHTML = "";
            
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
          vTaskDelay(1); // yield so IDLE0 can reset the watchdog
        }
      }

      client.stop();
    }
    else {
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
}

// Stream.
void task_getRequestStream(void *param) {
  (void)param;
  while (1) {
    WiFiClient client = serverStream.available();
    
    if (client) {
      String currentLine = "";

      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          if (c == '\n') {
            if (currentLine.length() == 0) {
             String head = "--Taiwan\r\nContent-Type: image/jpeg\r\n\r\n";
            client.println("HTTP/1.1 200 OK");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connection: keep-alive");
            client.println("Content-Type: multipart/x-mixed-replace; boundary=Taiwan");
            client.println();
            while(client.connected()) {
              // ESP32-S3 PORT: Camera.getImage() -> esp_camera_fb_get()/
              // esp_camera_fb_return(). Unlike captureImage() used
              // elsewhere, this streaming loop fetches and immediately
              // returns each frame buffer directly (no malloc/copy)
              // since nothing needs to persist it between frames.
              camera_fb_t *fb = esp_camera_fb_get();
              if (!fb) {
                vTaskDelay(10 / portTICK_PERIOD_MS);
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
                else if (fbLen%1024>0) {
                  size_t remainder = fbLen%1024;
                  client.write(fbBuf, remainder);
                }
              }
              client.print("\r\n");
              esp_camera_fb_return(fb);
              
              vTaskDelay(10 / portTICK_PERIOD_MS);
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
          vTaskDelay(1); // yield so IDLE0 can reset the watchdog
        }
      }
      client.stop();
    }
    else {
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
}

// Background task for continuous Telegram polling
void task_getTelegramMessage(void *param) {
  (void)param;
  while (1) {
    
    getTelegramMessage();
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
  }
}

// Periodic system check task
void task_theft_detection(void *param) {
  (void)param;
  while (1) {
	  
    vTaskDelay(300000 / portTICK_PERIOD_MS);
    
    // Wait until Telegram task is idle
    botClient.stop();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    Serial.println("\n\nExecuting Skill: theft_detection\n\n");

    String workId = String(taskTags[4]) + " " + getRtcTimeString();
    
    evaluateWorkflowContinuation(
		workId, 
		true, 
		"Must execute skill theft_detection. Return ONLY tool_call JSON."
	);

  }
  
}

// Returns the given integer value as a zero-padded two-digit string.
// Used for formatting timestamps (e.g., 9 → "09").
String twoDigits(int value) {
  if (value < 10)
    return "0" + String(value);
  return String(value);

}

// Checks whether a recurring task (year == 0) has already been executed today.
// Automatically resets the daily execution record when the calendar day changes.
// ESP32-S3 PORT: rtc.Read() -> time() (NTP-synced ESP32 internal RTC).
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
void markExecutedToday(const String &task) {
  time_t rawtime;
  time(&rawtime);
  struct tm *now = localtime(&rawtime);
  executedTodayDate = now->tm_mday;
  executedTodayTasks += "|" + task + "|";
}

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

      int resolvedYear  = (year  == 0) ? (now->tm_year + 1900) : year;
      int resolvedMonth = (month == 0) ? (now->tm_mon  + 1)    : month;
      int resolvedDay   = (day   == 0) ? (now->tm_mday)        : day;

      struct tm tmTask = {};
      tmTask.tm_year = resolvedYear  - 1900;
      tmTask.tm_mon  = resolvedMonth - 1;
      tmTask.tm_mday = resolvedDay;
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

// FreeRTOS task that runs every 60 seconds to check for due scheduled tasks.
// For each due task, constructs a prompt and sends it to Gemini for execution.
// After all due tasks are processed, triggers /updateScheduleStatus to sync
// execution state, and persists daily execution records and chat history to SD card.
void task_time_scheduling(void *param) {
  (void)param;
  while (1) {
	  
    vTaskDelay(60000 / portTICK_PERIOD_MS);

    // Wait until Telegram task is idle
    botClient.stop();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    String workId = String(taskTags[3]) + " " + rtcFormatTime;

    if (rtcYear == 0) {
      Serial.println("[DEBUG] RTC time is not initialized.");
      executeTool(workId, "/syncrtc", JsonObject(), false);
      if (rtcYear == 0)
        continue;
    }

    if (scheduleTasks.startsWith("[") && scheduleTasks.indexOf("]") !=-1) {
      scheduleTasks = scheduleTasks.substring(0, scheduleTasks.lastIndexOf("]") + 1);
 
      String unfinishedScheduleTasksJson = getExecuteScheduleTasksJson(scheduleTasks);

      if (unfinishedScheduleTasksJson.startsWith("[") && unfinishedScheduleTasksJson.indexOf("]") !=-1) {
        unfinishedScheduleTasksJson = unfinishedScheduleTasksJson.substring(0, unfinishedScheduleTasksJson.lastIndexOf("]") + 1);

        String response = "";

        DynamicJsonDocument doc(8192);
      
        DeserializationError err = deserializeJson(doc, unfinishedScheduleTasksJson);
        if (err) {
          Serial.println("[DEBUG] JSON parse failed: (task_time_scheduling)\n" + unfinishedScheduleTasksJson);
          return;
        }  

        JsonArray tasks = doc.as<JsonArray>();
        
        for (JsonObject obj : tasks) {

          String taskName = obj["task"].as<String>();

          String schedule = obj["schedule"].as<String>();
          String item = obj["task"].as<String>();           

          String prompt =
            "This is a deterministic scheduling execution step. "
          
            "\n\nUnfinished scheduled tasks:\n" +
            item +
          
            "\n\nThe task list above already contains ONLY tasks that have not been executed. "
            "Evaluate EVERY task in this list independently. "
            "If a task's scheduled time is less than or equal to the current time, "
            "it MUST be executed immediately. "
            "Do NOT skip any eligible task. "
            "More than one task may be eligible at the same time. "
            "If multiple tasks are eligible, execute ALL of them in the same response. "
            "Tasks whose scheduled time is still in the future must be ignored. "
          
            "Output rules: "
            "1. If no task is eligible for execution, return EXACTLY: NONE. "
            "2. If one or more tasks are eligible, return tool_call JSON for ALL eligible tasks. "
            "3. Never return natural language. "
            "4. Never explain. "
            "5. Never summarize. "
            "6. Never ask questions. "
            "7. Never claim success without tool execution results. "
            "8. Process every task in the provided task list. "
            "9. A task remains executable forever after its scheduled time has passed until it is marked executed=true. "
            "10. Do not stop after the first eligible task.";

          response = geminiChatRequest(workId, prompt);

          handleAgentResponse(workId, response);

          markExecutedToday(schedule + " " + item);
        }
        
        if (tasks.size()>0) {
          executeTool(workId, "/updateScheduleStatus", JsonObject(), false);

        }
      }
      
    }
  }
}

// Initialize WiFi
void initWiFi() {
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

void setEnvironmentSettings(String jsonString) {
  
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    Serial.println("[DEBUG] JSON parse failed : (setEnvironmentSettings)\n" + jsonString);
    return;
  }

  JsonObject obj = doc.as<JsonObject>();
  deviceName =  obj["device_name"].as<String>();  
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
void setup() {
  Serial.begin(115200);

  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  if (!initCamera()) {
    Serial.println("[DEBUG] Camera initialization failed. Still images / vision / stream will not work.");
  }
  else {
    Serial.println("Camera initialization successful.");
  }    

  devicesDefinitionFinal = devicesDefinition;
  devicesDefinitionFinal += "\n\nDevice Name: " + deviceName;
  devicesDefinitionFinal += "\nDevice timezone: " + timeZone;
  
  if (geminiRole.length() == 0 || devicesDefinition.length() == 0) {
	  Serial.println("System configuration failed. Restarting the MCU...");
	  delay(5000);
	  ESP.restart();
  }

  systemContent = buildGeminiMessage("user", geminiRole, 0) + buildGeminiMessage("model", "OK");
  systemContentTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule + skillsDefinition + toolsDefinition, 0) + buildGeminiMessage("model", "OK");
  systemContentNoTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule, 0) + buildGeminiMessage("model", "OK");  

  initWiFi();  

  Serial.println("AP mode"); 
  Serial.println("fuClaw Manager: http://192.168.1.1:81");
  Serial.println("Video stream: http://192.168.1.1:82"); 
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

  server.begin(); 
  serverStream.begin();  

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
    Serial.println("Video stream: http://" + Ip2String(WiFi.localIP()) + ":82");            
    Serial.println();

    historicalMessages += buildGeminiMessage("user", "Device IP: " + Ip2String(WiFi.localIP()));
  } 

}

// Main loop
void loop() {
}
