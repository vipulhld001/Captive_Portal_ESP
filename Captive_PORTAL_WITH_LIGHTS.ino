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

const int ledPin = 27;
const int greenPin = 26;
//bool ledState = false;

static const char responsePortal[] = R"===(
<!DOCTYPE html><html><head><title>ESP32 CaptivePortal</title></head><body>
<h1>MAA BETA HAR GHAR UJALA YOJNA</h1><p>Jor ka Zhatka haye joro se laga.</p>
<h1>MUMMY LIGHT ON KAR DO!!!<h1>    
<button type="button" onclick="fetch('/toggle')">Red Light ON!</button>
<button type="button" onclick="fetch('/togglegreen')">Green Light ON!</button>
<button type="button" onclick="fetch('/toggle2')">Light OFF!</button> </body></html>
)===";

// index page handler
void handleRoot() {
  server.send(200, "text/plain", "Hello from the other Side");
}

void handleToggle() {
  //ledState = !ledState;
  digitalWrite(ledPin, HIGH );
  server.send(200, "text/plain", "OK"); // Send a success response back to fetch()
  Serial.println("Button was clicked! LED state changed to HIGH.");
}

void handleToggle2() {
  //ledState = !ledState;
  digitalWrite(greenPin, LOW);
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "OK"); // Send a success response back to fetch()
  Serial.println("Button was clicked! LED state changed to LOW.");
}



void handleTogglegreen() {
  //ledState = !ledState;
  digitalWrite(greenPin, HIGH);
  server.send(200, "text/plain", "OK"); // Send a success response back to fetch()
  Serial.println("Button was clicked! LED state changed to LOW.");
}
// this will redirect unknown http req's to our captive portal page
// based on this redirect various systems could detect that WiFi AP has a captive portal page
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

  // by default DNSServer is started serving any "*" domain name. It will reply
  // AccessPoint's IP to all DNS request (this is required for Captive Portal detection)
  if (dnsServer.start()) {
    Serial.println("Started DNS server in captive portal-mode");
  } else {
    Serial.println("Err: Can't start DNS server!");
  }

  // serve a simple root page
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/toggle2", handleToggle2);
  server.on("/togglegreen", handleTogglegreen);
  // serve portal page
  server.on("/portal", []() {
    server.send(200, "text/html", responsePortal);
  });

  // all unknown pages are redirected to captive portal
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
  delay(5);  // give CPU some idle time
}
