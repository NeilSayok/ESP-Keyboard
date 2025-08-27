#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

USBHIDKeyboard Keyboard;

// WiFi credentials - modify these for your network
const char* ssid = "Boni Home Wifi";
const char* password = "02061998";

bool wifiConnected = false;

// API endpoint
const char* apiUrl = "https://apifreellm.com/api/chat";

// Function to make API call and get prompt
String getPrompt() {
  if (!wifiConnected) {
    Keyboard.print("WiFi not connected, cannot make API call");
    Keyboard.write('\n');
    return "";
  }

  HTTPClient http;
  
  // Set timeouts to handle responses
  http.setTimeout(30000);  // 30 seconds timeout
  http.setConnectTimeout(10000);  // 10 seconds connect timeout
  
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP32");
  
  String payload = "{\"message\": \"give me a new prompt related to java in one line to write a program and it should be related to automation, just a question nothing else.\"}";
  
  Keyboard.print("Making API call to get prompt...");
  Keyboard.write('\n');
  int httpResponseCode = http.POST(payload);
  
  // Add detailed error code interpretation
  if (httpResponseCode < 0) {
    Keyboard.print("HTTP Error Code: " + String(httpResponseCode));
    Keyboard.write('\n');
    
    switch(httpResponseCode) {
      case -1: Keyboard.print("Error: Connection failed"); break;
      case -2: Keyboard.print("Error: Send header failed"); break;
      case -3: Keyboard.print("Error: Send payload failed"); break;
      case -4: Keyboard.print("Error: Not connected"); break;
      case -5: Keyboard.print("Error: Connection lost"); break;
      case -6: Keyboard.print("Error: No stream"); break;
      case -7: Keyboard.print("Error: No HTTP server"); break;
      case -8: Keyboard.print("Error: Too less RAM"); break;
      case -9: Keyboard.print("Error: Encoding error"); break;
      case -10: Keyboard.print("Error: Stream write error"); break;
      case -11: Keyboard.print("Error: Read timeout"); break;
      default: Keyboard.print("Error: Unknown HTTP error"); break;
    }
    Keyboard.write('\n');
  }
  
  String response = "";
  if (httpResponseCode > 0) {
    String responseBody = http.getString();
    Keyboard.print("API Response: " + responseBody);
    Keyboard.write('\n');
    
    // Parse JSON response
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, responseBody);
    
    if (error) {
      Keyboard.print("JSON parsing failed: " + String(error.c_str()));
      Keyboard.write('\n');
    } else {
      Keyboard.print("JSON parsed successfully");
      Keyboard.write('\n');
      
      // Debug: print status value
      String statusValue = doc["status"];
      Keyboard.print("Status field value: '" + statusValue + "'");
      Keyboard.write('\n');
      
      if (doc["status"] == "success") {
        response = doc["response"].as<String>();
        Keyboard.print("Extracted prompt: " + response);
        Keyboard.write('\n');
      } else {
        Keyboard.print("API call failed - status not success");
        Keyboard.write('\n');
      }
    }
  } else {
    Keyboard.print("HTTP Error: " + String(httpResponseCode));
    Keyboard.write('\n');
  }
  
  http.end();
  return response;
}

// Function to make API call and get response for the prompt
String getResponse(String prompt) {
  if (!wifiConnected || prompt.length() == 0) {
    Keyboard.print("WiFi not connected or empty prompt, cannot make API call");
    Keyboard.write('\n');
    return "";
  }

  HTTPClient http;
  
  // Set timeouts to handle large responses
  http.setTimeout(30000);  // 30 seconds timeout
  http.setConnectTimeout(10000);  // 10 seconds connect timeout
  
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP32");
  
  // Escape quotes in the prompt to prevent JSON syntax errors
  String escapedPrompt = prompt;
  escapedPrompt.replace("\"", "\\\"");
  escapedPrompt.replace("\n", "\\n");
  escapedPrompt.replace("\r", "\\r");
  
  String payload = "{\"message\": \"" + escapedPrompt + "\"}";
  
  Keyboard.print("Making API call to get response for prompt...");
  Keyboard.write('\n');
  Keyboard.print("Payload length: " + String(payload.length()));
  Keyboard.write('\n');
  Keyboard.print("Payload: " + String(escapedPrompt));
  Keyboard.write('\n');
  
  int httpResponseCode = http.POST(payload);
  
  // Add detailed error code interpretation
  if (httpResponseCode < 0) {
    Keyboard.print("HTTP Error Code: " + String(httpResponseCode));
    Keyboard.write('\n');
    
    switch(httpResponseCode) {
      case -1: Keyboard.print("Error: Connection failed"); break;
      case -2: Keyboard.print("Error: Send header failed"); break;
      case -3: Keyboard.print("Error: Send payload failed"); break;
      case -4: Keyboard.print("Error: Not connected"); break;
      case -5: Keyboard.print("Error: Connection lost"); break;
      case -6: Keyboard.print("Error: No stream"); break;
      case -7: Keyboard.print("Error: No HTTP server"); break;
      case -8: Keyboard.print("Error: Too less RAM"); break;
      case -9: Keyboard.print("Error: Encoding error"); break;
      case -10: Keyboard.print("Error: Stream write error"); break;
      case -11: Keyboard.print("Error: Read timeout"); break;
      default: Keyboard.print("Error: Unknown HTTP error"); break;
    }
    Keyboard.write('\n');
  }
  
  String response = "";
  if (httpResponseCode > 0) {
    String responseBody = http.getString();
    Keyboard.print("API Response: " + responseBody);
    Keyboard.write('\n');
    
    // Parse JSON response
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, responseBody);
    
    if (error) {
      Keyboard.print("JSON parsing failed: " + String(error.c_str()));
      Keyboard.write('\n');
    } else {
      Keyboard.print("JSON parsed successfully");
      Keyboard.write('\n');
      
      // Debug: print status value
      String statusValue = doc["status"];
      Keyboard.print("Status field value: '" + statusValue + "'");
      Keyboard.write('\n');
      
      if (doc["status"] == "success") {
        response = doc["response"].as<String>();
        Keyboard.print("Extracted response: " + response);
        Keyboard.write('\n');
      } else {
        Keyboard.print("API call failed - status not success");
        Keyboard.write('\n');
      }
    }
  } else {
    Keyboard.print("HTTP Error: " + String(httpResponseCode));
    Keyboard.write('\n');
  }
  
  http.end();
  return response;
}

// WiFi connection function
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Keyboard.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Keyboard.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Keyboard.write('\n');
    Keyboard.print("WiFi connected!");
    Keyboard.write('\n');
    Keyboard.print("IP address: " + WiFi.localIP().toString());
    Keyboard.write('\n');
    Keyboard.print("RSSI: " + String(WiFi.RSSI()));
    Keyboard.write('\n');
  } else {
    Keyboard.write('\n');
    Keyboard.print("WiFi connection failed!");
    Keyboard.write('\n');
  }
}

// Check WiFi status and reconnect if needed
void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    Keyboard.print("WiFi disconnected, attempting to reconnect...");
    Keyboard.write('\n');
    wifiConnected = false;
    connectToWiFi();
  }
}

// Human typing characteristics - random delays between keystrokes
void humanDelay() {
  // Random delay between 50-150ms to simulate human typing speed
  delay(random(50, 150));
}

// Human-like typing function that adds natural pauses
void humanType(String text) {
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    
    // Simulate occasional typos and corrections (5% chance)
    if (random(100) < 5 && i > 0) {
      // Type wrong character, then backspace and correct
      char wrongChar = (char)(c + random(-3, 4));
      if (wrongChar != c && wrongChar >= 32 && wrongChar <= 126) {
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
    if (c == '.' || c == ',' || c == ' ' || c == '!' || c == '?') {
      delay(random(100, 300));
    }
  }
}

void setup() {
  // Initialize Serial for debugging - must be first
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {
    delay(10); // Wait for serial port to connect, but timeout after 5 seconds
  }
  delay(1000);
  
  // Configure USB device to appear as Logitech keyboard
  USB.VID(0x046D);  // Logitech vendor ID
  USB.PID(0xC31C);  // Generic keyboard product ID
  USB.productName("Logitech Generic Keyboard");
  USB.manufacturerName("Logitech");
  
  USB.begin();         // Start USB stack
  Keyboard.begin();    // Start Keyboard
  
  delay(2000);         // Give OS time to recognize device
  
  // Announce startup
  Keyboard.print("=== ESP32 Keyboard Starting ===");
  Keyboard.write('\n');
  Keyboard.print("Logitech Generic Keyboard initialized");
  Keyboard.write('\n');
  
  // Connect to WiFi
  Keyboard.print("Starting WiFi connection...");
  Keyboard.write('\n');
  connectToWiFi();
}

void loop() {
  // Check WiFi status periodically
  checkWiFi();
  
  if (wifiConnected) {
    Keyboard.print("Starting API workflow...");
    Keyboard.write('\n');
    
    // Step 1: Get a Java automation prompt
    String prompt = getPrompt();
    
    if (prompt.length() > 0) {
      Keyboard.print("Got prompt, now getting response...");
      Keyboard.write('\n');
      
      // Add delay between API calls to prevent server overload
      delay(2000);
      
      // Step 2: Get response for the prompt
      String response = getResponse(prompt);
      
      if (response.length() > 0) {
        Keyboard.print("Got response, typing it out...");
        Keyboard.write('\n');
        
        // Step 3: Type the response with human characteristics
        humanType(response);
        
        // Human-like pause before pressing Enter
        delay(random(200, 500));
        Keyboard.write('\n');   // Press Enter
        
        Keyboard.print("Response typed successfully!");
        Keyboard.write('\n');
      } else {
        Keyboard.print("Failed to get response from API");
        Keyboard.write('\n');
        // Type error message instead
        humanType("API Error: No response received");
        delay(random(200, 500));
        Keyboard.write('\n');
      }
    } else {
      Keyboard.print("Failed to get prompt from API");
      Keyboard.write('\n');
      // Type error message instead
      humanType("API Error: No prompt received");
      delay(random(200, 500));
      Keyboard.write('\n');
    }
  } else {
    // If WiFi is not connected, type status message
    Keyboard.print("WiFi not connected, typing status...");
    Keyboard.write('\n');
    humanType("WiFi Disconnected - Cannot access API");
    delay(random(200, 500));
    Keyboard.write('\n');
  }
  
  // Wait before starting the whole process again (10-20 seconds)
  Keyboard.print("Waiting before next cycle...");
  Keyboard.write('\n');
  delay(random(10000, 20000));
}