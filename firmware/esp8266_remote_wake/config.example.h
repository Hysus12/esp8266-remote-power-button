#pragma once

// ===== Wi‑Fi =====
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

// ===== Google Apps Script =====
// Host is almost always script.google.com
#define GSCRIPT_HOST    "script.google.com"

// The part after /macros/s/<...>/exec
// Example: https://script.google.com/macros/s/AKfy.../exec
#define GSCRIPT_ID      "YOUR_GSCRIPT_DEPLOYMENT_ID"

// ===== Behavior =====
#define POLL_INTERVAL_MS  2000   // polling interval
#define RELAY_PULSE_MS    2000   // how long to hold the relay (power button) pressed
