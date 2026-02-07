/*
  ESP8266 Remote Wake (IoHub)

  What it does:
  - Connects to Wi‑Fi
  - Polls a Google Apps Script HTTPS endpoint
  - Reads "cmd" from the JSON response
  - When cmd changes, it toggles a relay for a short pulse (remote power button)

  Hardware (default pins):
  - RELAY_PIN: GPIO14 (D5 on many NodeMCU boards)
  - STATUS_LED_PIN: GPIO12 (D6 on many NodeMCU boards)

  Setup:
  1) Copy config.example.h -> config.h, then fill in your Wi‑Fi and Google Script info.
  2) Upload to an ESP8266 board.
*/

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "config.h"

// Pins
const uint8_t RELAY_PIN      = 14;  // GPIO14
const uint8_t STATUS_LED_PIN = 12;  // GPIO12

// HTTPS
const int HTTPS_PORT = 443;
HTTPSRedirect* client = nullptr;

// State
String last_cmd = "";
unsigned long last_blink_ms = 0;

static void blinkWhileConnecting() {
  const unsigned long now = millis();
  if (now - last_blink_ms >= 500) {
    last_blink_ms = now;
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
}

static String extractCmd(const String& json) {
  // Expect JSON like: {"cmd":"1","ts":"..."}
  const String key = "\"cmd\":\"";
  int start = json.indexOf(key);
  if (start < 0) return "";
  start += key.length();
  int end = json.indexOf("\"", start);
  if (end < 0) return "";
  return json.substring(start, end);
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);      // relay off (active-low)
  digitalWrite(STATUS_LED_PIN, HIGH); // led on

  Serial.begin(115200);
  delay(50);

  Serial.println();
  Serial.print("Connecting to Wi‑Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    blinkWhileConnecting();
    delay(10);
  }
  digitalWrite(STATUS_LED_PIN, HIGH);
  Serial.print("Wi‑Fi connected, IP: ");
  Serial.println(WiFi.localIP());

  client = new HTTPSRedirect(HTTPS_PORT);
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");

  Serial.print("Connecting to host: ");
  Serial.println(GSCRIPT_HOST);
  client->connect(GSCRIPT_HOST, HTTPS_PORT);
}

void loop() {
  // Reconnect Wi‑Fi if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi‑Fi disconnected. Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      blinkWhileConnecting();
      delay(10);
    }
    digitalWrite(STATUS_LED_PIN, HIGH);
  }

  // Ensure HTTPS client is connected
  if (!client->connected()) {
    client->connect(GSCRIPT_HOST, HTTPS_PORT);
  }

  // Build endpoint path
  const String url = String("/macros/s/") + GSCRIPT_ID + "/exec";

  if (client->GET(url, GSCRIPT_HOST)) {
    digitalWrite(STATUS_LED_PIN, HIGH);

    const String body = client->getResponseBody();
    const String cmd = extractCmd(body);

    if (cmd.length() > 0) {
      Serial.print("cmd=");
      Serial.println(cmd);

      if (last_cmd.length() == 0) {
        last_cmd = cmd; // first read: just initialize
      } else if (cmd != last_cmd) {
        Serial.println("cmd changed -> trigger relay pulse");
        last_cmd = cmd;

        // Relay pulse (simulate pressing power button)
        digitalWrite(RELAY_PIN, LOW);
        delay(RELAY_PULSE_MS);
        digitalWrite(RELAY_PIN, HIGH);
      }
    } else {
      Serial.println("Failed to parse cmd from response.");
    }
  } else {
    Serial.println("HTTPS GET failed.");
    digitalWrite(STATUS_LED_PIN, LOW);
  }

  delay(POLL_INTERVAL_MS);
}
