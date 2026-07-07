/*
 * ESP32 AP mode - Web page manager
------------------------------------------------------------
Author
------------------------------------------------------------
Author:
  ChungYi Fu (Kaohsiung, Taiwan)
  https://www.facebook.com/francefu

Repository:
  https://github.com/fustyles/fuClaw

Build Date: 2026-07-06 22:00
------------------------------------------------------------
*/

// ---------------------------------------------------------
// WiFi credentials used to join an existing (station) network
// ---------------------------------------------------------
String wifiSsid = "xxxxxxxxxx";      // SSID of the WiFi network to connect to
String wifiPassword = "xxxxxxxxxx"; // Password for the WiFi network above

// ---------------------------------------------------------
// AP (Access Point) credentials - device also hosts its own network
// Web UI can be reached at http://192.168.1.1:81
// ---------------------------------------------------------
String apSsid = "fuclaw";
String apPassword = "12345678";

// Copy page content to a local buffer.
// request can generate new content independently.
#include "index_chat_html.h"   // Contains the HTML page content (INDEX_CHAT_HTML)

#include <WiFi.h>
#include "FreeRTOS.h"
#include "task.h"

WiFiServer server(80);     // HTTP server listening on port 80

// ---------------------------------------------------------
// Decode a URL-encoded string (e.g. "%20" -> " ", "+" -> " ")
// Used to decode the body/path of incoming HTTP requests
// ---------------------------------------------------------
String urldecode(const String& input) {
    String result = "";
    result.reserve(input.length());
    for (int i = 0; i < (int)input.length(); i++) {
        if (input[i] == '%' && i + 2 < (int)input.length()) {
            // Convert the next two hex digits following '%' into a byte
            char hex[3] = { input[i+1], input[i+2], '\0' };
            uint8_t val = (uint8_t)strtol(hex, nullptr, 16);
            result.concat((char)val);
            i += 2; // Skip the two hex digits we just consumed
        } else if (input[i] == '+') {
            // '+' in URL encoding represents a space
            result += ' ';
        } else {
            // Regular character, copy as-is
            result += input[i];
        }
    }
    return result;
}

// ---------------------------------------------------------
// Build and send the HTTP response back to the client
// getHead: the first request line (e.g. "GET / HTTP/1.1")
// getBody: the request path (for GET) or POST body content
// ---------------------------------------------------------
void handleRequest(WiFiClient &client, const String &getHead, String getBody) {

  getBody = urldecode(getBody);  // Decode any URL-encoded characters in the body/path

  Serial.println("getHead = " + getHead); 
  Serial.println("getBody = " + getBody);

  String mainPageHTML = "";   // Local buffer holding the actual response body for this request

  // Route the request based on its head/body content
  if (getHead.startsWith("GET / ") || getHead.startsWith("POST / "))
    mainPageHTML = String(INDEX_CHAT_HTML);   // Serve the main chat HTML page
  else if (getBody == "/on") {
    digitalWrite(LED_BUILTIN, HIGH);
    mainPageHTML = "The light is turned on";  // Simple command: turn light on
  }
  else if (getBody == "/off") {
    digitalWrite(LED_BUILTIN, LOW);
    mainPageHTML = "The light is turned off"; // Simple command: turn light off
  }
  else
    mainPageHTML = "Please enter /on or /off"; // Fallback message for unrecognized requests

  // Send standard HTTP response headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Content-Length: " + String(mainPageHTML.length()));
  client.println("Access-Control-Allow-Origin: *"); // Allow cross-origin requests
  client.println("Cache-Control: no-cache");
  client.println("Connection: close");
  client.println();  // Blank line separates headers from body

  // Send the response body in small chunks to avoid overloading the TCP buffer
  const char* ptr = mainPageHTML.c_str();
  int total  = mainPageHTML.length();
  int sent   = 0;
  while (sent < total) {
    int chunk   = (total - sent) > 512 ? 512 : (total - sent);
    int written = client.write((const uint8_t*)(ptr + sent), chunk);
    if (written > 0) sent += written;
    else delay(5);  // Wait briefly and retry if nothing was written
  }
  client.flush();
  
}

// ---------------------------------------------------------
// Background task: continuously waits for and handles
// incoming HTTP client connections
// ---------------------------------------------------------
void task_getRequest(void *param) {
  (void)param;
  
  while (1) {

    WiFiClient client = server.available();  // Check for a new client connection

    if (client) {

      // Keep processing while the client is connected or has data available
      while (client.connected() || client.available()) {

        String currentLine = "";   // Buffer for the current line being read
        String getHead = "";       // Stores the request line (GET/POST + path)
        String getBody = "";       // Stores the request path (GET) or body content (POST)
        int bodyLength = 0;        // Expected length of the POST body (from Content-Length header)
        bool bodyStart = false;    // Flag: true once headers are finished and body reading begins
        int waitTime = 5000;       // Max time (ms) to wait for more data before giving up
        unsigned long startTime = millis();

        // Read incoming data until the request is fully parsed or the timeout is reached
        while ((startTime + waitTime) > millis()) {
          vTaskDelay(100 / portTICK_PERIOD_MS);
          
          while (client.available()) {
            char c = client.read();

            // If we're in the body-reading phase, accumulate body characters
            if (bodyStart == true && bodyLength > 0) {
              getBody += c;
              if (getBody.length() == bodyLength) {
                // Full POST body received; handle the request now
                handleRequest(client, getHead, getBody);
                startTime = 0;  // Force the outer wait loop to exit
                break;
              }
            }
              
            if (c == '\n')  {
              String lowerLine = currentLine;
              lowerLine.toLowerCase();
              
              if (currentLine.startsWith("GET /")) {
                // GET request: extract the path between the first and last space
                // e.g. "GET /on HTTP/1.1" -> " /on "
                getHead = currentLine;
                getBody = currentLine.substring(currentLine.indexOf(" "), currentLine.lastIndexOf(" "));
                handleRequest(client, getHead, getBody);
                startTime = 0;  // Done handling this request, stop waiting
                break;                  
              }
              else if (currentLine.startsWith("POST /")) {
                // Remember the POST request line; body will follow after headers
                getHead = currentLine;
              }                
              else if (lowerLine.startsWith("content-length:")) {
                // Parse the Content-Length header to know how many body bytes to expect
                lowerLine.replace("content-length:", "");
                lowerLine.trim();
                bodyLength = lowerLine.toInt();
              }

              // An empty line marks the end of headers.
              // If this was a POST request with a body, start reading the body next.
              if (currentLine.length()==0 && getHead.startsWith("POST /") && bodyLength > 0 ) {
                bodyStart = true; 
              }                            

              currentLine = "";  // Reset line buffer for the next line
            }
            else if (c != '\r') {
              // Accumulate characters into the current line (ignore carriage returns)
              currentLine += c;
            }
  
            startTime = millis();  // Reset the timeout since we just received data
          }
        }           
      }
      
      client.stop();  // Close the connection once done

    } else {
      vTaskDelay(5);  // No client yet; yield briefly before checking again
    } 

    vTaskDelay(5);  // Yield to other tasks each loop iteration
  }
}

// ---------------------------------------------------------
// Initialize WiFi: start the Access Point and also try to
// join an existing WiFi network (station mode) concurrently
// ---------------------------------------------------------
void initWiFi() {

  WiFi.mode(WIFI_AP_STA);
  
  WiFi.softAP(apSsid.c_str(), apPassword.c_str());

  for (int i=0;i<2;i++) {  // Retry connecting to the station network up to 2 times

    if (wifiSsid=="")
      break;  // No station SSID configured, skip connecting

    WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
    delay(1000);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifiSsid);

    unsigned long StartTime=millis();

    // Wait up to 5 seconds for the connection to succeed
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);

      if ((StartTime+5000) < millis())
        break;
    }

    Serial.println("\n");   
    Serial.println("Main page\nhttp://192.168.4.1");
    Serial.println("AP ssid : " + apSsid);
    Serial.println("AP password : " + apPassword);
    Serial.println("\n");  

    // If successfully connected to the station WiFi network, blink the LED
    // and print the assigned IP address for convenience
    if (WiFi.status() == WL_CONNECTED) {
      for (int i=0 ; i<3 ; i++) {
        digitalWrite(LED_BUILTIN, 1);
        delay(300);
        digitalWrite(LED_BUILTIN, 0);
        delay(300);      
      }
      
      Serial.println("Main page\nhttp://" + Ip2String(WiFi.localIP()));
      Serial.println("\n");   
    }  
  }
  
}

// ---------------------------------------------------------
// Convert an IPAddress object into a human-readable string
// e.g. 192.168.1.1
// ---------------------------------------------------------
String Ip2String(IPAddress ip) {
  return String(ip[0])+String(".")+String(ip[1])+String(".")+String(ip[2])+String(".")+String(ip[3]);
}

void setup() {
  Serial.begin(115200);

  // Indicator LED  
  pinMode(LED_BUILTIN, OUTPUT);

  initWiFi();   // Bring up AP mode and (optionally) join a station network

  server.begin();   // Start listening for HTTP connections on port 80

  // Create a background FreeRTOS task to handle incoming HTTP requests
  if (xTaskCreate(
        task_getRequest,
        (const char *)"task_getRequest",
        16384,                  // Stack size
        NULL,
        tskIDLE_PRIORITY + 1,   // Task priority
        NULL
      )!= pdPASS) {

    Serial.println("Create task_task_getRequest failed");
  }         
  
}

// Main loop (unused - all work happens in the background task)
void loop() {
}
