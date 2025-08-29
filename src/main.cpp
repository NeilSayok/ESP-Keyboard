#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials - modify these for your network
const char *ssid = "Boni Home Wifi";
const char *password = "02061998";

bool wifiConnected = false;
bool fallBackToDefaultAlgo = true;

// API endpoint
const char *apiUrl = "https://apifreellm.com/api/chat";

HTTPClient http;

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

// Human-like typing function that adds natural pauses
void humanType(String text)
{
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
        Keyboard.write(wrongChar);
        humanDelay();
        Keyboard.write(0x08); // Backspace
        humanDelay();
      }
    }

    // Type the correct character
    Keyboard.write(c);

    // Add human-like delays
    humanDelay();

    // Longer pause after punctuation or spaces (natural reading rhythm)
    if (c == '.' || c == ',' || c == ' ' || c == '!' || c == '?')
    {
      delay(random(100, 300));
    }
  }
}

String getPrompt()
{
  if (!wifiConnected)
  {
    return "";
  }

  HTTPClient http;

  // Set timeouts to handle responses
  http.setTimeout(90000);        // 30 seconds timeout
  http.setConnectTimeout(90000); // 10 seconds connect timeout

  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP32");

  String payload = "{\"message\": \"give me a new prompt related to java in one line to write a program and it should be related to automation, just a question nothing else.\"}";

  int httpResponseCode = http.POST(payload);

  String response = "";
  if (httpResponseCode > 0)
  {
    String responseBody = http.getString();

    // Parse JSON response
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, responseBody);

    // Debug: print status value
    String statusValue = doc["status"];

    if (doc["status"] == "success")
    {
      response = doc["response"].as<String>();
    }
    else
    {
      response = "";
    }
  }

  http.end();
  return response;
}

String getResponse(String prompt)
{
  if (!wifiConnected || prompt.length() == 0)
  {
    return "";
  }

  HTTPClient http;

  // Set timeouts to handle large responses
  http.setTimeout(90000);        // 90 seconds timeout
  http.setConnectTimeout(90000); // 90 seconds connect timeout

  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP32");

  // Escape quotes in the prompt to prevent JSON syntax errors
  String escapedPrompt = prompt;
  escapedPrompt.replace("\"", "\\\"");
  escapedPrompt.replace("\n", "\\n");
  escapedPrompt.replace("\r", "\\r");

  String payload = "{\"message\": \"" + escapedPrompt + "\"}";

  int httpResponseCode = http.POST(payload);

  String response = "";
  if (httpResponseCode > 0)
  {
    String responseBody = http.getString();

    // Parse JSON response
    DynamicJsonDocument doc(3000);
    DeserializationError error = deserializeJson(doc, responseBody);

    // Debug: print status value
    String statusValue = doc["status"];

    if (doc["status"] == "success")
    {
      response = doc["response"].as<String>();
    }
    else
    {
      response = "";
    }
  }
  http.end();
  return response;
}

String generateRandomString(int length)
{
  const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  String result = "";
  for (int i = 0; i < length; i++)
  {
    int index = random(0, sizeof(charset) - 1);
    result += charset[index];
  }
  return result;
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

  delay(2000); // Give OS time to recognize device

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
      delay(8000);
      String response = getResponse(prompt);
      Serial.println(response);

      int startIndex = response.indexOf("```");
      int lastIndex = response.lastIndexOf("```") + 1;

      if (response.length() > 0 && startIndex > -1 && lastIndex > -1 && startIndex != lastIndex)
      {
        // Step 3: Type the response with human characteristics
        int startIndex = response.indexOf("```");
        int lastIndex = response.lastIndexOf("```") + 1;

        String updatedResponse = response.substring(startIndex, lastIndex);

        humanType(updatedResponse);

        // Human-like pause before pressing Enter
        humanDelay();
        Keyboard.write('\n'); // Press Enter
      }
      else
      {
        if (fallBackToDefaultAlgo)
        {
          humanType(generateRandomString(random(5, 20)));
        }
      }
    }
  }
  else
  {
    if (fallBackToDefaultAlgo)
    {
      humanType(generateRandomString(random(5, 20)));
    }
  }
  delay(random(10000, 20000));
}