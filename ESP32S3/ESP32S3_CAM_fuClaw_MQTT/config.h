
// WiFi credentials
String wifiSsid = "xxxxxxxxxx";
String wifiPassword = "xxxxxxxxxx";

// AP credentials http://192.168.1.1:81
String apSsid = "fuclaw";
String apPassword = "12345678";

// MQTT broker settings
String mqttServer   = "broker.emqx.io";                          // Broker hostname or IP
uint16_t mqttPort   = 1883;                                  // Standard MQTT port (unencrypted)
String mqttUser     = "";                                    // Leave empty if no auth required
String mqttPassword = "";                                    // Leave empty if no auth required

// MQTT topic strings
//   Subscribe topic : broker pushes incoming commands here
//   Publish topics  : device pushes text replies and camera images here
String mqttSubscribeTextTopic      = "xxxxxxxxxx/subscribe";       // Inbound command topic
String mqttPublishTextTopic        = "xxxxxxxxxx/publish";         // Outbound text reply topic
String mqttPublishImageTopic       = "xxxxxxxxxx/publishimage";    // Outbound JPEG topic

// Stores the MQTT Client ID for this device (generated from MAC address to ensure uniqueness)
String wifiClientId = "";

// Gemini API configuration
String geminiApiKey = "xxxxxxxxxx";
String geminiModel = "gemini-3-flash-preview";

int geminiMaxOutputTokens = 8192;  // If the AI ​​is unable to transmit complete data, please increase the value.
float geminiTemperature = 1.0;

String timeZone = "Asia/Taipei";

String deviceName = "fuClaw";

String systemCommand =
  "Built-in commands:\n"
  "/help command list\n"
  "/still capture and send a camera image\n"
  "/syncrtc update the hardware RTC\n" 
  "/getrtc get the hardware RTC current time\n"
  "/getSchedule Get all scheduled tasks\n"
  "/getUnfinishedSchedule Get unfinished scheduled tasks\n"
  "/getMemory show system memory usage\n"
  "/getLog show tool execution history\n"
  "Hardware control supported:\n"
  "- Digital output (0 or 1)\n"
  "- Analog output (0–255)\n"
  "- Digital input reading\n"
  "- Analog input reading\n\n"
  "System Status:\n<memory>"
  "\n\nYou can chat with Gemini using natural language.\n"
  "The system supports real-time search and vision-based analysis.\n\n"
  "Documentation:\n"
  "https://github.com/fustyles/fuClaw";
