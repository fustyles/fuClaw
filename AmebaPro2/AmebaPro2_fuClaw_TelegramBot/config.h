
// WiFi credentials
String wifiSsid = "xxxxxxxxxx";
String wifiPassword = "xxxxxxxxxx";

// AP credentials http://192.168.1.1:81
String apSsid = "fuclaw";
String apPassword = "12345678";

// Telegram bot configuration
String telegrambotToken = "xxxxxxxxxx";
String telegrambotChatId = "xxxxxxxxxx";

// Gemini API configuration
String geminiApiKey = "xxxxxxxxxx";
String geminiModel = "gemini-3-flash-preview";

int geminiMaxOutputTokens = 8192;  // If the AI ​​is unable to transmit complete data, please increase the value.
float geminiTemperature = 1.0;

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

String telegrambotKeyboard = "{\"keyboard\":[[{\"text\":\"/help\"},{\"text\":\"/still\"},{\"text\":\"/getLog\"}],[{\"text\":\"/getMemory\"},{\"text\":\"/syncrtc\"},{\"text\":\"/getrtc\"}],[{\"text\":\"/getSchedule\"},{\"text\":\"/getUnfinishedSchedule\"}]],\"resize_keyboard\":true,\"one_time_keyboard\":false}";

String timeZone = "Asia/Taipei";
