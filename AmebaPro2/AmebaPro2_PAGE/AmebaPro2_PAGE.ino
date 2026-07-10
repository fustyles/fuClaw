/*
 * AmebaPro2 AP mode - Web page manager
 *
 * This sketch turns the AmebaPro2 board into a small web server that:
 *   1) Hosts its own WiFi Access Point (AP mode) so a phone/PC can connect
 *      to it directly without any existing network.
 *   2) Simultaneously tries to join an existing WiFi network (station mode),
 *      so the same board can also be reached over the local LAN/router.
 *   3) Serves a single HTML "chat" style control page and accepts simple
 *      "/on" and "/off" commands to toggle an LED, over both GET and POST.
 *
------------------------------------------------------------
Author
------------------------------------------------------------
Author:
  ChungYi Fu (Kaohsiung, Taiwan)
  https://www.facebook.com/francefu

Repository:
  https://github.com/fustyles/fuClaw

Build Date: 2026-07-10 19:00:00
------------------------------------------------------------
*/

// ---------------------------------------------------------
// WiFi credentials used to join an existing (station) network.
// If wifiSsid is left as an empty string (""), the board will
// skip station-mode connection entirely and only run as an AP.
// ---------------------------------------------------------
String wifiSsid = "xxxxxxxxxx";      // SSID of the WiFi network to connect to
String wifiPassword = "xxxxxxxxxx"; // Password for the WiFi network above

// ---------------------------------------------------------
// AP (Access Point) credentials - the device also hosts its own network
// so a client can connect directly to the board without a router.
// Web UI can be reached at http://192.168.1.1:81
// (Note: the HTTP server itself is actually started on port 80 below;
//  81 in the comment refers to the AP's default management port/UI.)
// ---------------------------------------------------------
String apSsid = "fuclaw";       // Name of the WiFi network this board broadcasts
String apPassword = "12345678"; // Password required to join the AP above

// Copy page content to a local buffer.
// request can generate new content independently.
#include "index_chat_html.h"   // Contains the HTML page content (INDEX_CHAT_HTML)
                                // This external header stores the full HTML/CSS/JS
                                // for the front-end page as a single string constant.

// Indicator LED output pin
// This pin is toggled HIGH/LOW to visually confirm "/on" and "/off" commands,
// and is also blinked 3 times in setup() if station WiFi connects successfully.
int ledPin = 24;    // green led (AMB82-mini: 24, HUB 8735 Ultra: 25)

#include <WiFi.h>   // Ameba WiFi library: provides WiFi.begin(), WiFi.apbegin(),
                    // WiFiServer, WiFiClient, and related networking classes

char channel_ap[] = "2";   // WiFi channel used for the Access Point
WiFiServer server(80);     // HTTP server instance listening on TCP port 80

#define CONFIG_INIC_IPC_HIGH_TP   // Enable high-throughput inter-IC communication config
                                  // (board-specific macro that tunes the internal
                                  // WiFi-chip <-> MCU communication link for higher
                                  // throughput; required by some AmebaPro2 WiFi builds)

// ---------------------------------------------------------
// Decode a URL-encoded string (e.g. "%20" -> " ", "+" -> " ")
// Used to decode the body/path of incoming HTTP requests, since
// browsers/clients percent-encode reserved and non-ASCII characters
// before sending them in a URL or form body.
// ---------------------------------------------------------
String urldecode(const String& input) {
    String result = "";
    result.reserve(input.length());  // Pre-allocate to avoid repeated reallocation

    for (int i = 0; i < (int)input.length(); i++) {
        if (input[i] == '%' && i + 2 < (int)input.length()) {
            // '%' introduces a percent-escape sequence: the next two
            // characters are hex digits representing one byte value.
            // Convert the next two hex digits following '%' into a byte.
            char hex[3] = { input[i+1], input[i+2], '\0' };
            uint8_t val = (uint8_t)strtol(hex, nullptr, 16); // Base-16 parse of the hex pair
            result.concat((char)val);  // Append the decoded raw byte/character
            i += 2; // Skip the two hex digits we just consumed
        } else if (input[i] == '+') {
            // '+' in URL/form encoding (application/x-www-form-urlencoded)
            // represents a literal space character.
            result += ' ';
        } else {
            // Regular character with no special encoding meaning, copy as-is
            result += input[i];
        }
    }
    return result;
}

// ---------------------------------------------------------
// Decide what response to send back based on the parsed request.
// getHead: the raw first request line, e.g. "GET / HTTP/1.1"
// getBody: for GET requests this is the decoded path (e.g. " /on ");
//          for POST requests this is the decoded request body content.
// Returns the full string that should be sent as the HTTP response body.
// ---------------------------------------------------------
String routeRequest(const String &getHead, const String &getBody) {

  String head = getHead;
  String body = urldecode(getBody);  // Decode percent-encoding / '+' before comparing

  // Debug logging: print what was received so requests can be traced
  // over the Serial monitor during development.
  Serial.println("Head = " + head); 
  Serial.println("Body = " + body);

  // Route the request based on its head/body content.
  // Order matters: root path is checked first, then specific commands,
  // then a generic fallback for anything unrecognized.
  if (head.startsWith("GET / ") || head.startsWith("POST / "))
    return String(INDEX_CHAT_HTML);   // Root path: serve the main chat HTML page
  else if (body == "/on") {
    digitalWrite(ledPin, HIGH);        // Command: turn the LED on
    return "The light is turned on";   // Simple confirmation text sent back to client
  }
  else if (body == "/off") {
    digitalWrite(ledPin, LOW);         // Command: turn the LED off
    return "The light is turned off";  // Simple confirmation text sent back to client
  }
  else
    return "Please enter /on or /off"; // Fallback message for unrecognized requests
}

// ---------------------------------------------------------
// Build and send the HTTP response back to the client.
// getHead: the first request line (e.g. "GET / HTTP/1.1")
// getBody: the request path (for GET) or POST body content
// ---------------------------------------------------------
void handleRequest(WiFiClient &client, const String &getHead, const String &getBody) {

  // Local buffer holding the actual response body for this request,
  // computed by routing the request to the correct handler above.
  String responseBody = routeRequest(getHead, getBody);

  // Send standard HTTP response headers.
  // Each client.println() call writes one header line followed by CRLF.
  client.println("HTTP/1.1 200 OK");                                  // Status line: always respond with 200 OK
  client.println("Content-Type: text/html; charset=utf-8");           // Body is HTML/text, UTF-8 encoded
  client.println("Content-Length: " + String(responseBody.length())); // Exact byte length of the body, so the client knows when to stop reading
  client.println("Access-Control-Allow-Origin: *"); // Allow cross-origin requests (CORS), so the page can be fetched from other origins/tools
  client.println("Cache-Control: no-cache");         // Prevent browsers from caching this dynamic response
  client.println("Connection: close");               // Tell the client the connection will be closed after this response
  client.println();  // Blank line separates headers from body, per HTTP spec

  // Send the response body in small chunks to avoid overloading the TCP buffer.
  // Writing everything in one huge call can overflow the underlying socket
  // buffer on constrained embedded hardware, so we stream it out in pieces.
  const char* ptr = responseBody.c_str();  // Raw pointer to the response bytes
  int total  = responseBody.length();      // Total number of bytes to send
  int sent   = 0;                          // How many bytes have been sent so far

  while (sent < total) {
    // Send at most 512 bytes per write call
    int chunk   = (total - sent) > 512 ? 512 : (total - sent);
    int written = client.write((const uint8_t*)(ptr + sent), chunk);
    if (written > 0)
      sent += written;      // Advance by however many bytes were actually accepted
    else
      delay(5);  // Nothing was written (buffer likely full); wait briefly and retry
  }

  client.flush();  // Ensure all buffered data is actually pushed out over the socket
  
}

// ---------------------------------------------------------
// Background task: continuously waits for and handles
// incoming HTTP client connections.
// This runs forever as its own FreeRTOS task so the main loop()
// stays free; each iteration accepts one client and fully parses
// one HTTP request (headers, and body if present) before moving on.
// ---------------------------------------------------------
void task_getRequest(void *param) {
  (void)param;  // Unused parameter (required by the FreeRTOS task function signature)
  
  while (1) {

    WiFiClient client = server.available();  // Check for a new client connection (non-blocking-ish poll)

    if (client) {

      // Keep processing while the client is connected or still has
      // buffered data available to read (covers the tail end of a request).
      while (client.connected() || client.available()) {

        String currentLine = "";   // Buffer for the current line being read (headers, request line, etc.)
        String getHead = "";       // Stores the request line (e.g. "GET /on HTTP/1.1" or "POST / HTTP/1.1")
        String getBody = "";       // Stores the request path (GET) or accumulated body content (POST)
        int bodyLength = 0;        // Expected length of the POST body, parsed from the Content-Length header
        bool bodyStart = false;    // Flag: true once headers are finished and body reading begins
        int waitTime = 5000;       // Max time (ms) to wait for more data before giving up on this request
        unsigned long startTime = millis();  // Timestamp used to track the read timeout

        // Read incoming data until the request is fully parsed or the timeout is reached.
        // The timeout is refreshed every time new data actually arrives (see below),
        // so this only aborts if the client goes quiet for `waitTime` ms.
        while ((startTime + waitTime) > millis()) {
          vTaskDelay(100 / portTICK_PERIOD_MS);  // Yield to other tasks briefly between polls
          
          while (client.available()) {
            char c = client.read();  // Read one byte/character at a time from the socket

            // If we've already finished the headers and are now expecting
            // a POST body, accumulate body characters here instead of
            // treating them as header lines.
            if (bodyStart == true && bodyLength > 0) {
              getBody += c;
              if (getBody.length() == bodyLength) {
                // Full POST body received (matches Content-Length); handle the request now.
                handleRequest(client, getHead, getBody);
                startTime = 0;  // Force the outer wait loop to exit immediately
                break;
              }
            }
              
            if (c == '\n')  {
              // End of a line reached (assuming '\r' was already stripped below).
              // Make a lowercase copy for case-insensitive header matching,
              // while keeping `currentLine` itself untouched for exact comparisons.
              String lowerLine = currentLine;
              lowerLine.toLowerCase();
              
              if (currentLine.startsWith("GET /")) {
                // GET request: extract the path between the first and last space.
                // e.g. "GET /on HTTP/1.1" -> " /on "
                getHead = currentLine;
                getBody = currentLine.substring(currentLine.indexOf(" "), currentLine.lastIndexOf(" "));
                handleRequest(client, getHead, getBody);  // GET requests have no body, so handle immediately
                startTime = 0;  // Done handling this request, stop waiting
                break;                  
              }
              else if (currentLine.startsWith("POST /")) {
                // Remember the POST request line; the actual body will
                // follow later, after all the headers have been read.
                getHead = currentLine;
              }                
              else if (lowerLine.startsWith("content-length:")) {
                // Parse the Content-Length header (case-insensitively) to
                // know exactly how many body bytes to expect afterward.
                lowerLine.replace("content-length:", "");
                lowerLine.trim();
                bodyLength = lowerLine.toInt();
              }

              // An empty line marks the end of the HTTP headers section.
              // If this was a POST request with a non-zero body length,
              // switch into body-reading mode starting from the next byte.
              if (currentLine.length()==0 && getHead.startsWith("POST /") && bodyLength > 0 ) {
                bodyStart = true; 
              }                            

              currentLine = "";  // Reset line buffer for the next line
            }
            else if (c != '\r') {
              // Accumulate characters into the current line, ignoring
              // carriage returns ('\r') so lines end up newline-only.
              currentLine += c;
            }
  
            startTime = millis();  // Reset the timeout since we just received data
          }
        }           
      }
      
      client.stop();  // Close the connection once the request/response cycle is done

    } else {
      vTaskDelay(5);  // No client yet; yield briefly before checking again
    } 

    vTaskDelay(5);  // Yield to other tasks each loop iteration, regardless of branch taken
  }
}

// ---------------------------------------------------------
// Initialize WiFi: start the Access Point and also try to
// join an existing WiFi network (station mode) concurrently.
// Both modes run at the same time thanks to enableConcurrent(),
// so the board is reachable both as its own hotspot and as a
// regular client on the existing network.
// ---------------------------------------------------------
void initWiFi() {
  
  WiFi.enableConcurrent();  // Allow AP mode and station mode to run at the same time
  WiFi.apbegin((char*)apSsid.c_str(), (char*)apPassword.c_str(), channel_ap, 0);  // Start the Access Point with the configured SSID/password/channel
    
  for (int i=0;i<2;i++) {  // Retry connecting to the station network up to 2 times

    if (wifiSsid=="")
      break;  // No station SSID configured, skip connecting entirely (AP-only mode)

    WiFi.begin((char*)wifiSsid.c_str(), (char*)wifiPassword.c_str());  // Attempt to join the configured network
    delay(1000);  // Give the WiFi stack a moment to start the connection attempt

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifiSsid);

    unsigned long StartTime=millis();  // Mark when this connection attempt began

    // Wait up to 5 seconds for the connection to succeed before
    // giving up on this attempt (the outer for-loop may retry once more).
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);

      if ((StartTime+5000) < millis())
        break;  // Timed out waiting for this attempt; exit and let the loop retry or finish
    }
    
    if (WiFi.status() == WL_CONNECTED)
      break;    
  }
  
}

// ---------------------------------------------------------
// Convert an IPAddress object into a human-readable dotted string
// e.g. IPAddress(192,168,1,1) -> "192.168.1.1"
// ---------------------------------------------------------
String Ip2String(IPAddress ip) {
  return String(ip[0])+String(".")+String(ip[1])+String(".")+String(ip[2])+String(".")+String(ip[3]);
}

void setup() {
  Serial.begin(115200);  // Start serial communication for debug logging

  // Indicator LED  
  pinMode(ledPin, OUTPUT);  // Configure the LED pin as a digital output

  initWiFi();   // Bring up AP mode and (optionally) join a station network

  server.begin();   // Start listening for HTTP connections on port 80

  // Create a background FreeRTOS task to handle incoming HTTP requests,
  // so the main loop() can remain idle/free for other work.
  if (xTaskCreate(
        task_getRequest,        // Task function to run
        (const char *)"task_getRequest",  // Human-readable task name (for debugging)
        16384,                  // Stack size (bytes) allocated for this task
        NULL,                   // No parameter passed to the task
        tskIDLE_PRIORITY + 1,   // Task priority (slightly above idle)
        NULL                    // Task handle not needed, so not stored
      )!= pdPASS) {

    Serial.println("Create task_task_getRequest failed");  // Log failure if task creation didn't succeed
  }        

  // Print connection info for convenience when debugging over Serial.
  Serial.println("\n");   
  Serial.println("Main page\nhttp://192.168.1.1");
  Serial.println("AP ssid : " + apSsid);
  Serial.println("AP password : " + apPassword);
  Serial.println("\n");  

  // If successfully connected to the station WiFi network, blink the LED
  // 3 times as a visual "connected" indicator, and print the assigned
  // IP address so the board can also be reached via the local network.
  if (WiFi.status() == WL_CONNECTED) {
    for (int i=0 ; i<3 ; i++) {
      digitalWrite(ledPin, 1);  // LED on
      delay(300);
      digitalWrite(ledPin, 0);  // LED off
      delay(300);      
    }
    
    Serial.println("Main page\nhttp://" + Ip2String(WiFi.localIP()));
    Serial.println("\n");   
  }   
  
}

// Main loop (unused - all work happens in the background task_getRequest task)
void loop() {
}
