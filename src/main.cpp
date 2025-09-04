#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "./library/include/prompt.h"
#include "./library/include/typing.h"

// WiFi credentials - modify these for your network
const char *ssid = "Boni Home Wifi";
const char *password = "02061998";

bool wifiConnected = false;
bool fallBackToDefaultAlgo = true;

void connectToWiFi()
{
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    wifiConnected = true;
  }
  else
  {
    wifiConnected = false;
  }
}

void checkWiFi()
{
  if (WiFi.status() != WL_CONNECTED && wifiConnected)
  {
    wifiConnected = false;
    connectToWiFi();
  }
}

void setup()
{
  // Initialize Serial for debugging - must be first
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
  {
    delay(10); // Wait for serial port to connect, but timeout after 5 seconds
  }
  delay(1000);

  Serial.println("Program Started");

  setupKeyBoard();

  

  // Connect to WiFi
  connectToWiFi();
}

void loop()
{
  // Check WiFi status periodically
  checkWiFi();

  if (wifiConnected)
  {
    // Step 1: Get a prompt
    String prompt = getPrompt();
    Serial.println(prompt);

    if (prompt.length() > 0)
    {
      String response = getCode();
      Serial.println(response);

      humanType(response);
      humanDelay();
      humanType("\n");
    }
  }
  delay(random(10000, 20000));
}