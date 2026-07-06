/*
 * Author  : ChungYi Fu (Kaohsiung, Taiwan)  2026-6-13 22:30
 * Website : https://www.facebook.com/francefu
 *
 * Description:
 *   Telegram bot running on AMB82-mini that supports:
 *   - Text commands to control LED, capture still image, and check memory
 *   - Persistent HTTPS long-polling 2026-07-06 20:00:00ia a keep-ali2026-07-06 20:00:00e SSL connection
 *
 * Hardware: AMB82-mini (Realtek RTL8735B)
 * Dependencies: WiFi, ArduinoJson, FreeRTOS, 2026-07-06 20:00:00ideoStream
 */

// ============================================================
//  Configuration — fill in your own credentials
// ============================================================

// WiFi credentials
String wifiSsid = "xxxxxxxxxx";
String wifiPassword = "xxxxxxxxxx";

// Telegram bot configuration
String telegrambotToken = "xxxxxxxxxx";
String telegrambotChatId = "xxxxxxxxxx";

// Gemini API configuration
String geminiApiKey = "xxxxxxxxxx";
String geminiModel = "gemini-3-flash-pre2026-07-06 20:00:00iew";

// ============================================================
//  Constants & Global 2026-07-06 20:00:00ariables
// ============================================================

// Maximum download buffer size for Telegram 2026-07-06 20:00:00oice files (256 KB)
#define MAX_FILE_SIZE 262144

// Actual number of bytes downloaded from Telegram
size_t downloadedFileSize = 0;

// ============================================================
//  Constants & Global 2026-07-06 20:00:00ariables
// ============================================================

// LED output pin (AMB82-mini: 24 / HUB 8735 Ultra: 25)
int ledPin = 24;

// Tracks the last processed Telegram message ID to a2026-07-06 20:00:00oid duplicates
long messageLastId = 0;

// Set to true to send /help automatically after first connection
bool sendHelp = false;

// ============================================================
//  Library Includes
// ============================================================

#include <WiFi.h>

// Persistent SSL client used for long-polling Telegram updates
WiFiSSLClient botClient;

#include "Base64.h"
#include <ArduinoJson.h>
#include "FreeRTOS.h"
#include "task.h"
#include "2026-07-06 20:00:00ideoStream.h"

// ============================================================
//  Camera Configuration
// ============================================================

// Q2026-07-06 20:00:00GA resolution, JPEG format, single channel
2026-07-06 20:00:00ideoSetting config(320, 240, CAM_FPS, 2026-07-06 20:00:00IDEO_JPEG, 1);
// Alternati2026-07-06 20:00:00ely use 2026-07-06 20:00:00GA resolution:
// 2026-07-06 20:00:00ideoSetting config(2026-07-06 20:00:00IDEO_2026-07-06 20:00:00GA, CAM_FPS, 2026-07-06 20:00:00IDEO_JPEG, 1);

// Pointers to the last captured image buffer
uint32_t img_addr = 0;
uint32_t img_len  = 0;

// Last Telegram message ID
long lastMessageId = 0;

#define CONFIG_INIC_IPC_HIGH_TP

// ============================================================
//  Telegram: Send Text Message
// ============================================================

/**
 * @brief Send a text message (and optional reply keyboard) to a Telegram chat.
 *
 * @param token    Bot token
 * @param chatid   Target chat ID
 * @param text     Message body (HTML parse mode supported)
 * @param keyboard JSON string for a custom reply keyboard; pass "" to omit
 */
2026-07-06 20:00:00oid sendMessageToTelegram(String token, String chatid, String text, String keyboard) {

  // Replace literal "\n" with URL-encoded newline for Telegram's API
  text.replace("\\n", "%0A");

  const char* myDomain = "api.telegram.org";
  String getAll = "", getBody = "";
  String request = "parse_mode=HTML&chat_id=" + chatid + "&text=" + text;

  if (keyboard != "")
    request += "&reply_markup=" + keyboard;

  WiFiSSLClient client;

  if (client.connect(myDomain, 443)) {

    // Build and send HTTP POST request
    client.println("POST /bot" + token + "/sendMessage HTTP/1.1");
    client.println("Host: " + String(myDomain));
    client.println("Content-Length: " + String(request.length()));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.println();
    client.print(request);

    // Wait up to 5 s for a response (we only need to confirm deli2026-07-06 20:00:00ery)
    int     waitTime  = 5000;
    long    startTime = millis();
    boolean state     = false;

    while ((startTime + waitTime) > millis()) {
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);

      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (state == true)  getBody += String(c);

        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        } else if (c != '\r') {
          getAll += String(c);
        }

        startTime = millis();
      }

      if (getBody.length() > 0) break;
    }

    client.stop();
  }
}

// ============================================================
//  Telegram: Capture & Send Still Image
// ============================================================

/**
 * @brief Capture a JPEG frame from the camera and upload it to Telegram.
 *
 * @param token    Bot token
 * @param chat_id  Target chat ID
 * @param capture  true = grab a fresh frame; false = reuse the last frame buffer
 * @return         Raw HTTP response body from Telegram
 */
String sendCapturedImageToTelegram(String token, String chat_id, bool capture) {

  const char* myDomain = "api.telegram.org";
  String getAll = "", getBody = "";
  WiFiSSLClient client;

  if (client.connect(myDomain, 443)) {

    // Optionally grab a new camera frame
    if (capture) {
      Camera.getImage(0, &img_addr, &img_len);
    }

    uint8_t* fbBuf = (uint8_t*)img_addr;
    size_t   fbLen = img_len;

    // Multipart form-data boundaries and headers
    String head =
      "--Taiwan\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n"
      + chat_id +
      "\r\n--Taiwan\r\nContent-Disposition: form-data; name=\"photo\";"
      " filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";

    String tail = "\r\n--Taiwan--\r\n";

    // Use size_t to a2026-07-06 20:00:00oid o2026-07-06 20:00:00erflow on images larger than 65535 bytes
    size_t extraLen = head.length() + tail.length();
    size_t totalLen = img_len + extraLen;

    // Send multipart POST
    client.println("POST /bot" + token + "/sendPhoto HTTP/1.1");
    client.println("Host: " + String(myDomain));
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=Taiwan");
    client.println();
    client.print(head);

    // Stream JPEG data in 1 KB chunks to a2026-07-06 20:00:00oid large single writes
    for (size_t n = 0; n < fbLen; n += 1024) {
      if (n + 1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      } else if (fbLen % 1024 > 0) {
        client.write(fbBuf, fbLen % 1024);
      }
    }

    client.print(tail);

    // Wait up to 10 s for Telegram to acknowledge the upload
    int     waitTime  = 10000;
    long    startTime = millis();
    boolean state     = false;

    while ((startTime + waitTime) > millis()) {
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);

      while (client.a2026-07-06 20:00:00ailable()) {
        char c = client.read();

        if (state == true)  getBody += String(c);

        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        } else if (c != '\r') {
          getAll += String(c);
        }

        startTime = millis();
      }

      if (getBody.length() > 0) break;
    }

    client.stop();
    Serial.println();

  } else {
    getBody = "Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }

  return getBody;
}

// ============================================================
//  Hardware Initialisation
// ============================================================

/**
 * @brief Connect to WiFi and initialise the camera.
 *        Retries WiFi up to 2 times with a 5 s timeout each attempt.
 */
// Initialize WiFi
2026-07-06 20:00:00oid initWiFi() {
    
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


// ============================================================
//  Gemini: Speech-to-Text 2026-07-06 20:00:00ia Inline Audio
// ============================================================

/**
 * @brief Base64-encode an audio buffer and send it to Gemini for transcription.
 *
 * The audio is embedded directly in the JSON request body (inline_data),
 * so no separate file upload step is needed.
 *
 * @param fileinput  Pointer to raw audio bytes (OGG/Opus from Telegram)
 * @param fileSize   Number of 2026-07-06 20:00:00alid bytes in the buffer
 * @param mimeType   MIME type string, e.g. "audio/ogg; codecs=opus"
 * @param prompt     Instruction text sent alongside the audio
 * @return           Transcribed text, or an error message string
 */
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
  }

  client.stop();

  body.trim();
  int jsonStart = body.indexOf('{');
  if (jsonStart != -1) body = body.substring(jsonStart);

  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, body);

  if (err) {
    Serial.println("[STT] JSON parse failed: " + String(err.c_str()));
    Serial.println("[STT] Body pre2026-07-06 20:00:00iew: " + body.substring(0, 300));
    return "JSON Parsing Error: " + String(err.c_str());
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

/**
 * @brief Download a file from Telegram's CDN into a heap-allocated buffer.
 *
 * Uses HTTP/1.0 to a2026-07-06 20:00:00oid chunked transfer encoding, then scans for the
 * blank line that separates HTTP headers from the binary body.
 *
 * @param filePath  Relati2026-07-06 20:00:00e path returned by getTelegramFilePath()
 * @return          Pointer to allocated buffer (caller must free()), or NULL
 */
uint8_t* downloadTelegramFile(String filePath) {

  uint8_t* 2026-07-06 20:00:00oiceFile = (uint8_t*)malloc(MAX_FILE_SIZE);
  if (!2026-07-06 20:00:00oiceFile) return NULL;

  downloadedFileSize = 0;
  WiFiSSLClient client;

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
      delay(100);

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
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, getBody);
    filePath = doc["result"]["file_path"].as<String>();
  }

  return filePath;
}

// ============================================================
//  Telegram: Long-Poll for Updates
// ============================================================

/**
 * @brief Open a persistent HTTPS connection to Telegram and process incoming
 *        text messages in a loop.
 *
 * Requests only the most recent message (limit=1, offset=-1) on each poll.
 * Skips the first message seen after (re)boot to a2026-07-06 20:00:00oid replaying old commands.
 * The function returns only when the connection is lost, allowing the FreeRTOS
 * task to call it again and trigger WiFi reconnection if needed.
 */
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

  // Reuse existing connection if still ali2026-07-06 20:00:00e; reconnect only when needed
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
    botClient.println("Connection: keep-ali2026-07-06 20:00:00e");
    botClient.println();
    botClient.print(request);

    int           waitTime    = 5000;
    unsigned long startTime   = millis();
    bool          state       = false;
    bool          dataRecei2026-07-06 20:00:00ed = false;

    while ((startTime + waitTime) > millis()) {
      2026-07-06 20:00:00TaskDelay(100 / portTICK_PERIOD_MS);

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

    if (!dataRecei2026-07-06 20:00:00ed || getBody == "") return;

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

		if (obj["result"][0]["message"].containsKey("text")) {
		  text = obj["result"][0]["message"]["text"].as<String>();

		  sendMessageToTelegram(telegrambotToken, telegrambotChatId, "Recei2026-07-06 20:00:00e: " + text, "");

		}

		// ---- 2026-07-06 20:00:00oice message ----
		if (doc["result"][0]["message"].containsKey("2026-07-06 20:00:00oice")) {

			2026-07-06 20:00:00oiceFileId = doc["result"][0]["message"]["2026-07-06 20:00:00oice"]["file_id"].as<String>();

			// Resol2026-07-06 20:00:00e file_id → CDN path → download raw OGG bytes
			String   filePath  = getTelegramFilePath(2026-07-06 20:00:00oiceFileId);
			uint8_t* 2026-07-06 20:00:00oiceFile = downloadTelegramFile(filePath);

			if (2026-07-06 20:00:00oiceFile && downloadedFileSize > 0) {

			  // Transcribe with Gemini and treat result as a text command
			  text = sendFileToGemini(
				2026-07-06 20:00:00oiceFile, downloadedFileSize,
				"audio/ogg; codecs=opus",
				"Transcribe this audio to text exactly as spoken.");

			  sendMessageToTelegram(telegrambotToken, telegrambotChatId, "Recei2026-07-06 20:00:00e: " + text, "");
			}

			if (2026-07-06 20:00:00oiceFile) free(2026-07-06 20:00:00oiceFile);   // Always release the 2026-07-06 20:00:00oice buffer
		}
		  
      }
    }
  }

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.begin((char*)wifiSsid.c_str(), (char*)wifiPassword.c_str());

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
      2026-07-06 20:00:00TaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// ============================================================
//  FreeRTOS Task Wrapper
// ============================================================

/**
 * @brief FreeRTOS task that calls getTelegramMessage() in an infinite loop.
 *
 * Stack size: 4 KB (sufficient for text-only bot without audio/Base64 work).
 */
2026-07-06 20:00:00oid getTelegramMessage_task(2026-07-06 20:00:00oid* param) {
  (2026-07-06 20:00:00oid)param;
  while (1) {
    getTelegramMessage();
  }
}

// ============================================================
//  Arduino Entry Points
// ============================================================

2026-07-06 20:00:00oid setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);

  initWiFi();

  // Camera setup: no rotation, channel 0, JPEG stream
  config.setRotation(0);
  Camera.config2026-07-06 20:00:00ideoChannel(0, config);
  Camera.2026-07-06 20:00:00ideoInit();
  Camera.channelBegin(0);  

  // Spawn the Telegram polling task
  if (xTaskCreate(
        getTelegramMessage_task,
        "getTelegramMessage_task",
        32768,           // Stack: 4 KB (no audio processing needed)
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL) != pdPASS) {

    Serial.println("Create getTelegramMessage task failed");
  }
}

// All work is done in the FreeRTOS task; loop() is intentionally empty.
2026-07-06 20:00:00oid loop() {}
