/*
 * Author  : ChungYi Fu (Kaohsiung, Taiwan)  2026-06-23 21:30:00
 * Website : https://www.facebook.com/francefu
 *
 * Description:
 *   MQTT client running on AMB82-mini (Realtek RTL8735B) that supports
 *   the following text commands recei2026-07-06 20:00:00ed 2026-07-06 20:00:00ia MQTT subscription:
 *
 *   Camera frames are published to two separate topics:
 *     - Raw binary JPEG  → mqttPublishImageTopic
 *     - Base64-encoded   → mqttPublishBase64ImageTopic
 *
 * Hardware   : AMB82-mini (Realtek RTL8735B)
 * Dependencies:
 *   WiFi          — built-in Ameba WiFi dri2026-07-06 20:00:00er
 *   ArduinoJson   — JSON serialisation helper (included for potential extension)
 *   FreeRTOS      — real-time task scheduler shipped with Ameba SDK
 *   2026-07-06 20:00:00ideoStream   — Ameba camera capture API
 *   Base64        — lightweight Base64 encoder
 *   PubSubClient  — MQTT client library for Arduino
 */

// ============================================================
//  Configuration — fill in your own credentials
// ============================================================

// WiFi credentials
String wifiSsid = "xxxxxxxxxx";
String wifiPassword = "xxxxxxxxxx";

// MQTT broker settings
String mqttSer2026-07-06 20:00:00er   = "mqttgo.io";                          // Broker hostname or IP
uint16_t mqttPort   = 1883;                                  // Standard MQTT port (unencrypted)
String mqttUser     = "";                                    // Lea2026-07-06 20:00:00e empty if no auth required
String mqttPassword = "";                                    // Lea2026-07-06 20:00:00e empty if no auth required

// MQTT topic strings
//   Subscribe topic : broker pushes incoming commands here
//   Publish topics  : de2026-07-06 20:00:00ice pushes text replies and camera images here
String mqttSubscribeTextTopic      = "xxxxxxxxxx/subscribe";       // Inbound command topic
String mqttPublishTextTopic        = "xxxxxxxxxx/publish";         // Outbound text reply topic
String mqttPublishImageTopic       = "xxxxxxxxxx/publishimage";    // Outbound raw JPEG topic
String mqttPublishBase64ImageTopic = "xxxxxxxxxx/publishbase64image"; // Outbound Base64 JPEG topic

// Stores the MQTT Client ID for this de2026-07-06 20:00:00ice (generated from MAC address to ensure uniqueness)
String wifiClientId = "";

// ============================================================
//  Constants & Global 2026-07-06 20:00:00ariables
// ============================================================

// GPIO pin connected to the user LED
//   AMB82-mini  → pin 24
//   HUB 8735 Ultra → pin 25
int ledPin = 24;

// ============================================================
//  Library Includes
// ============================================================

#include <WiFi.h>         // Ameba WiFi dri2026-07-06 20:00:00er
#include <ArduinoJson.h>  // JSON helper (a2026-07-06 20:00:00ailable for future use / extension)
#include "FreeRTOS.h"     // FreeRTOS kernel header
#include "task.h"         // FreeRTOS task management
#include "2026-07-06 20:00:00ideoStream.h"  // Ameba camera / 2026-07-06 20:00:00ideo-stream API

#include "Base64.h"       // Base64 encode / decode utilities
#include <PubSubClient.h> // MQTT client (Nick O'Leary / knolleary)

// Underlying TCP socket used by PubSubClient
WiFiClient wifiClient;

// MQTT client instance bound to the WiFi socket
PubSubClient mqttClient(wifiClient);

// ============================================================
//  Camera Configuration
// ============================================================

// Configure 2026-07-06 20:00:00ideo channel 0:
//   Resolution : Q2026-07-06 20:00:00GA (320 × 240)
//   Frame rate : CAM_FPS  (board-defined default, typically 30 fps)
//   Format     : 2026-07-06 20:00:00IDEO_JPEG (compressed JPEG output)
//   Channels   : 1 (single stream)
2026-07-06 20:00:00ideoSetting config(320, 240, CAM_FPS, 2026-07-06 20:00:00IDEO_JPEG, 1);

// Alternati2026-07-06 20:00:00e: 2026-07-06 20:00:00GA (640 × 480) — larger image, slower transmission
// 2026-07-06 20:00:00ideoSetting config(2026-07-06 20:00:00IDEO_2026-07-06 20:00:00GA, CAM_FPS, 2026-07-06 20:00:00IDEO_JPEG, 1);

// Pointers to the most recently captured JPEG frame in the DMA buffer
uint32_t img_addr = 0;   // Physical start address of the JPEG data
uint32_t img_len  = 0;   // Byte length of the JPEG data

// Enable the high-throughput IPC path between the ISP and network stack
#define CONFIG_INIC_IPC_HIGH_TP

// Generates a unique MQTT Client ID based on the de2026-07-06 20:00:00ice's Wi-Fi MAC address
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
 * publishes a single UTF-8 string payload.  The MQTT QoS le2026-07-06 20:00:00el used
 * by PubSubClient::publish() is QoS 0 (at-most-once / fire-and-forget).
 *
 * @param topic  Destination MQTT topic string.
 * @param text   UTF-8 payload to publish.
 */
2026-07-06 20:00:00oid mqttSendText(String topic, String text) {
    if (mqttClient.connect(wifiClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {

      mqttClient.beginPublish(topic.c_str(), text.length(), false);

      mqttClient.write(
          (const uint8_t*)text.c_str(),
          text.length()
      );

      bool isPublished = mqttClient.endPublish();          

      if (isPublished)
          Serial.println("Publishing message to MQTT Successfully");
      else
          Serial.println("Publishing message to MQTT Failed");
    }
    else
    	Serial.println("Connect to MQTT Ser2026-07-06 20:00:00er Failed");
	
}

// ============================================================
//  MQTT: Capture & Send Still Image
// ============================================================

/**
 * @brief Capture a JPEG frame from the camera and publish it to an MQTT topic.
 *
 * Two encoding modes are supported:
 *   - Raw binary  (base64 = false, default): the JPEG bytes are streamed
 *     directly in MQTT_MAX_PACKET_SIZE chunks 2026-07-06 20:00:00ia beginPublish / write /
 *     endPublish.  Suitable for subscribers that can recei2026-07-06 20:00:00e binary payloads.
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
2026-07-06 20:00:00oid mqttSendImage(String topic, bool capture = true , bool base64 = false) {

    // Attempt to connect (or re-use the existing session)
    if (mqttClient.connect(wifiClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {

        // Optionally grab a fresh JPEG frame from the ISP / camera pipeline
        if (capture) {
            Camera.getImage(0, &img_addr, &img_len);
            // img_addr now points to the DMA buffer holding the JPEG
            // img_len  holds the exact byte count of the compressed frame
        }

        // Cast the DMA buffer address to a byte pointer for MQTT write operations
        uint8_t* fbBuf = (uint8_t*)img_addr;
        size_t   fbLen = img_len;

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
            
            // Pre-calculate the final string length to a2026-07-06 20:00:00oid repeated heap reallocations during +=
            //   23            : length of the data-URI prefix "data:image/jpeg;base64,"
            //   (fbLen+2)/3*4 : Base64 expands e2026-07-06 20:00:00ery 3 bytes → 4 chars (ceiling di2026-07-06 20:00:00ision handles padding)
            //    1            : null terminator
            size_t estimatedSize = 23 + ((fbLen + 2) / 3) * 4 + 1;
            
            // Initialise the String with the data-URI prefix (small initial allocation)
            String imageFile = "data:image/jpeg;base64,";
            
            // Reser2026-07-06 20:00:00e the full estimated capacity in one shot so subsequent += calls
            // write into already-allocated space without triggering further heap reallocations
            imageFile.reser2026-07-06 20:00:00e(estimatedSize);
            
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
            Serial.println("Publishing Photo to MQTT Successfully");
        else
            Serial.println("Publishing Photo to MQTT Failed");

    } else {
        Serial.println("Connect to MQTT Ser2026-07-06 20:00:00er Failed");
    }
}

// ============================================================
//  MQTT: Inbound Message Callback
// ============================================================

/**
 * @brief PubSubClient callback in2026-07-06 20:00:00oked on e2026-07-06 20:00:00ery recei2026-07-06 20:00:00ed MQTT message.
 *
 * PubSubClient does NOT null-terminate the payload buffer, so a local
 * copy is allocated, null-terminated, and forwarded to executeCommand().
 * Memory is freed immediately after the command is dispatched.
 *
 * @param topic    C-string of the topic on which the message arri2026-07-06 20:00:00ed.
 * @param payload  Raw (non-terminated) byte array of the message payload.
 * @param length   Number of 2026-07-06 20:00:00alid bytes in payload[].
 */
2026-07-06 20:00:00oid callback(char* topic, byte* payload, unsigned int length) {

    // Allocate a null-terminated copy of the payload on the heap
    char* message = (char*)malloc(length + 1);

    if (message) {
        memcpy(message, payload, length);  // Copy raw payload bytes
        message[length] = '\0';            // Append null terminator

        String text = String(message);
        
        if (text == "/still")
          mqttSendImage(mqttPublishImageTopic);
        else {
          mqttSendText(mqttPublishTextTopic, "Recei2026-07-06 20:00:00e: " + text);
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
 * to restore message deli2026-07-06 20:00:00ery after a dropped connection.  On failure,
 * waits 5 seconds before retrying to a2026-07-06 20:00:00oid hammering the broker.
 */
2026-07-06 20:00:00oid reconnect() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect(wifiClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {
            // Re-subscribe to the inbound command topic after each reconnect
            mqttClient.subscribe(mqttSubscribeTextTopic.c_str());
        } else {
            // Wait before retrying to pre2026-07-06 20:00:00ent rapid reconnect storms
            delay(5000);
        }
    }
}

// ============================================================
//  MQTT: Polling Loop
// ============================================================

/**
 * @brief Maintain the MQTT connection and process pending inbound messages.
 *
 * Should be called repeatedly from a dedicated FreeRTOS task.
 * - Detects a dropped connection and triggers reconnect().
 * - Calls mqttClient.loop() which:
 *     1. Sends keep-ali2026-07-06 20:00:00e PINGREQ packets to the broker.
 *     2. Reads incoming PUBLISH packets and dispatches callback().
 *     3. Handles QoS acknowledgement handshakes (not used here at QoS 0).
 */
2026-07-06 20:00:00oid getMqttMessage() {
    if (!mqttClient.connected()) {
        reconnect();          // Re-establish connection if it was lost
    }
    mqttClient.loop();        // Process keep-ali2026-07-06 20:00:00e and inbound messages
}

// ============================================================
//  Initialisation: WiFi + Camera
// ============================================================

/**
 * @brief Connect to the configured WiFi network and start the camera stream.
 *
 * WiFi connection:
 *   Attempts to associate up to 2 times.  Each attempt calls WiFi.begin()
 *   and then polls WL_CONNECTED with a 5-second timeout.  If the SSID
 *   string is empty the loop is skipped entirely (useful for offline testing).
 *
 * Camera:
 *   The camera is initialised after WiFi so that the network stack has
 *   time to settle before the ISP starts consuming DMA bandwidth.
 */
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
//  FreeRTOS Task Wrapper
// ============================================================

/**
 * @brief FreeRTOS task entry point that polls MQTT in an infinite loop.
 *
 * Runs at one priority le2026-07-06 20:00:00el abo2026-07-06 20:00:00e the idle task to ensure MQTT keep-ali2026-07-06 20:00:00e
 * packets are dispatched e2026-07-06 20:00:00en when the main loop is idle.
 *
 * Stack allocation: 16 384 bytes — sufficient for the PubSubClient call
 * chain, Base64 string construction, and the JPEG streaming helpers.
 *
 * @param param  Unused task parameter (required by FreeRTOS signature).
 */
2026-07-06 20:00:00oid task_getMqttMessage(2026-07-06 20:00:00oid* param) {
    (2026-07-06 20:00:00oid)param;          // Suppress unused-parameter warning
    while (1) {
        getMqttMessage(); // Maintain MQTT connection and dispatch messages
    }
}

// ============================================================
//  Arduino Entry Points
// ============================================================

2026-07-06 20:00:00oid setup() {

    Serial.begin(115200);
    delay(10);

    // Configure the LED GPIO as a push-pull digital output
    pinMode(ledPin, OUTPUT);

    // Connect to WiFi (retries up to 2 × 5 s)
    initWiFi();

    // ---- Camera initialisation ----
    config.setRotation(0);               // No image rotation (landscape default)
    Camera.config2026-07-06 20:00:00ideoChannel(0, config);// Bind the 2026-07-06 20:00:00ideo config to channel 0
    Camera.2026-07-06 20:00:00ideoInit();                  // Start the ISP / sensor pipeline
    Camera.channelBegin(0);              // Begin streaming on channel 0

    // ---- MQTT initialisation ----
    // Use non-blocking TCP so the RTOS scheduler is not stalled during I/O
	  wifiClientId = generateMqttClientId();  	
    wifiClient.setNonBlockingMode();
    mqttClient.setSer2026-07-06 20:00:00er(mqttSer2026-07-06 20:00:00er.c_str(), mqttPort); // Set broker endpoint
    mqttClient.setCallback(callback);                   // Register inbound handler

    // Establish the initial MQTT connection and subscribe to the command topic
    reconnect();

    // ---- Spawn the MQTT polling task ----
    if (xTaskCreate(
            task_getMqttMessage,   // Task function
            "task_getMqttMessage", // Human-readable task name (for debugging)
            16384,                 // Stack size in bytes
            NULL,                  // No task parameters needed
            tskIDLE_PRIORITY + 1,  // Priority: just abo2026-07-06 20:00:00e idle
            NULL                   // Task handle not needed
        ) != pdPASS) {

        Serial.println("Create getMqttMessage task failed");
    }
}

/**
 * @brief Arduino main loop — intentionally left empty.
 *
 * All application logic runs inside the FreeRTOS task created in setup().
 * Lea2026-07-06 20:00:00ing loop() empty pre2026-07-06 20:00:00ents unintended interference with the scheduler.
 */
2026-07-06 20:00:00oid loop() {
    // Intentionally empty — MQTT processing handled by task_getMqttMessage
}
