/*
------------------------------------------------------------
fuClaw Embedded AI Agent Framework
------------------------------------------------------------

Author:
  ChungYi Fu (Kaohsiung, Taiwan)
  https://www.facebook.com/francefu

Repository:
  https://github.com/fustyles/fuClaw

------------------------------------------------------------
Version
-----------------------------------------------------------
Prompt-Orchestrated Embedded Agent Edition

Build Date: 2026-06-23 00:00
------------------------------------------------------------
Overview
------------------------------------------------------------
fuClaw is an embedded multimodal AI agent framework running
on Realtek Ameba Pro2 devices:

- AMB82-mini
- HUB 8735 Ultra

It combines:

- MQTT messaging
- Gemini Chat Web Interface
- Google Gemini generateContent API
- Gemini grounded web search
- Gemini multimodal vision reasoning
- Gemini speech-to-text processing
- Prompt-driven JSON tool routing
- GPIO digital / analog I/O control
- Camera capture and image publishing
- Persistent conversation memory
- SD card runtime storage
- FreeRTOS concurrent task scheduling

The runtime acts as a hybrid autonomous agent:

Conversation + Reasoning + Tools + Vision + Memory + Skills + Hardware
------------------------------------------------------------
Runtime Architecture
------------------------------------------------------------
MQTT / Web Chat User
      ↓
Communication Task
(MQTT / Web Chat)
      ↓
Message Router
      ↓
Gemini Agent Engine
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
Workflow evaluation
      ↓
Natural language reply
------------------------------------------------------------
Execution Model
------------------------------------------------------------
fuClaw is an embedded multimodal AI agent framework running
on Realtek Ameba Pro2 devices:
- AMB82-mini
- HUB 8735 Ultra

It combines:
- MQTT Broker
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
/mqttSendMessage          Send a message to another device or agent over TCP or any subscriber via MQTT
/mqttSendImage            Send a video snapshot to another fuClaw device or any subscriber via MQTT
/telegramSendMessage      Send a message to Telegram Bot
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

Unknown hardware mappings require clarification.
GPIO values are strictly validated before execution.
------------------------------------------------------------
Software Stack
------------------------------------------------------------
- WiFi.h
- WiFiSSLClient
- PubSubClient
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
// Gemini chat via MQTT
#include "index_mqtt_chat_html.h"
// Schedule manager
#include "index_schedule_html.h"

// Array of task-related tags used as stop markers when parsing text
// Every tag MUST be enclosed in angle brackets '<' and '>'.
const char* taskTags[] = { "<PAGE>", "<BOT>", "<MQTT>", "<TIME_SCHEDULING>", "<THEFT_DETECTION>" };

String mainPageHTML = "";
bool mainPageStatus = false;

// Serialized system prompt content used as the initial conversation context
String systemContent = "";
String systemContentTools = "";
String systemContentNoTools = "";

// Logs each tool execution as a human-readable record for /log command
String executeToolHistory = "";
  
// Stores entire chat history in Gemini API JSON format
// Used to preserve conversation memory across requests
String historicalMessages = "";

// Schedule Tasks
String scheduleTasks = "";
int scheduleTimeout = 5;    // minutes
String executedTodayTasks = "";
int executedTodayDate = 0;

// Indicator LED output pin
int ledPin = 24;    // green led (AMB82-mini: 24, HUB 8735 Ultra: 25)

#include <WiFi.h>

// Underlying TCP socket used by PubSubClient
WiFiClient wifiClient;

char channel_ap[] = "2";
WiFiServer server(81);
WiFiServer serverStream(82);

#include "Base64.h"
#include <ArduinoJson.h>
#include "FreeRTOS.h"
#include "task.h"

#include <PubSubClient.h> // MQTT client (Nick O'Leary / knolleary)

// MQTT client instance bound to the WiFi socket
PubSubClient mqttClient(wifiClient);

// Forward declarations
String getUnfinishedScheduleTasksJson(const String &scheduleTasksJson);
String getExecuteScheduleTasksJson(const String &scheduleTasksJson);
String buildGeminiMessage(String role, String message, bool comma);
String getRtcTimeString(bool filename);
void replyUserMessage(String workId, String text);
void handleAgentResponse(String workId, String message);
String geminiChatRequest(String workId, String message, int tools);

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

// Generates a unique MQTT Client ID based on the device's Wi-Fi MAC address
String generateMqttClientId() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char clientId[32];
  snprintf(clientId, sizeof(clientId),
           "AmebaPro2-%02X%02X%02X",
           mac[3], mac[4], mac[5]);
  return String(clientId);
}

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

  WiFiSSLClient client;
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

// Initialize the RTC using Gemini-synchronized local time.
void rtcInitialTime(String workName) {
	
  rtcUpdateStatus = true;
  
  String prompt =
    "Convert this GMT datetime to " + timeZone + ".\n"
    "GMT datetime: " + getGeminiDatetime() + "\n\n"
	
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

  String message = geminiChatRequest("<MQTT>", prompt, -1);

  if (message.startsWith("{") && message.endsWith("}")) {

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.println("[DEBUG] JSON parse failed\n" + message);
      replyUserMessage(workName, "RTC time update failed.");
      
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
    replyUserMessage(workName, "RTC time update failed.");
  }

  rtc.Init();
  long long initTime = rtc.SetEpoch(rtcYear, rtcMonth, rtcDay, rtcHour, rtcMinute, rtcSecond);
  rtc.Write(initTime);
}

// ============================================================
//  MQTT: Send Text Message
// ============================================================

/**
 * @brief Publish a plain-text message to an MQTT topic.
 *
 * Connects (or re-uses an existing connection) to the broker and
 * publishes a single UTF-8 string payload.  The MQTT QoS level used
 * by PubSubClient::publish() is QoS 0 (at-most-once / fire-and-forget).
 *
 * @param topic  Destination MQTT topic string.
 * @param text   UTF-8 payload to publish.
 */
String mqttSendText(String topic, String text) {
  
    // Attempt to connect (no-op if already connected)
    if (mqttClient.connect(wifiClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {

      mqttClient.beginPublish(topic.c_str(), text.length(), false);

      mqttClient.write(
          (const uint8_t*)text.c_str(),
          text.length()
      );

      bool isPublished = mqttClient.endPublish();          

      if (isPublished)
          return "Publishing message to MQTT Successfully";
      else
          return "Publishing message to MQTT Failed";
    }
    else
    	return "Connect to MQTT Server Failed";
	
}

// ============================================================
//  MQTT: Capture & Send Still Image
// ============================================================

/**
 * @brief Capture a JPEG frame from the camera and publish it to an MQTT topic.
 *
 * Two encoding modes are supported:
 *   - Raw binary  (base64 = false, default): the JPEG bytes are streamed
 *     directly in MQTT_MAX_PACKET_SIZE chunks via beginPublish / write /
 *     endPublish.  Suitable for subscribers that can receive binary payloads.
 *   - Base64       (base64 = true): the JPEG is Base64-encoded and prefixed
 *     with a data-URI header ("data:image/jpeg;base64,…").  Useful for
 *     web-based MQTT dashboards that render <img src="…"> tags directly.
 *
 * @param topic    Destination MQTT topic string.
 * @param capture  true  = call Camera.getImage() to grab a fresh frame before
 *                         publishing (normal use case for /still command).
 *                 false = reuse the last frame already stored in img_addr /
 *                         img_len (e.g. to publish the same frame to a second
 *                         topic without incurring another capture latency).
 * @param base64   true  = encode the frame as a Base64 data-URI string.
 *                 false = publish raw binary JPEG bytes (default).
 */
String mqttSendImage(String topic, bool capture, bool base64 = false) {

    // Attempt to connect (or re-use the existing session)
    if (mqttClient.connect(wifiClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {

        // Optionally grab a fresh JPEG frame from the ISP / camera pipeline
        if (capture) {
            Camera.getImage(0, &imageAddress, &imageLength);
            // imageAddress now points to the DMA buffer holding the JPEG
            // imageLength  holds the exact byte count of the compressed frame
        }

        // Cast the DMA buffer address to a byte pointer for MQTT write operations
        uint8_t* fbBuf = (uint8_t*)imageAddress;
        size_t   fbLen = imageLength;

        boolean isPublished;

        if (!base64) {
            // ---- Raw binary path ----------------------------------------
            // Open a "large payload" MQTT publish transaction.
            // PubSubClient::beginPublish() writes the fixed MQTT header and
            // the payload-length field; subsequent write() calls stream the
            // body; endPublish() flushes and finalises the packet.
            mqttClient.beginPublish(topic.c_str(), fbLen, false);

            // Stream the JPEG in MQTT_MAX_PACKET_SIZE-byte slices to stay
            // within the library's internal packet buffer limit.
            int ps = MQTT_MAX_PACKET_SIZE;
            for (int i = 0; i < (int)fbLen; i += ps) {
                // Clamp the last chunk so it does not exceed remaining bytes
                int s = ((fbLen - i) < (size_t)ps) ? (fbLen - i) : ps;
                mqttClient.write((uint8_t*)fbBuf + i, s);
            }

            isPublished = mqttClient.endPublish();

        } else {
          
            // Cast the DMA frame buffer to a char pointer for byte-by-byte Base64 encoding
            char *input = (char *)fbBuf;
            
            // Output buffer for one Base64-encoded group (4 chars + null terminator per 3 input bytes)
            char output[base64_enc_len(3)];
            
            // Pre-calculate the final string length to avoid repeated heap reallocations during +=
            //   23            : length of the data-URI prefix "data:image/jpeg;base64,"
            //   (fbLen+2)/3*4 : Base64 expands every 3 bytes → 4 chars (ceiling division handles padding)
            //    1            : null terminator
            size_t estimatedSize = 23 + ((fbLen + 2) / 3) * 4 + 1;
            
            // Initialise the String with the data-URI prefix (small initial allocation)
            String imageFile = "data:image/jpeg;base64,";
            
            // Reserve the full estimated capacity in one shot so subsequent += calls
            // write into already-allocated space without triggering further heap reallocations
            imageFile.reserve(estimatedSize);
            
            // Encode the raw JPEG bytes to Base64 and append to imageFile.
            for (int i = 0; i < fbLen; i++) {
                base64_encode(output, (input++), 3);
                if (i % 3 == 0) imageFile += String(output);
            }
            
            // Capture the final encoded string length for the MQTT packet header
            size_t imageFileSize = imageFile.length();
            
            // Open the MQTT publish transaction and declare the total payload length upfront.
            // 'false' = non-retained message (broker will not store it for late subscribers)
            mqttClient.beginPublish(topic.c_str(), imageFileSize, false);
            
            // Stream the Base64 string in 1024-character chunks.
            for (int index = 0; index < (int)imageFileSize; index += 1024) {
                mqttClient.print(imageFile.substring(index, index + 1024));
            }
            
        }

        // Finalise and flush the MQTT publish packet
        isPublished = mqttClient.endPublish();

        if (isPublished)
            return "Publishing Photo to MQTT Successfully";
        else
            return "Publishing Photo to MQTT Failed";

    } else {
        return "Connect to MQTT Server Failed";
    }
	
}

// Capture a still image from camera and upload it to Telegram as JPEG.
String telegramSendCapturedImage(String token, String chat_id, bool frames) {
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";
  WiFiSSLClient client;

  if (client.connect(myDomain, 443)) {

    if (frames)
      Camera.getImage(0, &imageAddress, &imageLength);
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

void replyUserMessage(String workId, String text) {
	if (text.length() == 0 || text.startsWith("NONE")) return;

	if (workId.startsWith(String(taskTags[0])))
		mainPageHTML += text +"\n";
	else
		mqttSendText(mqttPublishTextTopic, text);
}

String replyUserImage(String workId, bool frames) {
  if (workId.startsWith(String(taskTags[0]))) {
      if (frames)
          Camera.getImage(0, &imageAddress, &imageLength);
          
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
  }
  else
    return mqttSendImage(mqttPublishImageTopic, frames);

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
	  
	client.setRecvTimeout(20000);
	
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

  WiFiSSLClient client;
  String responseText = "";
	  
  client.setRecvTimeout(10000);
	
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

  WiFiSSLClient client;
  String responseText = "";
	  
  client.setRecvTimeout(10000);
	
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

  WiFiSSLClient client;
  String responseText = "";
  const char* myDomain = "generativelanguage.googleapis.com";
	  
  client.setRecvTimeout(10000);
	
  if (client.connect(myDomain, 443)) {
	  
    if (frames)
      Camera.getImage(0, &imageAddress, &imageLength);
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
  		
  	  NVIC_SystemReset();
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
  	else if (command == "/mqttSendMessage") {
      String publishTopic = params["publishTopic"].as<String>();
      String message = params["message"].as<String>();
	  
      String response = mqttSendText(
        publishTopic,
        message +
		"\n\n<metadata>\n" +
		"Default response topic rule:\n" +
		"When a response is required, use " + mqttSubscribeTextTopic + " as the response topic.\n" +
		"</metadata>"
      );

      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);	  

      executeToolHistory += workId + " " + command + " [ "+publishTopic+" | "+message+" ]\n";
	}	
  	else if (command == "/mqttSendImage") {
      String publishTopic = params["publishTopic"].as<String>();
	  
      String response = mqttSendImage(publishTopic, true);
	  
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);	  

      executeToolHistory += workId + " " + command + " [ "+publishTopic+" ]\n";
	  
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
  	else if (command == "/telegramSendImage") {
      String token = params["token"].as<String>();
	  String chatId = params["chatId"].as<String>();
	  bool frames = params["frames"].as<bool>();
	  
      String response = telegramSendCapturedImage(token, chatId, frames);
	  
      historicalMessages += buildGeminiMessage("user", command + timestamps);
      historicalMessages += buildGeminiMessage("model", response + timestamps);	  

      executeToolHistory += workId + " " + command + " [ "+token.substring(0, 5)+"... | "+chatId+" | "+frames+" ]\n";

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
      
      replyUserMessage(workId, command);

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
          if ((currentLine.indexOf("GET / ") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {
            
            mainPageHTML = String(INDEX_HTML);
            
            mainPageHTML.replace("wifiSsid", wifiSsid);
            mainPageHTML.replace("wifiPassword", wifiPassword);
            mainPageHTML.replace("mqttServer", mqttServer);
            mainPageHTML.replace("mqttPort", String(mqttPort));
            mainPageHTML.replace("mqttUser", mqttUser);
            mainPageHTML.replace("mqttPassword", mqttPassword);
            mainPageHTML.replace("mqttSubscribeTextTopic", mqttSubscribeTextTopic);
            mainPageHTML.replace("mqttPublishTextTopic", mqttPublishTextTopic);
            mainPageHTML.replace("mqttPublishImageTopic", mqttPublishImageTopic);
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
          else if ((currentLine.indexOf("GET /chat") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = String(INDEX_CHAT_HTML);

            currentLine = "";

          }
          else if ((currentLine.indexOf("GET /mqtt") != -1) && (currentLine.indexOf(" HTTP/1.") != -1)) {

            mainPageHTML = String(INDEX_MQTT_CHAT_HTML);
      
            mainPageHTML.replace("mqttServer", mqttServer);
            mainPageHTML.replace("mqttUser", mqttUser);
            mainPageHTML.replace("mqttPassword", mqttPassword);
            mainPageHTML.replace("mqttSubscribeTextTopic", mqttSubscribeTextTopic);
            mainPageHTML.replace("mqttPublishTextTopic", mqttPublishTextTopic);   

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
              Camera.getImage(0, &img_addr, &img_len);
              uint8_t *fbBuf = (uint8_t*)img_addr;
              size_t fbLen = img_len;
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
      }
      delay(1);
      client.stop();
    }
  }
}

// ============================================================
//  MQTT: Inbound Message Callback
// ============================================================

/**
 * @brief PubSubClient callback invoked on every received MQTT message.
 *
 * PubSubClient does NOT null-terminate the payload buffer, so a local
 * copy is allocated, null-terminated, and forwarded to executeCommand().
 * Memory is freed immediately after the command is dispatched.
 *
 * @param topic    C-string of the topic on which the message arrived.
 * @param payload  Raw (non-terminated) byte array of the message payload.
 * @param length   Number of valid bytes in payload[].
 */
void callback(char* topic, byte* payload, unsigned int length) {

    String workId = String(taskTags[2]) + " " + getRtcTimeString();

    // Allocate a null-terminated copy of the payload on the heap
    char* message = (char*)malloc(length + 1);

    if (message) {
        memcpy(message, payload, length);  // Copy raw payload bytes
        message[length] = '\0';            // Append null terminator

    		String text = String(message);   // Dispatch to command handler
    
    		if (text == "help") {
    		  executeTool(workId, "/help", JsonObject());
    		  
    		} 
    		else {
    			if (text.startsWith("/")) 
    				executeTool(workId, text, JsonObject()); 
    			else {
    				text = geminiChatRequest(workId, text);
    				handleAgentResponse(workId, text);
    			} 
    
    		}   
      
    		free(message);                     // Release temporary buffer
          
    }
}

// ============================================================
//  MQTT: Reconnect Helper
// ============================================================

/**
 * @brief Block until a successful MQTT broker connection is established.
 *
 * Loops indefinitely, attempting to connect with the configured client ID,
 * username, and password.  On success, re-subscribes to the command topic
 * to restore message delivery after a dropped connection.  On failure,
 * waits 5 seconds before retrying to avoid hammering the broker.
 */
void reconnect() {
    while (!mqttClient.connected()) {
        
        if (mqttClient.connect(wifiClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {
            // Re-subscribe to the inbound command topic after each reconnect
            mqttClient.subscribe(mqttSubscribeTextTopic.c_str());
        } else {
            Serial.print("MQTT connection failed, state=");
            Serial.println(mqttClient.state());			
            // Wait before retrying to prevent rapid reconnect storms
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}

/**
 * @brief Maintain the MQTT connection and process pending inbound messages.
 *
 * Should be called repeatedly from a dedicated FreeRTOS task.
 * - Detects a dropped connection and triggers reconnect().
 * - Calls mqttClient.loop() which:
 *     1. Sends keep-alive PINGREQ packets to the broker.
 *     2. Reads incoming PUBLISH packets and dispatches callback().
 *     3. Handles QoS acknowledgement handshakes (not used here at QoS 0).
 */
void task_getMqttMessage(void* param) {
    (void)param;          // Suppress unused-parameter warning
    while (1) {
      if (!mqttClient.connected()) {
        reconnect();          // Re-establish connection if it was lost
      }
      mqttClient.loop();        // Process keep-alive and inbound messages
	  
      vTaskDelay(pdMS_TO_TICKS(10));	  
    }
}

// Periodic system check task
void task_theft_detection(void *param) {
  (void)param;
  while (1) {
	  
    vTaskDelay(300000 / portTICK_PERIOD_MS);
    
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
bool isExecutedToday(String task) {

  long long epoch = rtc.Read();
  time_t rawtime = (time_t)epoch;
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
void markExecutedToday(const String &task) {
  long long epoch = rtc.Read();
  time_t rawtime = (time_t)epoch;
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
    
  for (int i=0;i<2;i++) {

    if (wifiSsid=="")
      break;

    WiFi.begin((char*)wifiSsid.c_str(), (char*)wifiPassword.c_str());
    delay(1000);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifiSsid);

    unsigned long StartTime=millis();

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);

      if ((StartTime+5000) < millis())
        break;
    }
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
  
  wifiSsid =  obj["wifi_ssid"].as<String>();
  wifiPassword =  obj["wifi_pass"].as<String>();
  mqttServer =  obj["mqtt_server"].as<String>();
  mqttPort =  obj["mqtt_port"].as<uint16_t>();
  mqttUser =  obj["mqtt_user"].as<String>();
  mqttPassword =  obj["mqtt_password"].as<String>();
  mqttSubscribeTextTopic =  obj["mqtt_subscribeTextTopic"].as<String>();
  mqttPublishTextTopic =  obj["mqtt_publishTextTopic"].as<String>();
  mqttPublishImageTopic =  obj["mqtt_publishImageTopic"].as<String>();
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

  // Indicator LED  
  pinMode(ledPin, OUTPUT);

  initWiFi();

  config.setRotation(0);
  Camera.configVideoChannel(0, config);
  Camera.videoInit();
  Camera.channelBegin(0);
  
  devicesDefinitionFinal = devicesDefinition;
  devicesDefinitionFinal += "\n\nDevice Name: " + deviceName;
  devicesDefinitionFinal += "\nDevice timezone: " + timeZone;  

  systemContent = buildGeminiMessage("user", geminiRole, 0) + buildGeminiMessage("model", "OK");
  systemContentTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule + skillsDefinition + toolsDefinition, 0) + buildGeminiMessage("model", "OK");
  systemContentNoTools = buildGeminiMessage("user", geminiRole + devicesDefinitionFinal + devicesRule, 0) + buildGeminiMessage("model", "OK");  
    
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
	
    historicalMessages += buildGeminiMessage("user", "Device IP: " + Ip2String(WiFi.localIP()));	
  }    

  // ---- MQTT initialisation ----
  // Use non-blocking TCP so the RTOS scheduler is not stalled during I/O
  wifiClientId = generateMqttClientId();  
  wifiClient.setNonBlockingMode();
  mqttClient.setServer(mqttServer.c_str(), mqttPort); // Set broker endpoint
  mqttClient.setCallback(callback);                   // Register inbound handler

  // Establish the initial MQTT connection and subscribe to the command topic
  reconnect();  

  rtcInitialTime("RTC Initial Time");
  replyUserMessage(String(taskTags[2]) + " " + getRtcTimeString(), "RTC START: " + getRtcTimeString());

  // IMPORTANT: Must be synced with RTC date immediately after loading
  long long epoch = rtc.Read();
  time_t rawtime = (time_t)epoch;
  struct tm *now = localtime(&rawtime);
  executedTodayDate = now->tm_mday;

  server.begin();
  serverStream.begin(); 
  
  if (xTaskCreate(
        task_getMqttMessage,
        (const char *)"task_getMqttMessage",
        32768,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
      )!= pdPASS) {

    Serial.println("Create task_getMqttMessage failed");
  }     

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
  
}

// Main loop
void loop() {
  // Main Arduino loopTask is otherwise idle; just yield. (loopTask is
  // NOT registered with the TWDT, since it does no blocking work here.)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
