/*
Author:
  ChungYi Fu (Kaohsiung, Taiwan)
  https://www.facebook.com/francefu

Repository:
  https://github.com/fustyles/fuClaw
------------------------------------------------------------
version
-----------------------------------------------------------
Build Date: 2026-07-06 22:00
------------------------------------------------------------
Overview
------------------------------------------------------------
Realtek Ameba Pro2 devices:

- AMB82-mini
- HUB 8735 Ultra
------------------------------------------------------------
Persistent Files
------------------------------------------------------------
index.html
  fuClaw configuration web page

index_chat.html
  Gemini talk web page (Web Chat Interface)
------------------------------------------------------------
Hardware Safety
------------------------------------------------------------
Confirmed device mappings only.

AMB82-mini
- Green LED : GPIO 24
- Blue LED  : GPIO 23

HUB 8735 Ultra
- Green LED : GPIO 25
- Blue LED  : GPIO 26
- Fill LED  : GPIO 13
- Button    : GPIO 12 (input only, active-low)
------------------------------------------------------------
Software Stack
------------------------------------------------------------
- WiFi.h
- WiFiSSLClient
- ArduinoJson
- FreeRTOS
- videoStream
- Base64
------------------------------------------------------------
*/

// WiFi credentials
String wifiSsid = "xxxxxxxxxx";
String wifiPassword = "xxxxxxxxxx";

// AP credentials http://192.168.1.1:81
String apSsid = "fuclaw";
String apPassword = "12345678";

// MQTT broker settings
String mqttServer   = "mqttgo.io";                          // Broker hostname or IP
uint16_t mqttPort   = 1883;                                  // Standard MQTT port (unencrypted)
String mqttUser     = "";                                    // Leave empty if no auth required
String mqttPassword = "";                                    // Leave empty if no auth required

// MQTT topic strings
//   Subscribe topic : broker pushes incoming commands here
//   Publish topics  : device pushes text replies and camera images here
String mqttSubscribeTextTopic      = "fuclaw1/subscribe";       // Inbound command topic
String mqttPublishTextTopic        = "fuclaw1/publish";         // Outbound text reply topic
String mqttPublishImageTopic       = "fuclaw1/publishimage";    // Outbound JPEG topic

// Stores the MQTT Client ID for this device (generated from MAC address to ensure uniqueness)
String wifiClientId = "";

// Gemini API configuration
String geminiApiKey = "xxxxxxxxxx";
String geminiModel = "gemini-3-flash-preview";

int geminiMaxOutputTokens = 8192;  // If the AI ​​is unable to transmit complete data, please increase the value.
float geminiTemperature = 1.0;

String systemContent = "";

String mainPageHTML = "";
bool mainPageStatus = false;

// System prompt that defines assistant behavior.
// Must be JSON-safe (avoid invalid escape characters or unsupported symbols).
String geminiRole = R"(
You are a professional assistant with a lively, natural, and friendly personality, responding according to the user's language.
)"; 
  
// Stores entire chat history in Gemini API JSON format
// Used to preserve conversation memory across requests
String historicalMessages = "";

// Indicator LED output pin
int ledPin = 24;    // green led (AMB82-mini: 24, HUB 8735 Ultra: 25)

#include <WiFi.h>

// Underlying TCP socket used by PubSubClient
WiFiClient wifiClient;

char channel_ap[] = "2";
WiFiServer server(81);

#include "Base64.h"
#include <ArduinoJson.h>
#include "FreeRTOS.h"
#include "task.h"

#include <PubSubClient.h> // MQTT client (Nick O'Leary / knolleary)

// MQTT client instance bound to the WiFi socket
PubSubClient mqttClient(wifiClient);

// Web page
#include "index_chat.h";    // TCP Chat
#include "index_mqtt_chat.h";    // MQTT Chat

#include "videoStream.h"

// Camera video configuration
videoSetting config(320, 240, CAM_FPS, vIDEO_JPEG, 1);
//videoSetting config(vIDEO_vGA, CAM_FPS, vIDEO_JPEG, 1);

// Captured image buffer address and length
uint32_t imageAddress = 0;
uint32_t imageLength = 0;

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
      bool isPublished = mqttClient.publish(topic.c_str(), text.c_str());

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
String mqttSendImage(String topic, bool base64 = false) {

    // Attempt to connect (or re-use the existing session)
    if (mqttClient.connect(wifiClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {

        Camera.getImage(0, &imageAddress, &imageLength);

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

void replyUserMessage(String workId, String text) {
if (text.startsWith("NONE") || text == "") return;
  
	if (workId.startsWith("<PAGE>") && !text.startsWith("<PAGE>")) {
		if (text.indexOf("<PAGE>") != -1)
			text = text.substring(0, text.indexOf("<PAGE>"));
		mainPageHTML += text;
	}
	else if (workId.startsWith("<MQTT>") && !text.startsWith("<MQTT>")) {
		if (text.indexOf("<MQTT>") != -1)
			text = text.substring(0, text.indexOf("<MQTT>"));
		mqttSendText(mqttPublishTextTopic, text);
	}
	else
	mqttSendText(mqttPublishTextTopic, text);

}

String replyUserImage(String workId) {
  if (workId.startsWith("<PAGE>")) {
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
  else if (workId.startsWith("<MQTT>")) {
    return mqttSendImage(mqttPublishImageTopic);
  }

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

// Reset conversation memory to initial system prompt state
void geminiChatReset() {
  
  historicalMessages = "";

  systemContent = buildGeminiMessage("user", geminiRole, false) + buildGeminiMessage("model", "OK");
  
}

// Send request to Gemini and return response text
String geminiChatRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;
  
  historicalMessages += buildGeminiMessage("user", message + timestamps);

  String contents = systemContent + historicalMessages;
    

  String request = "{\"contents\": [" + contents +
                   "],\"generationConfig\": {\"maxOutputTokens\": " +
                   geminiMaxOutputTokens +
                   ", \"temperature\": " + geminiTemperature + "}}";

  WiFiSSLClient client;
  String responseText = "";

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
      responseText = "Gemini API Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
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

  responseText.replace(timestamps, "");
  responseText.replace(workId, "");
  historicalMessages += buildGeminiMessage("model", responseText + timestamps);

  return responseText;
  
}

// Send Gemini request with Google Search tool enabled
String geminiSearchRequest(String workId, String message, int tools = 1) {
  String timestamps = "\n" + workId;
  
  historicalMessages += buildGeminiMessage("user", message + timestamps);

  String contents = systemContent + historicalMessages;

  // Build request with Google Search tool
  String request = "{\"contents\": [" + contents +
                   "],\"tools\": [{\"google_search\": {}}],\"generationConfig\": {\"maxOutputTokens\": " +
                   geminiMaxOutputTokens +
                   ", \"temperature\": " + geminiTemperature + "}}";

  WiFiSSLClient client;
  String responseText = "";

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
      responseText = "Gemini Search API Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
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
  
  responseText.replace(timestamps, "");
  responseText.replace(workId, "");
  historicalMessages += buildGeminiMessage("model", responseText + timestamps);

  return responseText;
}

// Capture camera frame and send it to Gemini vision for multimodal analysis
String geminivisionRequest(String workId, String message) {
  String timestamps = "\n" + workId;
  
  historicalMessages += buildGeminiMessage("user", message + timestamps);

  WiFiSSLClient client;
  String responseText = "";
  const char* myDomain = "generativelanguage.googleapis.com";

  if (client.connect(myDomain, 443)) {
    Camera.getImage(0, &imageAddress, &imageLength);
    
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
      responseText = "Gemini vision API Error: " + doc["error"]["message"].as<String>();
      Serial.println(responseText);
    } 
    else {
      responseText = "Unexpected response from Gemini vision.";
    }

  } else {
    Serial.println("Failed to connect to Gemini API (vision)");
    responseText = "Connection failed";
  }

  if (responseText == "") {
    responseText = "Gemini vision did not respond. Please try again.";
  }

  responseText.replace(timestamps, "");
  responseText.replace(workId, "");
  historicalMessages += buildGeminiMessage("model", responseText + timestamps);

  return responseText;
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

// fuClaw configuration web page. Users can set system parameters from the webpage.
void task_getRequest(void *param) {
  (void)param;
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
            
            mainPageHTML = "Welcome to <a href=\"https://github.com/fustyles/fuClaw\">fuClaw</a> home!";
                
            currentLine = "";            
          } 
          else if (currentLine.startsWith("GET /chat") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = String(INDEX_CHAT_HTML);

            currentLine = "";

          }             
          else if (currentLine.startsWith("GET /mqtt") && currentLine.endsWith(" HTTP/1.")) {

            mainPageHTML = String(INDEX_MQTT_CHAT_HTML);
      
            mainPageHTML.replace("mqttServer", mqttServer);
            mainPageHTML.replace("mqttUser", mqttUser);
            mainPageHTML.replace("mqttPassword", mqttPassword);
            mainPageHTML.replace("mqttSubscribeTextTopic", mqttSubscribeTextTopic);
            mainPageHTML.replace("mqttPublishTextTopic", mqttPublishTextTopic);   

            currentLine = "";

          }  
    		  else if (currentLine.startsWith("GET /message?") && currentLine.endsWith(" HTTP/1.")) {
            
            String workId = "<PAGE>";       

            currentLine.replace("GET /message?", "");
            currentLine.replace(" HTTP/1.", "");

            if (currentLine != "") {
				      currentLine = urldecode(currentLine);  
				
      				if (currentLine == "/still")
      					replyUserImage(workId);
      				else if (currentLine == "/vision")
      					replyUserMessage(workId, geminivisionRequest(workId, "Describe the image in detail.")); 
      				else {
      					replyUserMessage(workId, geminiChatRequest(workId, currentLine)); 
      				}
            }

            currentLine = "";

    	    }      
        }
      }

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

    String workId = "<MQTT>";

    // Allocate a null-terminated copy of the payload on the heap
    char* message = (char*)malloc(length + 1);

    if (message) {
      memcpy(message, payload, length);  // Copy raw payload bytes
      message[length] = '\0';            // Append null terminator
  
  		String text = String(message);   // Dispatch to command handler
  
  		if (text == "/still")
  			replyUserImage(workId);
  		else {
  			replyUserMessage(workId, geminiChatRequest(workId, text)); 
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
    }
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
  Camera.configvideoChannel(0, config);
  Camera.videoInit();
  Camera.channelBegin(0);

  systemContent = buildGeminiMessage("user", geminiRole, 0) + buildGeminiMessage("model", "OK"); 
    
  server.begin();
  
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
 
  Serial.println("\n");
  Serial.println("Main page\nhttp://192.168.1.1:81/chat");    
  Serial.println("Web Chat\nhttp://192.168.1.1:81/chat");
  Serial.println("Web Chat via MQTT\nhttp://192.168.1.1:81/mqtt"); 
  Serial.println("AP ssid : " + apSsid);
  Serial.println("AP password : " + apPassword);
  Serial.println("\n");  

  if (WiFi.status() == WL_CONNECTED) {
    for (int i=0 ; i<3 ; i++) {
      digitalWrite(ledPin, 1);
      delay(300);
      digitalWrite(ledPin, 0);
      delay(300);      
    }

    Serial.println("Main page\nhttp://" + Ip2String(WiFi.localIP()) + ":81");     
    Serial.println("Web Chat\nhttp://" + Ip2String(WiFi.localIP()) + ":81/chat");
    Serial.println("Web Chat via MQTT\nhttp://" + Ip2String(WiFi.localIP()) + ":81/mqtt");        
    Serial.println("\n");   
  }   

  // ---- MQTT initialisation ----
  // Use non-blocking TCP so the RTOS scheduler is not stalled during I/O
  wifiClientId = generateMqttClientId();    
  wifiClient.setNonBlockingMode();
  mqttClient.setServer(mqttServer.c_str(), mqttPort); // Set broker endpoint
  mqttClient.setCallback(callback);                   // Register inbound handler

  // Establish the initial MQTT connection and subscribe to the command topic
  reconnect();  

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
 
}

// Main loop
void loop() {
 
}
