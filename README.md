# ESP32 Captive Portal Light Controller

A lightweight ESP32-based Arduino project that creates a Wi-Fi Access Point with a **Captive Portal**. When devices (Android, Windows, Ubuntu, iOS) connect to the Wi-Fi network, a browser banner or pop-up automatically prompts the user to interact with a web page to control dual LEDs (Red and Green).

---

## 📌 Features

- **Catch-All Captive Portal**: Intercepts DNS requests and automatically prompts connecting devices to open the Web Portal without requiring manual IP entry (`192.168.4.1`).
- **Wi-Fi Access Point (SoftAP)**: Broadcasts a custom SSID (`ESP32-Viplu`).
- **Web UI & Control**: Simple HTML interface with asynchronous `fetch()` requests to toggle connected LEDs.
- **Dual LED Output**:
  - **Red LED** control on GPIO 27.
  - **Green LED** control on GPIO 26.
- **Asynchronous Web Handlers**: Quick response handling for toggling light states on/off.

---

## 🛠️ Hardware Requirements

| Component | Quantity | Notes / Connections |
| :--- | :---: | :--- |
| **ESP32 Development Board** | 1 | NodeMCU-32S / ESP32 WROOM or similar |
| **Red LED** | 1 | Connected to **GPIO 27** via current-limiting resistor |
| **Green LED** | 1 | Connected to **GPIO 26** via current-limiting resistor |
| **Resistors** | 2 | 220Ω – 330Ω recommended for LEDs |
| **Breadboard & Jumpers** | - | Wiring connections |
| **Micro-USB Cable** | 1 | Power & programming |

---

## 🔌 Circuit Pinout

| ESP32 GPIO Pin | Component | Description |
| :--- | :--- | :--- |
| **GPIO 27** | Red LED (Anode / +) | Output control for Red LED |
| **GPIO 26** | Green LED (Anode / +) | Output control for Green LED |
| **GND** | LED Cathodes (-) | Common Ground (via resistors) |

---

## 📋 Software Prerequisites & Libraries

Ensure you have the **ESP32 board package** installed in your Arduino IDE or PlatformIO.

### Built-in ESP32 Libraries Used:
- `<Arduino.h>`
- `<WiFi.h>` – Handles SoftAP setup.
- `<DNSServer.h>` – Redirects DNS queries for Captive Portal functionality.
- `<WebServer.h>` – Serves HTML web pages and API endpoints.

---

## 📁 Source Code Overview

Below is the complete source code for the project:

```cpp
/*
This example enables catch-all Captive portal for ESP32 Access-Point
It will allow modern devices/OSes to detect that WiFi connection is
limited and offer a user to access a banner web-page.
There is no need to find and open device's IP address/URL, i.e. http://192.168.4.1/
This works for Android, Ubuntu, FireFox, Windows, maybe others...
*/

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

DNSServer dnsServer;
WebServer server(80);

const int ledPin = 27;     // Red LED pin
const int greenPin = 26;   // Green LED pin

static const char responsePortal[] = R"===(
<!DOCTYPE html><html><head><title>ESP32 CaptivePortal</title></head><body>
<h1>MAA BETA HAR GHAR UJALA YOJNA</h1><p>Jor ka Zhatka haye joro se laga.</p>
<h1>MUMMY LIGHT ON KAR DO!!!</h1>    
<button type="button" onclick="fetch('/toggle')">Red Light ON!</button>
<button type="button" onclick="fetch('/togglegreen')">Green Light ON!</button>
<button type="button" onclick="fetch('/toggle2')">Light OFF!</button> </body></html>
)===";

// Index page handler
void handleRoot() {
  server.send(200, "text/plain", "Hello from the other Side");
}

// Turn Red LED ON
void handleToggle() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/plain", "OK");
  Serial.println("Button was clicked! Red LED state changed to HIGH.");
}

// Turn Both LEDs OFF
void handleToggle2() {
  digitalWrite(greenPin, LOW);
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "OK");
  Serial.println("Button was clicked! Both LEDs state changed to LOW.");
}

// Turn Green LED ON
void handleTogglegreen() {
  digitalWrite(greenPin, HIGH);
  server.send(200, "text/plain", "OK");
  Serial.println("Button was clicked! Green LED state changed to HIGH.");
}

// Redirect unknown HTTP requests to captive portal page
void handleNotFound() {
  server.sendHeader("Location", "/portal");
  server.send(302, "text/plain", "redirect to captive portal");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-Viplu");
  
  pinMode(ledPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(greenPin, LOW);

  // Start DNS Server in wildcard mode for Captive Portal redirection
  if (dnsServer.start()) {
    Serial.println("Started DNS server in captive portal-mode");
  } else {
    Serial.println("Err: Can't start DNS server!");
  }

  // Register HTTP endpoint routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/toggle2", handleToggle2);
  server.on("/togglegreen", handleTogglegreen);
  
  // Serve Captive Portal UI
  server.on("/portal", []() {
    server.send(200, "text/html", responsePortal);
  });

  // Catch-all route for redirecting connected clients
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
  delay(5);  // CPU idle allowance
}
```

---

## 🚀 How to Run & Use

1. **Upload the Code**: Open the project in Arduino IDE, select your ESP32 board, set the baud rate to `115200`, and upload the code.
2. **Connect to Wi-Fi**: On your smartphone or computer, search for available Wi-Fi networks and connect to **`ESP32-Viplu`** (no password required).
3. **Captive Portal Pop-up**:
   - Most modern operating systems will immediately notify you: *"Sign in to Wi-Fi network"*.
   - Tap/click the notification to open the interactive captive portal page.
   - Alternatively, navigate to any website or open `http://192.168.4.1/portal` in a browser.
4. **Control LEDs**:
   - Press **Red Light ON!** to illuminate GPIO 27.
   - Press **Green Light ON!** to illuminate GPIO 26.
   - Press **Light OFF!** to turn off both LEDs.

---

## 🌐 API Routes Reference

| Route | HTTP Method | Description |
| :--- | :---: | :--- |
| `/` | `GET` | Returns plaintext welcome message (`Hello from the other Side`). |
| `/portal` | `GET` | Serves the Captive Portal HTML UI interface. |
| `/toggle` | `GET` | Turns **Red LED (GPIO 27)** `HIGH`. |
| `/togglegreen` | `GET` | Turns **Green LED (GPIO 26)** `HIGH`. |
| `/toggle2` | `GET` | Turns **Both LEDs** `LOW`. |
| `*` (NotFound) | `GET` | Issues a `302 Redirect` to `/portal`. |
