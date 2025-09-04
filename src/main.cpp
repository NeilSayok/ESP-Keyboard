#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "./library/include/prompt.h"

// WiFi credentials - modify these for your network
const char *ssid = "Boni Home Wifi";
const char *password = "02061998";

bool wifiConnected = false;
bool fallBackToDefaultAlgo = true;
bool usbHidReady = false;

USBHIDKeyboard Keyboard;

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

// Human typing characteristics - random delays between keystrokes
void humanDelay()
{
  // Random delay between 50-150ms to simulate human typing speed
  delay(random(100, 300));
}

// Check if USB HID is ready before sending keystrokes
bool isKeyboardReady()
{
  return usbHidReady;
}

// Human-like typing function that adds natural pauses
void humanType(String text)
{
  // Check if keyboard is ready before typing
  if (!isKeyboardReady())
  {
    Serial.println("USB HID not ready, skipping typing");
    return;
  }

  for (int i = 0; i < text.length(); i++)
  {
    char c = text.charAt(i);

    // Simulate occasional typos and corrections (5% chance)
    if (random(100) < 15 && i > 0)
    {
      // Type wrong character, then backspace and correct
      char wrongChar = (char)(c + random(-3, 4));
      if (wrongChar != c && wrongChar >= 32 && wrongChar <= 126)
      {
        if (isKeyboardReady())
        {
          Keyboard.write(wrongChar);
          humanDelay();
          if (isKeyboardReady())
          {
            Keyboard.write(0x08); // Backspace
            humanDelay();
          }
        }
      }
    }

    // Type the correct character
    if (isKeyboardReady())
    {
      Keyboard.write(c);
    }
    else
    {
      Serial.println("USB HID disconnected during typing");
      return;
    }

    // Add human-like delays
    humanDelay();

    // Longer pause after punctuation or spaces (natural reading rhythm)
    if (c == '.' || c == ',' || c == ' ' || c == '!' || c == '?')
    {
      delay(random(100, 300));
    }
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

  // Configure USB device to appear as Logitech keyboard
  USB.VID(0x046D); // Logitech vendor ID
  USB.PID(0xC31C); // Generic keyboard product ID
  USB.productName("Logitech Generic Keyboard");
  USB.manufacturerName("Logitech");

  Serial.println("Device Renamed to Logitech");

  USB.begin();      // Start USB stack
  Keyboard.begin(); // Start Keyboard
  setKeyboard(Keyboard);
  delay(2000); // Give OS time to recognize device

  // Wait for USB HID to be ready - test with a simple keystroke
  Serial.println("Waiting for USB HID to be ready...");
  delay(3000); // Give additional time for USB HID initialization

  // Test if keyboard is ready by attempting a test keystroke
  // We'll assume it's ready after the delay since we can't directly check
  usbHidReady = true;
  Serial.println("USB HID marked as ready");

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
      if (isKeyboardReady())
      {
        Keyboard.write('\n'); // Press Enter
      }
    }
  }
  delay(random(10000, 20000));
}