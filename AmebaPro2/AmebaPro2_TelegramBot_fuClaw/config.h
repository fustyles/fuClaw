
// WiFi credentials
String wifiSsid = "xxxxxxxxxx";
String wifiPassword = "xxxxxxxxxx";

// AP credentials http://192.168.1.1:81
String apSsid = "fuclaw";
String apPassword = "12345678";

// Telegram bot configuration
String telegrambotToken = "xxxxxxxxxx";
String telegrambotChatId = "xxxxxxxxxx";

String systemCommand =
  "Built-in commands:\n"
  "/help command list\n"
  "/still capture and send a camera image\n"
  "/syncrtc update the hardware RTC\n" 
  "/getrtc get the hardware RTC current time\n"                           
  "/memory show system memory usage\n"
  "/log show tool execution history\n"
  "/reset start a new conversation\n\n"
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

String telegrambotKeyboard = "{\"keyboard\":[[{\"text\":\"/help\"},{\"text\":\"/still\"},{\"text\":\"/syncrtc\"},{\"text\":\"/getrtc\"}],[{\"text\":\"/memory\"},{\"text\":\"/log\"},{\"text\":\"/reset\"}]],\"one_time_keyboard\":false}";

// Gemini API configuration
String geminiApiKey = "xxxxxxxxxx";

String geminiModel = "gemini-3-flash-preview";
int geminiMaxOutputTokens = 8192;  // If the AI ​​is unable to transmit complete data, please increase the value.
float geminiTemperature = 1.0;

String timeZone = "Taiwan";
