# esp8266-remote-power-button
Remote PC power-on **over the Internet** by emulating a **physical press of the PC power button** (Power SW) using an **ESP8266 + relay**, controlled from **Google Sheets** via **Google Apps Script**.

> This is **NOT Wake-on-LAN (WOL)**.  
> No BIOS WOL setup is required, because it electrically shorts the **Power SW** header (same as pressing the case power button).

---
## Demo Videos

### 1) IoT Server–based Remote Power-On Demo
[![IoT Server demo](https://img.youtube.com/vi/_lbdoF4VKnA/hqdefault.jpg)](https://youtube.com/shorts/_lbdoF4VKnA)

### 2) Google Sheets–based Remote Power-On Demo
[![Google Sheets demo](https://img.youtube.com/vi/qs9141ITmmg/hqdefault.jpg)](https://youtube.com/shorts/qs9141ITmmg)

## What it does

- ESP8266 connects to your Wi-Fi.
- It periodically polls a Google Apps Script (HTTPS).
- The script reads a value in Google Sheets (your control panel).
- When the value changes (or matches a command), ESP8266 triggers a relay:
  - Relay closes the Power SW circuit briefly
  - Your PC powers on exactly like a real button press

---

## Why this approach

Most “remote power on” guides ask for:
- Port forwarding / DDNS / VPN / NAS

This project does **not** require inbound access to your home network.
The ESP8266 only makes **outbound HTTPS requests** to Google.

Works behind NAT / CGNAT.

---

## Architecture
<img width="8192" height="1045" alt="URL Reachability Flow-2026-02-07-095009" src="https://github.com/user-attachments/assets/72a8bb6f-5d77-4015-959f-13d2851c937a" />

### Hardware

ESP8266 (NodeMCU / Wemos D1 mini)


1-channel relay module (prefer opto-isolated if possible)

2 wires to the motherboard Power SW header (in parallel with your case power button)

The Power SW header is a low-voltage momentary switch input on the motherboard.
Do NOT connect mains voltage. Do NOT modify the PSU wiring.

Setup (high level)
1) Google Sheets
Create a sheet that stores a control value, e.g.:

0 = idle

1 = trigger power press

You can also use a timestamp or a counter — anything that “changes” works.

2) Google Apps Script (Web App)
Paste server/google_apps_script/Code.gs into Apps Script.

Set your SPREADSHEET_ID (and sheet/tab name if needed).

Deploy as Web App (accessible via HTTPS URL).

Recommended: add a shared secret (simple auth), e.g. ?key=YOUR_SECRET.

3) ESP8266 firmware
Copy config.example.h to config.h

Fill in:

Wi-Fi SSID / password

Apps Script Web App URL (and optional secret key)

Flash the .ino to your ESP8266.

How the “trigger” works
Typical logic:

ESP8266 reads a value from the API (derived from Google Sheets).

If the value is different from last time, it triggers the relay pulse.

Pulse duration can be adjusted (e.g. 200–1000 ms).
Some boards may need longer (e.g. 1–2 seconds).

FAQ
Q: Is this Wake-on-LAN?
A: No. This project emulates a physical press of the motherboard Power SW header using a relay.

Q: Do I need to enable WOL in BIOS?
A: No.

Q: Do I need port forwarding or DDNS?
A: No. ESP8266 only makes outbound HTTPS requests to Google Apps Script.

Q: Is it safe?
A: Electrically, it’s similar to the case power button (low-voltage momentary input).
Use an isolated relay module and wire to the correct motherboard header.

>[!NOTE]
>- Replace placeholders with your own Sheet ID / Web App URL
>- Do NOT commit secrets (keep config.h private)

License
Apache License 2.0
