/*
------------------------------------------------------------
fuClaw AI Telegram Assistant with Gemini / OpenAI / Grok Integration
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

Build Date: 2026-08-07 00:00:00
------------------------------------------------------------
Overview
------------------------------------------------------------
fuClaw is an embedded multimodal AI agent framework running
on Realtek Ameba Pro2 devices:
- AMB82-mini
- HUB 8735 Ultra

It combines:
- Telegram Bot API (HTTPS long polling)
- LLM Chat Web Interface (Gemini / OpenAI / Grok)
- Google Gemini generateContent API / OpenAI Chat Completions API / Grok Chat Completions API
- Grounded web search (Gemini Google Search / OpenAI web_search / Grok web_search)
- Multimodal vision reasoning (Gemini / OpenAI / Grok)
- Prompt-driven JSON tool routing
- GPIO digital / analog I/O control
- Camera capture and image upload
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
LLM Reasoning Engine (Gemini / OpenAI / Grok)
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

The LLM (Gemini / OpenAI / Grok) does NOT use native function-calling APIs.

Instead:
- The LLM emits structured JSON tool_call responses
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
/still                    Capture a still image and send it to the user.
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
/telegramSendImage        Send a video snapshot to Telegram Bot
/lineSendMessage          Send a message to Line Bot
------------------------------------------------------------
Hardware Safety
------------------------------------------------------------
Confirmed device mappings only.

AMB82-mini
- GPIO SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,22,23,24
- ADC: 9, 10, 11, 21, 22
- PWM: 1, 2, 3, 4, 5, 6, 7, 8, 23
- Green LED : GPIO 24
- Blue LED  : GPIO 23

HUB 8735 Ultra
- Button    : GPIO 12 (input only, active-low)
- GPIO SET: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,22,23,24,25,26
- ADC: 0, 1, 2, 9, 10
- PWM: 11, 12, 13, 19, 20, 21, 22, 23, 24
- Green LED : GPIO 25
- Blue LED  : GPIO 26
- Fill LED  : GPIO 13

External Modules (Confirmed)


Unknown hardware mappings require clarification.
GPIO values are strictly validated before execution.
------------------------------------------------------------
Software Stack
------------------------------------------------------------
- WiFi.h
- WiFiSSLClient
- ArduinoJson
- FreeRTOS
- VideoStream
- Base64
------------------------------------------------------------
Known Limitations
------------------------------------------------------------
- Conversation history grows over time
- String-heavy heap fragmentation risk
- Vision encoding is CPU intensive
- Large JSON parsing impacts heap usage
- LLM response format handled by ArduinoJson validation layer
- Recursive tool chaining controlled via reCheck flag and NONE sentinel
------------------------------------------------------------
*/

// System settings
#include "config.h"

// System prompt that defines assistant behavior.
// Must be JSON-safe (avoid invalid escape characters or unsupported symbols).
#include "llmRole.h"
#include "devicesDefinition.h"
#include "toolsDefinition.h"
#include "skillsDefinition.h"

// Configuration manager
#include "index_html.h"
// LLM chat
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
  
// Stores entire chat history in LLM API JSON format
// Used to preserve conversation memory across requests
String historicalMessages = "";

// Schedule Tasks
String scheduleTasks = "";

String executedTodayTasks = "";
int executedTodayDate = 0;

// Indicator LED output pin
int ledPin = 24;    // green led (AMB82-mini: 24, HUB 8735 Ultra: 25)

// Last Telegram message ID
long lastMessageId = 0;

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

// Maximum ticks to wait when taking a mutex before giving up.
// 10 s is generous enough for the longest LLM round-trip.
#define MUTEX_TIMEOUT_TICKS pdMS_TO_TICKS(10000)

#include <WiFi.h>

// SSL client for secure Telegram polling
WiFiSSLClient botClient;

char channel_ap[] = "2";
WiFiServer server(81);
WiFiServer serverStream(82);

#include "Base64.h"
#include <ArduinoJson.h>
#include "FreeRTOS.h"
#include "task.h"

// Forward declarations
String getUnfinishedScheduleTasksJson(const String &scheduleTasksJson);
String getExecuteScheduleTasksJson(const String &scheduleTasksJson);
String getRtcTimeString(bool filename);
void replyUserMessage(String workId, String text, String keyboard);
void handleAgentResponse(String workId, String message);
String llmChatRequest(String workId, String message, int tools);
String geminiChatRequest(String workId, String message, int tools);
String openaiChatRequest(String workId, String message, int tools);
String grokChatRequest(String workId, String message, int tools);
String llmSearchRequest(String workId, String message, int tools);
String geminiSearchRequest(String workId, String message, int tools);
String openaiSearchRequest(String workId, String message, int tools);
String grokSearchRequest(String workId, String message, int tools);
void setEnvironmentSettings(String jsonString);

#include "VideoStream.h"

// Camera video configuration
VideoSetting config(320, 240, CAM_FPS, VIDEO_JPEG, 1);
//VideoSetting config(VIDEO_VGA, CAM_FPS, VIDEO_JPEG, 1);

// Captured image buffer address and length
uint32_t imageAddress = 0;
uint32_t imageLength = 0;

#include <stdio.h>
#include <time.h>
#include "rtc.h"
struct tm *timeinfo;
int rtcYear = 0;
int rtcMonth = 0;
int rtcDay = 0;
int rtcHour = 0;
int rtcMinute = 0;
int rtcSecond = 0;
String rtcFormatTime = "";
bool rtcUpdateStatus = false;

#define CONFIG_INIC_IPC_HIGH_TP

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

String escapeForJson(const String &src, bool gemini) {
  unsigned int n = src.length();
  const char* buf = src.c_str();

  String out;
  out.reserve(2 * n + 1);

  for (unsigned int i = 0; i < n; i++) {
    char c = buf[i];
    switch (c) {
      case '"':
        out += '\\'; out += '"';
        break;
      case '\\':
        out += '\\'; out += '\\';
        break;
      case '\r':
        if (gemini) out += c;
        else { out += '\\'; out += 'r'; }
        break;
      case '\n':
        if (gemini) out += c;
        else { out += '\\'; out += 'n'; }
        break;
      case '\t':
        if (gemini) out += c;
        else { out += '\\'; out += 't'; }
        break;
      default:
        out += c;
    }
  }
  return out;
}

// Convert role/content pair into Gemini-compatible JSON message object
String buildLlmMessage(String role, String message, bool comma = true) {
  String esc;
  String jsonMessage;

  if (llmType == "gemini") {
    esc = escapeForJson(message, true);
  } 
  else {
    esc = escapeForJson(message, false);
    role.replace("model", "system"); 
  }
  
  jsonMessage.reserve(esc.length() + role.length() + 48);
  
  jsonMessage  = comma ? ", {\"role\": \"" : "{\"role\": \"";
  jsonMessage += role;
  jsonMessage += "\", \"content\": \"";
  jsonMessage += esc;
  jsonMessage += "\" }";
    
  return jsonMessage;
}

// Get current UTC(GMT) datetime from HTTP response "Date:" header
String getGoogleDatetime() {

  WiFiSSLClient client;
  String getDatetime = "";

  if (client.connect("google.com", 443)) {
    client.println("GET / HTTP/1.1");
    client.println("Host: google.com");
    client.println("Connection: close");    
    client.println();

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

String getRtcTimeString(bool filename = false) {

  long long epoch = rtc.Read();

  time_t rawtime = (time_t)epoch;

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

// Initialize the RTC using LLM-synchronized local time.
void rtcInitialTime(String workName) {
	
  rtcUpdateStatus = true;
  
  String prompt =
    "Convert this GMT datetime to " + timeZone + ".\n"
    "GMT datetime: " + getGoogleDatetime() + "\n\n"
	
    "Before generating the JSON output, add exactly 4 seconds to the converted local datetime.\n"
    "Handle minute, hour, day, month, and year rollovers correctly if needed.\n\n" 

    "Output requirements:\n"
    "- Return ONLY a raw JSON object.\n"
    "- Do NOT use markdown.\n"
    "- Do NOT use code fences.\n"
    "- Do NOT explain anything.\n"
    "- Do NOT add extra text.\n"
    "- First character must be {.\n"
    "- Last character must be }.\n\n"

    "Required JSON format:\n"
    "{\n"
    "\"rtcYear\":2026,\n"
    "\"rtcMonth\":5,\n"
    "\"rtcDay\":28,\n"
    "\"rtcHour\":11,\n"
    "\"rtcMinute\":35,\n"
    "\"rtcSecond\":0\n"
    "}";

  String message = llmChatRequest(String(taskTags[1]), prompt, -1);

  if (message.startsWith("{") && message.endsWith("}")) {

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed\n" + message);
      replyUserMessage(workName, "RTC time update failed.", "");
      rtcUpdateStatus = false;
      return;
    }

    JsonObject obj = doc.as<JsonObject>();

    rtcYear   = obj["rtcYear"]   | 0;
    rtcMonth  = obj["rtcMonth"]  | 0;
    rtcDay    = obj["rtcDay"]    | 0;
    rtcHour   = obj["rtcHour"]   | 0;
    rtcMinute = obj["rtcMinute"] | 0;
    rtcSecond = obj["rtcSecond"] | 0;

  } else {
    Serial.println("[DEBUG] JSON parse failed : (rtcInitialTime)\n" + message);
    replyUserMessage(workName, "RTC time update failed.", "");
  }

  rtc.Init();
  long long initTime = rtc.SetEpoch(rtcYear, rtcMonth, rtcDay, rtcHour, rtcMinute, rtcSecond);
  rtc.Write(initTime);
}

// Send text message to Telegram bot
String telegramSendMessage(String token, String chatid, String text, String keyboard = "") {
  text.replace("\\n", "%0A");
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";
  String request = "parse_mode=HTML&chat_id="+chatid+"&text="+text;

  if (keyboard!="")
    request += "&reply_markup="+keyboard;

  WiFiSSLClient client;
  
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

  WiFiSSLClient client;

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
// so the buffer this function is sending can never be freed/replaced by
// a concurrent captureImage() call from another task mid-upload.
String telegramSendCapturedImage(String token, String chat_id, bool frames) {
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";
  WiFiSSLClient client;

  if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
    return "Image buffer busy, please try again.";
  }

  if (client.connect(myDomain, 443)) {

    if (frames)
      Camera.getImage(0, &imageAddress, &imageLength);
    else if (!frames && imageLength == 0) {
      client.stop();
      xSemaphoreGive(imageMutex);
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

  xSemaphoreGive(imageMutex);

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

// NOTE: imageMutex is held for the whole capture + base64-encode +
// publish sequence so the buffer can't be freed/replaced underneath us
// by a concurrent Camera.getImage() call from another task.
String replyUserImage(String workId, bool frames) {
  if (workId.startsWith(String(taskTags[0]))) {

      if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
        return "Image buffer busy, please try again.";
      }

      if (frames)
          Camera.getImage(0, &imageAddress, &imageLength);

      if (imageLength == 0) {
        xSemaphoreGive(imageMutex);
        return "Previous image does not exist";
      }

      uint8_t* fbBuf = (uint8_t*)imageAddress;
      size_t   fbLen = imageLength;

      char *input = (char *)fbBuf;
      char output[base64_enc_len(3)];
                  
      size_t estimatedSize = 23 + ((fbLen + 2) / 3) * 4 + 1;
      String imageFile = "<img src='data:image/jpeg;base64,";
      imageFile.reserve(estimatedSize);
      
      // Advance by 3 bytes per base64_encode() call (it reads 3 input
      // bytes at a time); base64_encode() handles the 1-2 byte tail
      // padding itself when fbLen is not a multiple of 3.
      for (size_t i = 0; i < fbLen; i += 3) {
          base64_encode(output, input, 3);
          input += 3;
          imageFile += String(output);
      }

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        mainPageHTML = imageFile + "' style='max-width:240px; height:auto; border-radius:8px;'><br>";
        xSemaphoreGive(stateMutex);
      }

      xSemaphoreGive(imageMutex);

	  return "Image file created.";
  }
  else
    return telegramSendCapturedImage(telegrambotToken, telegrambotChatId, frames);

  return "";
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
void llmChatReset() {

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages = "";
    executeToolHistory = "";
    systemContent = buildLlmMessage("user", llmRole, false) + buildLlmMessage("model", "OK");
    systemContentTools = buildLlmMessage("user", llmRole + devicesDefinitionFinal + devicesRule + skillsDefinition + toolsDefinition, false) + buildLlmMessage("model", "OK");
    systemContentNoTools = buildLlmMessage("user", llmRole + devicesDefinitionFinal + devicesRule, false) + buildLlmMessage("model", "OK");
    xSemaphoreGive(stateMutex);
  }
  
}

// Reset system Content
void systemContentReset() {

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    systemContent = buildLlmMessage("user", llmRole, false) + buildLlmMessage("model", "OK");
    systemContentTools = buildLlmMessage("user", llmRole + devicesDefinitionFinal + devicesRule + skillsDefinition + toolsDefinition, false) + buildLlmMessage("model", "OK");
    systemContentNoTools = buildLlmMessage("user", llmRole + devicesDefinitionFinal + devicesRule, false) + buildLlmMessage("model", "OK");
    xSemaphoreGive(stateMutex);
  }
  
}

// Send request to Gemini and return response text
String geminiChatRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    else if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 2)
      contents = systemContent + buildLlmMessage("user", message);
    else
      contents = systemContent + buildLlmMessage("user", message);
    xSemaphoreGive(stateMutex);
  }

  String request = "{\"contents\": [" + contents +
                   "],\"generationConfig\": {\"maxOutputTokens\": " +
                   llmMaxOutputTokens +
                   ", \"temperature\": " + llmTemperature + "}}";

  WiFiSSLClient client;
  String responseText = "";
	  
  client.setTimeout(10000);
		  
  if (client.connect("generativelanguage.googleapis.com", 443)) {

    client.println("POST /v1beta/models/"+llmModel+":generateContent?key="+llmKey+" HTTP/1.0");
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
         // [WDT FIX] prevent watchdog timeout during long Gemini response
      vTaskDelay(1);
    }
    
    client.stop();

    int jsonStart = body.indexOf('{'); 
    if (jsonStart != -1) { 
      body = body.substring(jsonStart);
    }
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
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

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;
  
}

// Send Gemini request with Google Search tool enabled
String geminiSearchRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    contents = systemContent + buildLlmMessage("user", message);
    if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    xSemaphoreGive(stateMutex);
  }

  // Build request with Google Search tool
  String request = "{\"contents\": [" + contents +
                   "],\"tools\": [{\"google_search\": {}}],\"generationConfig\": {\"maxOutputTokens\": " +
                   llmMaxOutputTokens +
                   ", \"temperature\": " + llmTemperature + "}}";

  WiFiSSLClient client;
  String responseText = "";
	  
  client.setTimeout(10000);
	
  if (client.connect("generativelanguage.googleapis.com", 443)) {

    // Send HTTP Request
    client.println("POST /v1beta/models/"+llmModel+":generateContent?key="+llmKey+" HTTP/1.0");
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
         // [WDT FIX] prevent watchdog timeout during long Gemini Search response
      vTaskDelay(1);
    }
    
    client.stop();  

    int jsonStart = body.indexOf('{'); 
    if (jsonStart != -1) { 
      body = body.substring(jsonStart);
    }
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (geminiSearchRequest)\n" + body);
      responseText = "JSON parse failed (geminiSearchRequest). Please try again.";
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

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;
}

// Capture camera frame and send it to Gemini Vision for multimodal analysis
//
// NOTE: imageMutex is held for the whole capture + base64-encode +
// HTTP send sequence so the buffer can't be freed/replaced underneath
// us by a concurrent captureImage() call from another task.
String geminiVisionRequest(String workId, String message, bool frames = true) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  WiFiSSLClient client;
  String responseText = "";
  const char* myDomain = "generativelanguage.googleapis.com";
	  
  client.setTimeout(10000);

  if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
    responseText = "Image buffer busy, please try again.";
    if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      historicalMessages += buildLlmMessage("model", responseText + timestamps);
      xSemaphoreGive(stateMutex);
    }
    return responseText;
  }
	
  if (client.connect(myDomain, 443)) {

    if (frames)
      Camera.getImage(0, &imageAddress, &imageLength);
    else if (!frames && imageLength == 0) {
      client.stop();
      xSemaphoreGive(imageMutex);

      responseText = "Previous image does not exist";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", responseText + timestamps);
        xSemaphoreGive(stateMutex);
      }

      return responseText;
    }
    
    uint8_t *fbBuf = (uint8_t*)imageAddress;
    size_t fbLen = imageLength;

    char *input = (char *)fbBuf;
    char output[base64_enc_len(3)];
    String imageFile = "";

    // Advance by 3 bytes per base64_encode() call (it reads 3 input
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
    xSemaphoreGive(imageMutex);

    client.println("POST /v1beta/models/"+llmModel+":generateContent?key="+llmKey+" HTTP/1.0");
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
         // [WDT FIX] prevent watchdog timeout during long Gemini Vision response
      vTaskDelay(1);
    }
    
    client.stop();   

    int jsonStart = body.indexOf('{'); 
    if (jsonStart != -1) { 
      body = body.substring(jsonStart);
    }
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed (geminiVisionRequest):\n" + body);
      responseText = "JSON parse failed (geminiVisionRequest). Please try again.";
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
    xSemaphoreGive(imageMutex);
  }

  if (responseText == "") {
    responseText = "Gemini Vision did not respond. Please try again.";
  }

  responseText = removeTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;
}

// Send request to OpenAI (Chat Completions API) and return response text
String openaiChatRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    else if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 2)
      contents = systemContent + buildLlmMessage("user", message);
    else
      contents = systemContent + buildLlmMessage("user", message);
    xSemaphoreGive(stateMutex);
  }

  String request = "{\"model\":\""+llmModel+"\",\"messages\":[" + contents + "], \"max_completion_tokens\": " + llmMaxOutputTokens + ", \"temperature\": " + llmTemperature + "}";

  WiFiSSLClient client;
  String responseText = "";

  client.setTimeout(10000);

  if (client.connect("api.openai.com", 443)) {

    client.println("POST /v1/chat/completions HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: api.openai.com");
    client.println("Authorization: Bearer " + llmKey);
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
         // [WDT FIX] prevent watchdog timeout during long OpenAI response
      vTaskDelay(1);
    }

    client.stop();

    int jsonStart = body.indexOf('{');
    if (jsonStart != -1) {
      body = body.substring(jsonStart);
    }
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (openaiChatRequest)\n" + body);
      responseText = "JSON parse failed (openaiChatRequest). Please try again.";
    }
    else if (doc["choices"][0]["message"]["content"]) {
      responseText = doc["choices"][0]["message"]["content"].as<String>();
    }
    else if (doc["error"]) {
      responseText = "[DEBUG] OpenAI API Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
      responseText = "OpenAI API Error";
    }
    else {
      responseText = "Unexpected response from OpenAI.";
      Serial.println("Unknown response format.");
    }

  } else {
    Serial.println("Failed to connect to OpenAI API");
    responseText = "Connection failed";
  }

  if (responseText == "") {
    responseText = "OpenAI did not respond. Please try again.";
  }

  responseText = removeTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;

}

// Send a prompt to OpenAI's web-search-capable chat model and return response text.
String openaiSearchRequest(String workId, String message, int tools = 1) {

  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    else if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 2)
      contents = systemContent + buildLlmMessage("user", message);
    else
      contents = systemContent + buildLlmMessage("user", message);
    xSemaphoreGive(stateMutex);
  }

  String request = "{\"model\":\""+llmModel+"\", \"tools\": [{\"type\": \"web_search\"}], \"input\":[" + contents + "], \"max_output_tokens\": " + llmMaxOutputTokens + ", \"temperature\": " + llmTemperature + "}";

  WiFiSSLClient client;
  String responseText = "";

  client.setTimeout(10000);

  if (client.connect("api.openai.com", 443)) {

    client.println("POST /v1/responses HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: api.openai.com");
    client.println("Authorization: Bearer " + llmKey);
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
         // [WDT FIX] prevent watchdog timeout during long OpenAI Search response
      vTaskDelay(1);
    }

    client.stop();

    int jsonStart = body.indexOf('{');
    if (jsonStart != -1) {
      body = body.substring(jsonStart);
    }
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    // Search responses include "annotations" (url_citation) alongside content; a larger
    // buffer than the plain chat request is used to accommodate citation metadata.
    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (openaiSearchRequest)\n" + body);
      responseText = "JSON parse failed (openaiSearchRequest). Please try again.";
    }
    else {
      JsonArray output = doc["output"].as<JsonArray>();

      for (JsonObject item : output) {

        String itemType = item["type"] | "";

        if (itemType != "message")
            continue;

        JsonArray content = item["content"].as<JsonArray>();

        for (JsonObject part : content) {
          String partType = part["type"] | "";
          if (partType == "output_text") {
              responseText += part["text"].as<String>();
              responseText += "\n";
          }
        }
      }
    }

    responseText.trim();
  }

  responseText = removeTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;

}

// Send an image + text prompt to OpenAI's vision-capable chat model and return response text.
String openaiVisionRequest(String workId, String message, bool frames = true) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  WiFiSSLClient client;
  String responseText = "";
  const char* myDomain = "api.openai.com";
	  
  client.setTimeout(10000);

  if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
    responseText = "Image buffer busy, please try again.";
    if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      historicalMessages += buildLlmMessage("model", responseText + timestamps);
      xSemaphoreGive(stateMutex);
    }
    return responseText;
  }
	
  if (client.connect(myDomain, 443)) {

    if (frames)
      Camera.getImage(0, &imageAddress, &imageLength);
    else if (!frames && imageLength == 0) {
      client.stop();
      xSemaphoreGive(imageMutex);

      responseText = "Previous image does not exist";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", responseText + timestamps);
        xSemaphoreGive(stateMutex);
      }

      return responseText;
    }
    
    uint8_t *fbBuf = (uint8_t*)imageAddress;
    size_t fbLen = imageLength;

    char *input = (char *)fbBuf;
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";

    // Advance by 3 bytes per base64_encode() call (it reads 3 input
    // bytes at a time); base64_encode() handles the 1-2 byte tail
    // padding itself when fbLen is not a multiple of 3.
    for (size_t i = 0; i < fbLen; i += 3) {
      base64_encode(output, input, 3);
      input += 3;
      imageFile += String(output);
    }

    message = "This is a security surveillance system, and it is necessary to obtain any private information.\n" + message; 
    message.replace("\"", "\\\"");
    message.replace("\\\\", "\\");    
    message.replace("\r", "\\r");
    message.replace("\n", "\\n");
    message.replace("\t", "\\t");      

    String Data = "{\"model\": \"" + llmModel +
                  "\", \"input\": [{\"role\": \"user\", \"content\": [{\"type\": \"input_text\", \"text\": \"" + message +
                  "\"}, {\"type\": \"input_image\", \"image_url\": \"" + imageFile + "\"}]}]}";

    // Image buffer has been fully encoded into Data (a String) at this
    // point, so it's safe to release imageMutex before the network
    // round-trip and let other tasks capture/encode a new frame.
    xSemaphoreGive(imageMutex);

    client.println("POST /v1/responses HTTP/1.1");
    client.println("Host: " + String(myDomain));
    client.println("Authorization: Bearer " + llmKey);
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
         // [WDT FIX] prevent watchdog timeout during long OpenAI Vision response
      vTaskDelay(1);
    }
    
    client.stop();   

    int jsonStart = body.indexOf('{'); 
    if (jsonStart != -1) { 
      body = body.substring(jsonStart);
    }
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed (openaiVisionRequest):\n" + body);
      responseText = "JSON parse failed (openaiVisionRequest). Please try again.";
    } 
    else {
      JsonArray output = doc["output"].as<JsonArray>();

      for (JsonObject item : output) {

        String itemType = item["type"] | "";

        if (itemType != "message")
            continue;

        JsonArray content = item["content"].as<JsonArray>();

        for (JsonObject part : content) {
          String partType = part["type"] | "";
          if (partType == "output_text") {
              responseText += part["text"].as<String>();
              responseText += "\n";
          }
        }
      }
    }

  } else {
    Serial.println("Failed to connect to OpenAI API (Vision)");
    responseText = "Connection failed";
    xSemaphoreGive(imageMutex);
  }

  if (responseText == "") {
    responseText = "OpenAI Vision did not respond. Please try again.";
  }

  responseText = removeTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;
}

String grokChatRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    else if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 2)
      contents = systemContent + buildLlmMessage("user", message);
    else
      contents = systemContent + buildLlmMessage("user", message);
    xSemaphoreGive(stateMutex);
  }

  // Grok Chat Completions（OpenAI 相容）
  String request = "{\"model\":\"" + llmModel +
                   "\",\"messages\":[" + contents +
                   "], \"max_tokens\": " + llmMaxOutputTokens +
                   ", \"temperature\": " + llmTemperature + "}";

  WiFiSSLClient client;
  String responseText = "";

  client.setTimeout(10000);

  if (client.connect("api.x.ai", 443)) {

    client.println("POST /v1/chat/completions HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: api.x.ai");
    client.println("Authorization: Bearer " + llmKey);
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
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (grokChatRequest)\n" + body);
      responseText = "JSON parse failed (grokChatRequest). Please try again.";
    }
    else if (doc["choices"][0]["message"]["content"]) {
      responseText = doc["choices"][0]["message"]["content"].as<String>();
    }
    else if (doc["error"]) {
      responseText = "[DEBUG] Grok API Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
      responseText = "Grok API Error";
    }
    else {
      responseText = "Unexpected response from Grok.";
      Serial.println("Unknown response format.");
    }

  } else {
    Serial.println("Failed to connect to Grok API");
    responseText = "Connection failed";
  }

  if (responseText == "") {
    responseText = "Grok did not respond. Please try again.";
  }

  responseText = removeTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;
}

String grokSearchRequest(String workId, String message, int tools = 1) {

  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  String contents = "";
  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    if (tools == 0)
      contents = systemContentNoTools + historicalMessages;
    else if (tools == 1)
      contents = systemContentTools + historicalMessages;
    else if (tools == 2)
      contents = systemContent + buildLlmMessage("user", message);
    else
      contents = systemContent + buildLlmMessage("user", message);
    xSemaphoreGive(stateMutex);
  }

  // Grok Responses API + built-in web_search tool
  String request = "{\"model\":\"" + llmModel +
                   "\", \"tools\": [{\"type\": \"web_search\"}], \"input\":[" + contents +
                   "], \"max_output_tokens\": " + llmMaxOutputTokens +
                   ", \"temperature\": " + llmTemperature + "}";

  WiFiSSLClient client;
  String responseText = "";

  client.setTimeout(10000);

  if (client.connect("api.x.ai", 443)) {

    client.println("POST /v1/responses HTTP/1.0");
    client.println("Connection: close");
    client.println("Host: api.x.ai");
    client.println("Authorization: Bearer " + llmKey);
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
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed: (grokSearchRequest)\n" + body);
      responseText = "JSON parse failed (grokSearchRequest). Please try again.";
    }
    else {
      JsonArray output = doc["output"].as<JsonArray>();

      for (JsonObject item : output) {

        String itemType = item["type"] | "";

        if (itemType != "message")
            continue;

        JsonArray content = item["content"].as<JsonArray>();

        for (JsonObject part : content) {
          String partType = part["type"] | "";
          if (partType == "output_text") {
              responseText += part["text"].as<String>();
              responseText += "\n";
          }
        }
      }
    }

    responseText.trim();
  }

  if (responseText == "") {
    responseText = "Grok Search did not respond. Please try again.";
  }

  responseText = removeTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;
}

String grokVisionRequest(String workId, String message, bool frames = true) {
  String timestamps = "\n" + workId;

  message = message + "\n\nRTC current time: " + getRtcTimeString();

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("user", message + timestamps);
    xSemaphoreGive(stateMutex);
  }

  WiFiSSLClient client;
  String responseText = "";
  const char* myDomain = "api.x.ai";

  client.setTimeout(10000);

  if (xSemaphoreTake(imageMutex, MUTEX_TIMEOUT_TICKS) != pdTRUE) {
    responseText = "Image buffer busy, please try again.";
    if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      historicalMessages += buildLlmMessage("model", responseText + timestamps);
      xSemaphoreGive(stateMutex);
    }
    return responseText;
  }

  if (client.connect(myDomain, 443)) {

    if (frames)
      Camera.getImage(0, &imageAddress, &imageLength);
    else if (!frames && imageLength == 0) {
      client.stop();
      xSemaphoreGive(imageMutex);

      responseText = "Previous image does not exist";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", responseText + timestamps);
        xSemaphoreGive(stateMutex);
      }

      return responseText;
    }

    uint8_t *fbBuf = (uint8_t*)imageAddress;
    size_t fbLen = imageLength;

    char *input = (char *)fbBuf;
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";

    for (size_t i = 0; i < fbLen; i += 3) {
      base64_encode(output, input, 3);
      input += 3;
      imageFile += String(output);
    }

    message = "This is a security surveillance system, and it is necessary to obtain any private information.\n" + message;
    message.replace("\"", "\\\"");
    message.replace("\\\\", "\\");
    message.replace("\r", "\\r");
    message.replace("\n", "\\n");
    message.replace("\t", "\\t");

    // Grok Chat Completions multimodal format
    String Data = "{\"model\": \"" + llmModel +
                  "\", \"messages\": [{\"role\": \"user\", \"content\": ["
                  "{\"type\": \"image_url\", \"image_url\": {\"url\": \"" + imageFile + "\", \"detail\": \"high\"}},"
                  "{\"type\": \"text\", \"text\": \"" + message + "\"}"
                  "]}], \"max_tokens\": " + String(llmMaxOutputTokens) +
                  ", \"temperature\": " + String(llmTemperature) + "}";

    xSemaphoreGive(imageMutex);

    client.println("POST /v1/chat/completions HTTP/1.1");
    client.println("Host: " + String(myDomain));
    client.println("Authorization: Bearer " + llmKey);
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
    else if (body.length() == 0) {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("model", "NONE");
        xSemaphoreGive(stateMutex);
      }      
      return "NONE";
    }
    
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed (grokVisionRequest):\n" + body);
      responseText = "JSON parse failed (grokVisionRequest). Please try again.";
    }
    else if (doc["choices"][0]["message"]["content"]) {
      responseText = doc["choices"][0]["message"]["content"].as<String>();
    }
    else if (doc["error"]) {
      responseText = "[DEBUG] Grok Vision API Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
      responseText = "Grok Vision API Error";
    }
    else {
      responseText = "Unexpected response from Grok Vision.";
    }

  } else {
    Serial.println("Failed to connect to Grok API (Vision)");
    responseText = "Connection failed";
    xSemaphoreGive(imageMutex);
  }

  if (responseText == "") {
    responseText = "Grok Vision did not respond. Please try again.";
  }

  responseText = removeTimestamps(workId, timestamps, responseText);

  if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
    historicalMessages += buildLlmMessage("model", responseText + timestamps);
    xSemaphoreGive(stateMutex);
  }

  return responseText;
}

String llmChatRequest(String workId, String message, int tools = 1) {
    if (llmType == "gemini")
        return geminiChatRequest(workId, message, tools);
    else if (llmType == "openai")
        return openaiChatRequest(workId, message, tools);
    else if (llmType == "grok")
        return grokChatRequest(workId, message, tools);
    else
        return "NONE";
}

String llmSearchRequest(String workId, String message, int tools = 1) {
    if (llmType == "gemini")
        return geminiSearchRequest(workId, message, tools);
    else if (llmType == "openai")
        return openaiSearchRequest(workId, message, tools);
    else if (llmType == "grok")
        return grokSearchRequest(workId, message, tools);
    else
        return "NONE";
}

String llmVisionRequest(String workId, String message, bool frames = true) {
    if (llmType == "gemini")
        return geminiVisionRequest(workId, message, frames);
    else if (llmType == "openai")
        return openaiVisionRequest(workId, message, frames);
    else if (llmType == "grok")
        return grokVisionRequest(workId, message, frames);
    else
        return "NONE";
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

// Ask LLM to re-check whether the current workflow is complete.
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
        "Analyze the latest tool execution result.\n"
        "If another hardware action is still required to accomplish the user's request,\n"
        "return ONLY a valid tool_call JSON.\n"
        "Otherwise, if the user should be informed of the result,\n"
        "reply naturally in the user's language.\n"
        "Return EXACTLY \"NONE\" only when:\n"
        "- no further tool_call is needed, and\n"
        "- no user-visible reply is necessary.\n"
        "Do not include explanations or extra text.";

    handleAgentResponse(workId, llmChatRequest(workId, prompt));
}

// Execute tool commands returned by LLM
void executeTool(String workId, String command, JsonObject params, bool reCheck = true) {
    String timestamps = "\n" + workId;

    // Reset the per-task watchdog at each tool boundary so the TWDT
    // doesn't fire during back-to-back multi-step tool chains.
    

    if (command == "/digitalwrite"||command == "/analogwrite") {
      int pin = params["pin"].as<int>();
      String pinmode = params["pinmode"].as<String>();
      int value = params["value"].as<int>();
      
      String response = toolPinOutput(pin, pinmode, value, workId);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+String(pin)+" | "+pinmode+" | "+String(value)+" ]\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);
    
    } 
    else if (command == "/digitalread" || command == "/analogread") {
      int pin = params["pin"].as<int>();
      String pinmode = params["pinmode"].as<String>();

      String response = toolPinInput(pin, pinmode, workId);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+String(pin)+" | "+pinmode+" ]\n";
        xSemaphoreGive(stateMutex);
      }

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

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+frames+" | "+task+" ]\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck, task);
      
    } 	
    else if (command == "/syncrtc") {
      rtcInitialTime(workId);
      String rtcTimeResponse = "RTC START: " + getRtcTimeString();
      replyUserMessage(workId, rtcTimeResponse);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", rtcTimeResponse + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }

    } 
    else if (command == "/getrtc") {
      String rtcTime = getRtcTimeString();
      replyUserMessage(workId, rtcTime);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", rtcTime + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }
              
    }
  	else if (command == "/schedule") {
      String task = params["task"].as<String>();
	  
      String response = "";
      String localSchedule = "";

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        localSchedule = scheduleTasks;
        xSemaphoreGive(stateMutex);
      }

	    if (task.startsWith("[") && task.indexOf("]") !=-1) {
		    task = task.substring(0, task.lastIndexOf("]") + 1);
  			if (localSchedule == "")
  				localSchedule = task;
  			else {
  				localSchedule += ", " + task;
  
				String prompt = 
				  "Merge all given JSON arrays into a single valid JSON array. "
				  "Output ONLY the merged array. "
				  "Ensure the result is valid JSON starting with [ and ending with ]. "
				  "For every object in the arrays, keep all fields unchanged. "
				  "The value of the task field MUST remain exactly as provided. "
				  "Never translate, rewrite, summarize, localize, or modify task descriptions. "
				  "Task descriptions MUST remain in the original user language.\n\n"
				  + localSchedule;
					  
				String jsonArray = llmChatRequest(workId, prompt, -1);
				
				if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
				  jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);
				  localSchedule = jsonArray;
				}
			}
				
			if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
			  scheduleTasks = localSchedule;
			  xSemaphoreGive(stateMutex);
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

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);
    
  	}	
    else if (command == "/modifySchedule") {
      String task = params["task"].as<String>();
            
      String response = "";
      String localSchedule = "";

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        localSchedule = scheduleTasks;
        xSemaphoreGive(stateMutex);
      }
      
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
          localSchedule +
          "\n\nUser-approved modification request:\n" +
          task;
            
      String jsonArray = llmChatRequest(workId, prompt);
      
      if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
        jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);
    
        if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
          scheduleTasks = jsonArray;
          xSemaphoreGive(stateMutex);
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
        "\"reason\":\"Invalid JSON array format.\","
        "\"workId\":\""+workId+"\"}";
      }  

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);
     
    }    
    else if (command == "/updateScheduleStatus") {
      String response = "";
      String localSchedule = "";

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        localSchedule = scheduleTasks;
        xSemaphoreGive(stateMutex);
      }
      
      String prompt =
          "You are given a JSON array of scheduled tasks and a tool execution history. "
          "For each task: "
          "- If the task's schedule has \"year\" equal to 0, it is a recurring task. Do NOT change its \"executed\" field. "
          "- Otherwise, set \"executed\" to true ONLY if the task's corresponding action appears in the execution history as successfully completed, otherwise Do NOT change its \"executed\" field. "
          "Output ONLY the updated JSON array. "
          "The result MUST start with [ and end with ]. "
          "Do NOT change any other fields.\n\n"
          + localSchedule;
            
      String jsonArray = llmChatRequest(workId, prompt);
      
      if (jsonArray.startsWith("[") && jsonArray.indexOf("]") !=-1) {
        jsonArray = jsonArray.substring(0, jsonArray.lastIndexOf("]") + 1);
		
        if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
          scheduleTasks = jsonArray;
          xSemaphoreGive(stateMutex);
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
        "\"reason\":\"Invalid JSON array format.\","
        "\"workId\":\""+workId+"\"}";
      }  

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);
     
    }
    else if (command == "/getSchedule") {
      String localSchedule = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        localSchedule = scheduleTasks;
        xSemaphoreGive(stateMutex);
      }

      String prompt =
      R"(You are summarizing a user's scheduled tasks.

      Instructions:
      - Reply in the user's current language.
      - Present all tasks as concise bullet points.
      - Some tasks are already written in natural language. Keep their meaning unchanged.
      - Some tasks are tool commands (such as JSON or function-call data). Interpret them and describe the action in natural language instead of displaying the command itself.
      - Never output raw JSON, code, or function-call syntax.
      - Include important details such as time, device, location, recipient, colors, brightness, duration, or other parameters whenever available.
      - If a command cannot be interpreted, simply state that the task could not be interpreted.
      - Produce only the final task list.

      Scheduled tasks:
      )"
      + localSchedule;

      String response = llmChatRequest(workId, prompt);
      replyUserMessage(workId, response); 
          
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }
     
    }    
    else if (command == "/getUnfinishedSchedule") {
      String localSchedule = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        if (scheduleTasks.startsWith("[") && scheduleTasks.indexOf("]") !=-1)
              scheduleTasks = scheduleTasks.substring(0, scheduleTasks.lastIndexOf("]") + 1);
        localSchedule = scheduleTasks;
        xSemaphoreGive(stateMutex);
      }
            
      String response = getUnfinishedScheduleTasksJson(localSchedule);
      replyUserMessage(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }
 
    }
    else if (command == "/clearSchedule") {
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        scheduleTasks = "";
        executedTodayTasks = "";
        xSemaphoreGive(stateMutex);
      }
      
      String response = "Scheduled tasks have been cleared.";
      replyUserMessage(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }
    }
    else if (command == "/reset") {
      llmChatReset();  
            
      String response = "New chat started.";
      replyUserMessage(workId, response);

    } 
    else if (command == "/getMemory") {
      String msg = getMemoryInfo();
      replyUserMessage(workId, msg);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", msg + timestamps);
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);          

    } 
    else if (command == "/getLog") {
      String localHistory = "";
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        localHistory = executeToolHistory;
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }
      Serial.println("\n\nExecute tools history:\n\n"+localHistory+"\n\n");
      replyUserMessage(workId, "Please check the serial monitor to view the tool execution log.");
      
    } 
    else if (command == "/chat") {
      String reply = params["reply"].as<String>();
      replyUserMessage(workId, reply);

    } 
    else if (command == "/search") {
      String query = params["query"].as<String>();
      String task = params["task"].as<String>();
	  
      String response = llmSearchRequest(workId, query, false);
      handleAgentResponse(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + " [ "+query+" | "+task+" ]\n";
        xSemaphoreGive(stateMutex);
      }
      
      evaluateWorkflowContinuation(workId, reCheck, task);

    } 
    else if (command == "/delay") {
      long milliseconds = params["milliseconds"].as<long>();
      milliseconds = constrain(milliseconds, 0, 10000);
  
      unsigned long start = millis();
  
      while (millis() - start < milliseconds) {
          vTaskDelay(10 / portTICK_PERIOD_MS);
          
      }

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + " [ " + String(milliseconds) + " ]\n";
        xSemaphoreGive(stateMutex);
      }
  
      evaluateWorkflowContinuation(workId, reCheck);
        
    }
    else if (command == "/vision") {
      String query = params.containsKey("query") ? params["query"].as<String>() : "Describe the image in detail in the user's language. Do not return bounding boxes or coordinates. Respond in natural language only.";
      bool frames = params.containsKey("frames") ? params["frames"].as<bool>() : true;
      String task = params.containsKey("task") ? params["task"].as<String>() : "NONE";
	  
      String response = llmVisionRequest(workId, query, frames);
      handleAgentResponse(workId, response);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + " [ "+query+" | "+frames+" | "+task+" ]\n";
        xSemaphoreGive(stateMutex);
      }
      
      evaluateWorkflowContinuation(workId, reCheck, task);
    }	
  	else if (command == "/reboot") {
  	  replyUserMessage(workId, "Rebooting the device, please wait...");

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        executeToolHistory += workId + " " + command + "\n";
        xSemaphoreGive(stateMutex);
      }
  		
  	  Serial.println("User requested reboot the device.");
  	  vTaskDelay(2000 / portTICK_PERIOD_MS);
  		
  	  NVIC_SystemReset();   // ESP32-S3 PORT: NVIC_SystemReset() -> ESP.restart()
  	}
  	else if (command == "/tcpSendMessage") {
      String device = params["device"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = tcpSendMessage(workId, device, message);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+device+" | "+message+" ]\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);
	}
  	else if (command == "/telegramSendMessage") {
      String token = params["token"].as<String>();
	  String chatId = params["chatId"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = telegramSendMessage(token, chatId, message);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+chatId+" | "+message+" ]\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);
	}
  	else if (command == "/telegramSendImage") {
      String token = params["token"].as<String>();
	  String chatId = params["chatId"].as<String>();
	  bool frames = params["frames"].as<bool>();
	  
      String response = telegramSendCapturedImage(token, chatId, frames);
	  
      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {	  
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);	  
        executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+chatId+" | "+frames+" ]\n";
        xSemaphoreGive(stateMutex);
      }	  

      evaluateWorkflowContinuation(workId, reCheck);
	}		
  	else if (command == "/lineSendMessage") {
      String token = params["token"].as<String>();
	  String targetId = params["targetId"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = lineSendMessage(token, targetId, message);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", command + timestamps);
        historicalMessages += buildLlmMessage("model", response + timestamps);
        executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+targetId+" | "+message+" ]\n";
        xSemaphoreGive(stateMutex);
      }

      evaluateWorkflowContinuation(workId, reCheck);
	}
    else if (command == "/help" || command == "/start") {
         
      String mem = getMemoryInfo();
      String command = systemCommand;
      command.replace("<memory>", mem);
      command = llmChatRequest(workId, "Reply the following text in the user's language:\n\n" + command);
      
      replyUserMessage(workId, command, telegrambotKeyboard);

      if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
        historicalMessages += buildLlmMessage("user", "Command list" + timestamps);
        historicalMessages += buildLlmMessage("model", command + timestamps);
        xSemaphoreGive(stateMutex);
      }
      
    }      
    else {
      String response = llmChatRequest(workId, command);
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

// Base64-encode an audio buffer and send it to LLM for transcription.

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

  WiFiSSLClient client;
  if (!client.connect("generativelanguage.googleapis.com", 443)) {
    Serial.println("[STT] Connection to Gemini failed");
    return "Connected to Gemini failed.";
  }

  client.println("POST /v1beta/models/" + llmModel +
                 ":generateContent?key=" + llmKey + " HTTP/1.1");
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
      }
    }
       // [WDT FIX] prevent watchdog timeout during voice file Gemini response
    vTaskDelay(1);
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

// Base64 is NOT used here — OpenAI Whisper requires multipart file upload.
String sendFileToOpenAI(uint8_t* fileinput, size_t fileSize, String mimeType, String prompt) {

  WiFiSSLClient client;
  if (!client.connect("api.openai.com", 443)) {
    Serial.println("[STT] Connection to OpenAI failed");
    return "Connected to OpenAI failed.";
  }

  // Telegram voice is OGG Opus; Whisper accepts ogg.
  String filename = "voice.ogg";

  String boundary = "Taiwan";
  String head =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"model\"\r\n\r\n"
    "whisper-1\r\n"
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"response_format\"\r\n\r\n"
    "json\r\n";

  if (prompt.length() > 0) {
    // Optional style / language hint for Whisper
    head +=
      "--" + boundary + "\r\n"
      "Content-Disposition: form-data; name=\"prompt\"\r\n\r\n" +
      prompt + "\r\n";
  }

  head +=
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n"
    "Content-Type: " + mimeType + "\r\n\r\n";

  String tail = "\r\n--" + boundary + "--\r\n";

  size_t totalLen = head.length() + fileSize + tail.length();

  client.println("POST /v1/audio/transcriptions HTTP/1.1");
  client.println("Host: api.openai.com");
  client.println("Authorization: Bearer " + llmKey);
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println("Content-Length: " + String(totalLen));
  client.println("Connection: close");
  client.println();

  client.print(head);

  // Stream binary audio in chunks
  uint8_t *buf = fileinput;
  size_t remaining = fileSize;
  while (remaining > 0) {
    size_t chunk = remaining > 1024 ? 1024 : remaining;
    client.write(buf, chunk);
    buf += chunk;
    remaining -= chunk;
    
  }

  client.print(tail);

  String body = "";
  unsigned long timeout = millis() + 20000;
  bool headersEnded = false;
  String line = "";

  while ((client.connected() || client.available()) && millis() < timeout) {
    while (client.available()) {
      char c = client.read();
      if (!headersEnded) {
        if (c == '\n') {
          if (line.length() <= 1)
            headersEnded = true;
          line = "";
        } else if (c != '\r') {
          line += c;
        }
      } else {
        body += c;
      }
    }
    
    vTaskDelay(1);
  }

  client.stop();

  int jsonStart = body.indexOf('{');
  if (jsonStart != -1)
    body = body.substring(jsonStart);
  else if (body.length() == 0)    
    return "NONE";

  DynamicJsonDocument doc(8192);
  DeserializationError err = deserializeJson(doc, body);

  if (err) {
    Serial.println("[DEBUG] JSON parse failed: (sendFileToOpenAI)\n" + body);
    return "JSON parse failed (sendFileToOpenAI). Please try again.";
  }

  if (doc.containsKey("error")) {
    String msg = "OpenAI STT Error: " + doc["error"]["message"].as<String>();
    Serial.println(msg);
    return msg;
  }

  if (doc.containsKey("text")) {
    String result = doc["text"].as<String>();
    result.replace("\n", "");
    return result;
  }

  return "No text returned from OpenAI.";
}

// xAI Grok Speech-to-Text REST API (multipart file upload)
String sendFileToGrok(uint8_t* fileinput, size_t fileSize, String mimeType, String prompt) {

  WiFiSSLClient client;
  if (!client.connect("api.x.ai", 443)) {
    Serial.println("[STT] Connection to Grok (xAI) failed");
    return "Connected to Grok failed.";
  }

  String filename = "voice.ogg";

  String boundary = "Taiwan";

  // Optional fields should precede the file part (xAI recommendation)
  String head = "";
  if (prompt.length() > 0) {
    // Use prompt text as a keyterm bias when provided
    head +=
      "--" + boundary + "\r\n"
      "Content-Disposition: form-data; name=\"keyterm\"\r\n\r\n" +
      prompt + "\r\n";
  }

  head +=
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n"
    "Content-Type: " + mimeType + "\r\n\r\n";

  String tail = "\r\n--" + boundary + "--\r\n";

  size_t totalLen = head.length() + fileSize + tail.length();

  client.println("POST /v1/stt HTTP/1.1");
  client.println("Host: api.x.ai");
  client.println("Authorization: Bearer " + llmKey);
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println("Content-Length: " + String(totalLen));
  client.println("Connection: close");
  client.println();

  client.print(head);

  uint8_t *buf = fileinput;
  size_t remaining = fileSize;
  while (remaining > 0) {
    size_t chunk = remaining > 1024 ? 1024 : remaining;
    client.write(buf, chunk);
    buf += chunk;
    remaining -= chunk;
    
  }

  client.print(tail);

  String body = "";
  unsigned long timeout = millis() + 20000;
  bool headersEnded = false;
  String line = "";

  while ((client.connected() || client.available()) && millis() < timeout) {
    while (client.available()) {
      char c = client.read();
      if (!headersEnded) {
        if (c == '\n') {
          if (line.length() <= 1)
            headersEnded = true;
          line = "";
        } else if (c != '\r') {
          line += c;
        }
      } else {
        body += c;
      }
    }
    
    vTaskDelay(1);
  }

  client.stop();

  int jsonStart = body.indexOf('{');
  if (jsonStart != -1)
    body = body.substring(jsonStart);
  else if (body.length() == 0)    
    return "NONE";

  DynamicJsonDocument doc(8192);
  DeserializationError err = deserializeJson(doc, body);

  if (err) {
    Serial.println("[DEBUG] JSON parse failed: (sendFileToGrok)\n" + body);
    return "JSON parse failed (sendFileToGrok). Please try again.";
  }

  if (doc.containsKey("error")) {
    String msg = "Grok STT Error: " + doc["error"]["message"].as<String>();
    Serial.println(msg);
    return msg;
  }

  if (doc.containsKey("text")) {
    String result = doc["text"].as<String>();
    result.replace("\n", "");
    return result;
  }

  return "No text returned from Grok.";
}

String sendFileToLlm(uint8_t* fileinput, size_t fileSize, String mimeType, String prompt) {
    if (llmType == "gemini")
        return sendFileToGemini(fileinput, fileSize, mimeType, prompt);
    else if (llmType == "openai")
        return sendFileToOpenAI(fileinput, fileSize, mimeType, prompt);
    else if (llmType == "grok")
        return sendFileToGrok(fileinput, fileSize, mimeType, prompt);
    else
        return "NONE";
}

// ============================================================
//  Telegram: Download File by Path
// ============================================================

// Download a file from Telegram's CDN into a heap-allocated buffer.

uint8_t* downloadTelegramFile(String filePath) {

  uint8_t* voiceFile = (uint8_t*)malloc(MAX_FILE_SIZE);
  if (!voiceFile) return NULL;

  downloadedFileSize = 0;
  WiFiSSLClient client;

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

  WiFiSSLClient client;
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
         // [WDT FIX] prevent watchdog timeout while waiting for Telegram response

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
            text = llmChatRequest(workId, text);
            handleAgentResponse(workId, text);
          }

          String localHistory = "";
          if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
            localHistory = historicalMessages;
            xSemaphoreGive(stateMutex);
          }

        } else if (doc["result"][0]["message"].containsKey("voice")) {
          voiceFileId = doc["result"][0]["message"]["voice"]["file_id"].as<String>();

          String   filePath  = getTelegramFilePath(voiceFileId);
          uint8_t* voiceFile = downloadTelegramFile(filePath);

          if (voiceFile && downloadedFileSize > 0) {
            text = sendFileToLlm(
              voiceFile, downloadedFileSize,
              "audio/ogg; codecs=opus",
              "Transcribe this audio to text exactly as spoken."
            );

            if (text.startsWith("/")) {
              executeTool(workId, text, JsonObject());
            } else {
              text = llmChatRequest(workId, text);
              handleAgentResponse(workId, text);
            }
          }

          if (voiceFile)
            free(voiceFile);

          String localHistory2 = "";
          if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
            localHistory2 = historicalMessages;
            xSemaphoreGive(stateMutex);
          }
   
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
     // Register this task with the TWDT
  while (1) {
    
	  
    WiFiClient client = server.available();

    if (client) {
      String currentLine = "";  // Buffer to accumulate one line of the HTTP request
      
      while (client.connected() || client.available()) { 
        
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
              client.println("Connection: close");
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
          if (currentLine.startsWith("GET / ") && currentLine.endsWith(" HTTP/1.")) {
            
            mainPageHTML = String(INDEX_HTML);
			
            mainPageHTML.replace("deviceName", deviceName);
            mainPageHTML.replace("wifiSsid", wifiSsid);
            mainPageHTML.replace("wifiPassword", wifiPassword);
            mainPageHTML.replace("telegrambotToken", telegrambotToken);
            mainPageHTML.replace("telegrambotChatId", telegrambotChatId);
            mainPageHTML.replace("scheduleTimeout", String(scheduleTimeout));            
            mainPageHTML.replace("llmType", llmType);            
            mainPageHTML.replace("llmKey", llmKey);
            mainPageHTML.replace("llmModel", llmModel);
            mainPageHTML.replace("timeZone", timeZone);		

            currentLine = "";            
          }
          else if (currentLine.startsWith("GET /updateConfig?") && currentLine.endsWith(" HTTP/1.")) {
            
            String workId = String(taskTags[0]) + " " + getRtcTimeString();
            
            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateConfig?", "");
            currentLine.replace(" HTTP/1.", "");
            
            if (currentLine.startsWith("{") && currentLine.endsWith("}")) {
     
			  setEnvironmentSettings(currentLine);
			  
              devicesDefinitionFinal = devicesDefinition;
              devicesDefinitionFinal += "\n\nDevice Name: " + deviceName;
              devicesDefinitionFinal += "\nDevice timezone: " + timeZone;
			  
			  systemContentReset();
			
              mainPageHTML = "Configuration updated successfully.";
			}
            else
              mainPageHTML = "Configuration updated failed. JSON parse failed.";
		  
			currentLine = "";
			
            // executeTool(workId, "/reboot", JsonObject());			
            
          }
          else if (currentLine.startsWith("GET /getSoul") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = llmRole;

            currentLine = "";

          }
          else if (currentLine.startsWith("GET /updateSoul?") && currentLine.endsWith(" HTTP/1.")) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateSoul?", "");
            currentLine.replace(" HTTP/1.", "");
            
            llmRole = currentLine;
			
            systemContentReset();
			
            mainPageHTML = "Soul updated successfully.";
            
            currentLine = "";  

            // executeTool(workId, "/reboot", JsonObject());			
            
          }				  
          else if (currentLine.startsWith("GET /getDevice") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = devicesDefinition;

            currentLine = "";

          }
		  else if (currentLine.startsWith("GET /updateDevice?") && currentLine.endsWith(" HTTP/1.")) {

            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateDevice?", "");
            currentLine.replace(" HTTP/1.", "");
            
            devicesDefinition = currentLine;
			
            devicesDefinitionFinal = devicesDefinition;
            devicesDefinitionFinal += "\n\nDevice Name: " + deviceName;
            devicesDefinitionFinal += "\nDevice timezone: " + timeZone;
			
			systemContentReset();
			
            mainPageHTML = "Device updated successfully.";
			
            currentLine = "";

            // executeTool(workId, "/reboot", JsonObject());			
            
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

            // executeTool(workId, "/reboot", JsonObject());			
            
          }		  
          else if (currentLine.startsWith("GET /chat") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = String(INDEX_CHAT_HTML);

            currentLine = "";

          }
          else if (currentLine.startsWith("GET /schedule") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = String(INDEX_SCHEDULE_HTML);
			if (mainPageHTML == "")
				mainPageHTML = "[]";
			
            currentLine = "";

          }
          else if (currentLine.startsWith("GET /getScheduleTasks") && currentLine.endsWith(" HTTP/1.")) {

            if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
              mainPageHTML = scheduleTasks;
              xSemaphoreGive(stateMutex);
            }

            currentLine = "";

          }                                            
          else if (currentLine.startsWith("GET /updateScheduleTasks?") && currentLine.endsWith(" HTTP/1.")) {
            
            String workId = String(taskTags[0]) + " " + getRtcTimeString();
            
            currentLine = urldecode(currentLine);
            currentLine.replace("GET /updateScheduleTasks?", "");
            currentLine.replace(" HTTP/1.", "");
            
            if (currentLine.startsWith("[") && currentLine.endsWith("]")) {
              
              mainPageHTML = "Schedule updated successfully.";

              if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
                scheduleTasks = currentLine;
                historicalMessages += buildLlmMessage("user", "GET /updateScheduleTasks?<NEW SCHEDULE TASKS>");
                historicalMessages += buildLlmMessage("model", mainPageHTML);
                String localHistory = historicalMessages;
                xSemaphoreGive(stateMutex);
        
              }
            }
            else
              mainPageHTML = "Schedule updated failed. JSON parse failed.";

            currentLine = "";        
            
          }
          else if (currentLine.startsWith("GET /message?") && currentLine.endsWith(" HTTP/1.")) {
            
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
    				  currentLine = llmChatRequest(workId, currentLine);
    				  handleAgentResponse(workId, currentLine);
    				}

              String localHistory = "";
              if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
                localHistory = historicalMessages;
                xSemaphoreGive(stateMutex);
              }
      
            }
            
            mainPageStatus = false;

            currentLine = "";

    			}      
        }
		else {
          vTaskDelay(1); 
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
    uint32_t img_addr = 0;
    uint32_t img_len = 0;	
    
    if (client) {
      String currentLine = "";

      while (client.connected() || client.available()) { 

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
              
              Camera.getImage(0, &img_addr, &img_len);
              uint8_t *fbBuf = (uint8_t*)img_addr;
              size_t fbLen = img_len;
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
          vTaskDelay(1); 
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
     // Register this task with the TWDT
  while (1) {
    

    if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      getTelegramMessage();   // getTelegramMessage 內部已有 wdt_reset
      xSemaphoreGive(botClientMutex);
    }

       // [WDT FIX] reset after mutex release (getTelegramMessage may take up to 5s)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
  }
}

// Periodic system check task
void task_theft_detection(void *param) {
  (void)param;
  
  while (1) {

    // Long sleep broken into slices so the watchdog is reset
    // periodically instead of once every 5 minutes.
    for (int i = 0; i < 300000 / 1000; i++) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      
    }

    // Wait until Telegram task is idle, then take exclusive ownership
    // of botClient before stopping it (see botClientMutex notes on
    // getTelegramMessage()).
    if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      botClient.stop();
      xSemaphoreGive(botClientMutex);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    Serial.println("\n\nExecuting Skill: theft_detection\n\n");

    String workId = String(taskTags[4]) + " " + getRtcTimeString();
    
    evaluateWorkflowContinuation(
		workId, 
		true, 
		"Must execute skill theft_detection. Return ONLY tool_call JSON."
	);
       // [WDT FIX] evaluateWorkflowContinuation chains LLM+Vision calls, reset after

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
  
  long long epoch = rtc.Read();
  time_t rawtime = (time_t)epoch;
  struct tm *now = localtime(&rawtime);

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
// For each due task, constructs a prompt and sends it to LLM for execution.
// After all due tasks are processed, triggers /updateScheduleStatus to sync
// execution state, and persists daily execution records and chat history to SD card.
void task_time_scheduling(void *param) {
  (void)param;
  while (1) {
    

    // [WDT FIX] Split 60s wait into 10s segments so WDT (30s) is reset regularly
    for (int i = 0; i < 6; i++) {
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      
    }
    

    // Stop the shared botClient connection so the Telegram task yields
    // before we start a potentially long scheduling cycle.
    if (xSemaphoreTake(botClientMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      botClient.stop();
      xSemaphoreGive(botClientMutex);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    String workId = String(taskTags[3]) + " " + rtcFormatTime;

    if (rtcYear == 0) {
      Serial.println("[DEBUG] RTC time is not initialized.");
      executeTool(workId, "/syncrtc", JsonObject(), false);
      if (rtcYear == 0)
        continue;
    }

    String localSchedule = "";
    if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
      if (scheduleTasks.startsWith("[") && scheduleTasks.indexOf("]") !=-1)
        scheduleTasks = scheduleTasks.substring(0, scheduleTasks.lastIndexOf("]") + 1);
      localSchedule = scheduleTasks;
      xSemaphoreGive(stateMutex);
    }

    if (localSchedule.startsWith("[") && localSchedule.indexOf("]") !=-1) {
 
      String unfinishedScheduleTasksJson = getExecuteScheduleTasksJson(localSchedule);

      if (unfinishedScheduleTasksJson.startsWith("[") && unfinishedScheduleTasksJson.indexOf("]") !=-1) {
        unfinishedScheduleTasksJson = unfinishedScheduleTasksJson.substring(0, unfinishedScheduleTasksJson.lastIndexOf("]") + 1);

        String response = "";

        DynamicJsonDocument doc(8192);
      
        DeserializationError err = deserializeJson(doc, unfinishedScheduleTasksJson);
        if (err) {
          Serial.println("[DEBUG] JSON parse failed: (task_time_scheduling)\n" + unfinishedScheduleTasksJson);
          continue;   // don't return — keep the task alive
        }  

        JsonArray tasks = doc.as<JsonArray>();
        
        for (JsonObject obj : tasks) {
             // reset per task to survive long LLM calls

          String taskName = obj["task"].as<String>();

          String schedule = obj["schedule"].as<String>();
          String item = obj["task"].as<String>();           

          if ((item.startsWith("{") && item.endsWith("}")) || (item.startsWith("[") && item.endsWith("]"))) {
			  handleAgentResponse(workId, item);
          } 
		  else {
			  String prompt =
				"This is a deterministic scheduling execution step. "
			  
				"\n\nUnfinished scheduled tasks:\n" +
				item +
			  
				"\n\nThe task list above already contains ONLY tasks that have not been executed. "
				"Evaluate EvERY task in this list independently. "
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

			  response = llmChatRequest(workId, prompt);
				 // [WDT FIX] llmChatRequest can take up to 20s, reset immediately after

			  handleAgentResponse(workId, response);
				 // [WDT FIX] handleAgentResponse may chain another LLM call
				 
          }	

          if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
            markExecutedToday(schedule + " " + item);
            xSemaphoreGive(stateMutex);
          }
        }
        
        if (tasks.size()>0) {
          executeTool(workId, "/updateScheduleStatus", JsonObject(), false);

          String localExecuted = "";
          String localHistory = "";
          if (xSemaphoreTake(stateMutex, MUTEX_TIMEOUT_TICKS) == pdTRUE) {
            localExecuted = executedTodayTasks;
            localHistory  = historicalMessages;
            xSemaphoreGive(stateMutex);
          }

        }
      }
      
    }
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.enableConcurrent();
  WiFi.apbegin((char*)apSsid.c_str(), (char*)apPassword.c_str(), channel_ap, 0);
    
  for (int i=0 ; i<2 ; i++) {

    if (wifiSsid == "")
      break;

    WiFi.begin((char*)wifiSsid.c_str(), (char*)wifiPassword.c_str());
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
    
    if (WiFi.status() == WL_CONNECTED)
      break;
  }
  
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
  llmType =  obj["llm_type"].as<String>();  
  llmKey =  obj["llm_key"].as<String>();
  llmModel =  obj["llm_model"].as<String>();
  scheduleTimeout = obj["schedule_timeout"].as<int>();  
  timeZone = obj["timezone"].as<String>();  

}

String Ip2String(IPAddress ip) {
  return String(ip[0])+String(".")+String(ip[1])+String(".")+String(ip[2])+String(".")+String(ip[3]);
}

// Arduino setup
void setup() {
  Serial.begin(115200);

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
    NVIC_SystemReset();
  }
  
  // Indicator LED  
  pinMode(ledPin, OUTPUT);

  initWiFi();  

  devicesDefinitionFinal = devicesDefinition;
  devicesDefinitionFinal += "\n\nDevice Name: " + deviceName;
  devicesDefinitionFinal += "\nDevice timezone: " + timeZone;
  
  if (llmRole.length() == 0 || devicesDefinition.length() == 0) {
	  Serial.println("System configuration failed. Restarting the MCU...");
	  delay(5000);
	  NVIC_SystemReset();
  }
  
  systemContent = buildLlmMessage("user", llmRole, 0) + buildLlmMessage("model", "OK");
  systemContentTools = buildLlmMessage("user", llmRole + devicesDefinitionFinal + devicesRule + skillsDefinition + toolsDefinition, 0) + buildLlmMessage("model", "OK");
  systemContentNoTools = buildLlmMessage("user", llmRole + devicesDefinitionFinal + devicesRule, 0) + buildLlmMessage("model", "OK");

  config.setRotation(0);
  Camera.configVideoChannel(0, config);
  Camera.videoInit();
  Camera.channelBegin(0);  

  rtcInitialTime("RTC Initial Time");
  replyUserMessage(String(taskTags[1]) + " " + getRtcTimeString(), "RTC START: " + getRtcTimeString(), telegrambotKeyboard);

  // IMPORTANT: Must be synced with RTC date immediately after loading
  long long epoch = rtc.Read();
  time_t rawtime = (time_t)epoch;
  struct tm *now = localtime(&rawtime);
  executedTodayDate = now->tm_mday;

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

  Serial.println("AP mode"); 
  Serial.println("fuClaw Manager: http://192.168.1.1:81");
  Serial.println("Video stream: http://192.168.1.1:82"); 
  Serial.println("AP ssid : " + apSsid);
  Serial.println("AP password : " + apPassword);
  Serial.println(); 

  if (WiFi.status() == WL_CONNECTED) {
    for (int i=0 ; i<3 ; i++) {
      digitalWrite(ledPin, 1);
      delay(300);
      digitalWrite(ledPin, 0);
      delay(300);      
    }
	
    Serial.println("STA mode"); 
    Serial.println("fuClaw Manager: http://" + Ip2String(WiFi.localIP()) + ":81"); 
    Serial.println("Video stream: http://" + Ip2String(WiFi.localIP()) + ":82");            
    Serial.println();

    historicalMessages += buildLlmMessage("user", "Current Device IP: " + Ip2String(WiFi.localIP()));
  }  
   
}

// Main loop
void loop() {
  // Main Arduino loopTask is otherwise idle; just yield. (loopTask is
  // NOT registered with the TWDT, since it does no blocking work here.)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
