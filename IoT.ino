
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ========== Wi-Fi credentials ==========
const char* ssid = "//your_device_name";
const char* password = "//your_wifi_password";

// ========== Telegram credentials ==========
// Use your NEW token from BotFather
const char* botToken = "//your_bot_token";
const char* chatID = "//your_chat_id";

// ========== Hardware pins ==========
const int PIR_PIN = 2;
const int LED_PIN = 13;

// ========== Timing ==========
const unsigned long LED_DURATION = 30000;    // 30 seconds
const unsigned long ALERT_COOLDOWN = 60000; // 1 minute

unsigned long ledStartTime = 0;
unsigned long lastAlertTime = 0;

bool ledIsOn = false;
bool motionDetected = false;

// ============================================
// URL encode text for Telegram
// ============================================
String urlEncode(String text) {
  String encoded = "";

  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);

    if (isalnum((unsigned char)c) ||
        c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else {
      char hex[4];
      snprintf(hex, sizeof(hex), "%%%02X",
               (unsigned char)c);
      encoded += hex;
    }
  }

  return encoded;
}

// ============================================
// Send Telegram alert
// ============================================
bool sendTelegramMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Alert not sent.");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;

  String url = "https://api.telegram.org/bot";
  url += botToken;
  url += "/sendMessage";

  if (!https.begin(client, url)) {
    Serial.println("Could not start HTTPS connection.");
    return false;
  }

  https.addHeader("Content-Type",
                  "application/x-www-form-urlencoded");

  String postData = "chat_id=" + String(chatID);
  postData += "&text=" + urlEncode(message);

  Serial.println("Sending Telegram alert...");

  int httpCode = https.POST(postData);
  String response = https.getString();

  Serial.print("HTTP code: ");
  Serial.println(httpCode);
  Serial.println(response);

  https.end();

  if (httpCode == 200) {
    Serial.println("Telegram alert sent successfully!");
    return true;
  }

  Serial.println("Telegram alert failed.");
  return false;
}

// ============================================
// Setup
// ============================================
void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Serial.println();
  Serial.println("ESP32 Home Security System");
  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(ssid, password);

  // Avoid waiting forever if Wi-Fi is unavailable
  unsigned long wifiStart = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - wifiStart < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi not connected. Will retry later.");
  }

  Serial.println("Allow the PIR sensor to stabilize.");
  delay(30000);

  Serial.println("System ready. Monitoring for motion...");
}

// ============================================
// Main loop
// ============================================
void loop() {
  // Reconnect to Wi-Fi when needed
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }

  int pirValue = digitalRead(PIR_PIN);

  // Detect the beginning of a motion event
  if (pirValue == HIGH && !motionDetected) {
    motionDetected = true;

    Serial.println("MOTION DETECTED!");

    // Start the 3-second LED timer
    digitalWrite(LED_PIN, HIGH);
    ledIsOn = true;
    ledStartTime = millis();

    // Send one alert per cooldown period
    if (lastAlertTime == 0 ||
        millis() - lastAlertTime >= ALERT_COOLDOWN) {

      String message =
        "SECURITY ALERT!\nMotion detected at your home.\n"
        "Please check your surroundings.";

      if (sendTelegramMessage(message)) {
        lastAlertTime = millis();
      }
    } else {
      Serial.println("Alert skipped: cooldown active.");
    }
  }

  // Re-arm detection when the PIR output goes LOW
  if (pirValue == LOW && motionDetected) {
    motionDetected = false;
    Serial.println("Motion event ended.");
  }

  // Turn LED OFF after 3 seconds, even if motion continues
  if (ledIsOn &&
      millis() - ledStartTime >= LED_DURATION) {

    digitalWrite(LED_PIN, LOW);
    ledIsOn = false;

    Serial.println("LED OFF: 3 seconds completed.");
  }

  delay(50);
}
