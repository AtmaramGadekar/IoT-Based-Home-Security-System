# IoT-Based Home Security System

An ESP32-based home security prototype that uses a PIR motion sensor to detect movement, activates an LED indicator, and sends a Telegram notification over Wi-Fi.

## Features

- PIR-based motion detection
- LED visual alert
- Telegram bot notifications
- Wi-Fi connection at startup
- Wi-Fi reconnection attempts if the connection drops
- Motion event re-arming when the PIR output returns LOW
- Alert cooldown to limit repeated Telegram messages

## Hardware

| Component | Connection |
|---|---|
| ESP32 development board | Main controller |
| PIR motion sensor | OUT → GPIO 2 |
| LED | GPIO 13 |
| PIR power | Connect VCC and GND according to the sensor module's specifications |
| LED ground | Use an appropriate current-limiting resistor and GND |

Check the specifications of your exact PIR module and LED before wiring. Do not connect a bare LED directly to a GPIO pin without a current-limiting resistor.

## Software and libraries

- Arduino IDE
- ESP32 Arduino core
- `WiFi.h`
- `WiFiClientSecure.h`
- `HTTPClient.h`

The three headers above are provided by the ESP32 Arduino core; install the ESP32 board support package in Arduino IDE.

## Configuration

Open `IoT.ino` and replace the placeholder values with your own credentials:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* botToken = "YOUR_TELEGRAM_BOT_TOKEN";
const char* chatID = "YOUR_TELEGRAM_CHAT_ID";
```

Keep real credentials private. Never commit your actual Wi-Fi password or Telegram bot token to a public repository. If credentials have ever been exposed publicly, change the Wi-Fi password or revoke/regenerate the bot token as appropriate.

## Current timing settings

The uploaded sketch currently defines:

- PIR stabilization delay after startup: 30 seconds
- LED on-duration: 30 seconds (`LED_DURATION = 30000`)
- Telegram alert cooldown: 60 seconds (`ALERT_COOLDOWN = 60000`)

The alert cooldown timestamp is updated after a successful Telegram request. Motion detection is re-armed when the PIR output goes LOW.

## Uploading the sketch

1. Install Arduino IDE and ESP32 board support.
2. Open `IoT.ino`.
3. Select the correct ESP32 board and serial port.
4. Enter your own Wi-Fi and Telegram credentials locally.
5. Upload the sketch.
6. Open Serial Monitor at `115200` baud to view connection and motion messages.

## Repository contents

- `IoT.ino` — original Arduino sketch supplied for this repository
- `Documentation/Hardware-Connections.md` — basic wiring notes
- `Images/` — add your own project and Telegram screenshots
- `Circuit_Diagram/` — add your circuit diagram

## Limitations and safety notes

- This is a prototype, not a certified alarm system. Do not rely on it as the only protection for a home.
- The sketch uses `client.setInsecure()`, which disables TLS certificate verification. This simplifies HTTPS connectivity but does not provide certificate authentication; consider proper certificate validation before using the system in a security-critical deployment.
- Telegram alerts require working Wi-Fi and access to Telegram's API.
- PIR sensors can require stabilization after power-up and may produce false or missed detections depending on placement and environment.

## Future improvements

- Door/window magnetic reed switch
- Buzzer alarm
- Remote arm/disarm control
- Event logging with timestamps
- Web dashboard and database-backed alert history
- Camera-based image capture

## Author

**Atmaram Gadekar**  
B.Tech — Information Technology
