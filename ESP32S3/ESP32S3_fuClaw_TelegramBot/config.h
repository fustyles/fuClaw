
// WiFi credentials
String wifiSsid = "xxxxxx";
String wifiPassword = "xxxxxx";

// AP credentials http://192.168.1.1:81
String apSsid = "fuclaw";
String apPassword = "12345678";

// Telegram bot configuration
String telegrambotToken = "xxxxxx";
String telegrambotChatId = "xxxxxx";

String systemCommand =
  "Built-in commands:\n"
  "/help command list\n"
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
  "\n\nYou can chat with AI using natural language.\n"
  "The system supports real-time search.\n\n"
  "Documentation:\n"
  "https://github.com/fustyles/fuClaw";

String telegrambotKeyboard = "{\"keyboard\":[[{\"text\":\"/help\"},{\"text\":\"/getLog\"}],[{\"text\":\"/getMemory\"},{\"text\":\"/syncrtc\"},{\"text\":\"/getrtc\"}],[{\"text\":\"/getSchedule\"},{\"text\":\"/getUnfinishedSchedule\"}]],\"resize_keyboard\":true,\"one_time_keyboard\":false}";

// API configuration
String llmType = "xxxxxx";    // gemini, openai, grok
String llmKey = "xxxxxx";
String llmModel = "xxxxxx";    // [gemini] gemini-3-flash-preview ,[openai] gpt-5.6 ,[grok] grok-4.5

int llmMaxOutputTokens = 8192;  // If the AI ​​is unable to transmit complete data, please increase the value.
float llmTemperature = 1.0;

int scheduleTimeout = 5;    // minutes

String timeZone = "Asia/Taipei";

String deviceName = "fuClaw";

bool heartbeat = false;