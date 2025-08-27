#include <Arduino.h>
#include <BleKeyboard.h>
#include <BLESecurity.h>

// Create a BLE keyboard instance
BleKeyboard bleKeyboard("ESP32-S3 Keyboard", "UG-LAND", 100);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Keyboard...");

  // Start BLE Keyboard
  bleKeyboard.begin();

  // Configure BLE security
  BLESecurity *pSecurity = new BLESecurity();

  // Require bonding (stores keys so reconnects work without re-pairing)
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  // No input/output (so "Just Works" pairing method)
  pSecurity->setCapability(ESP_IO_CAP_NONE);

  // Encryption keys setup
  pSecurity->setInitEncryptionKey(
    ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK
  );
}

void loop() {
  if (bleKeyboard.isConnected()) {
    Serial.println("Connected, sending keys...");

    // Type a random string every 3 seconds
    bleKeyboard.print("Hello from ESP32-S3!\n");

    delay(3000);
  } else {
    Serial.println("Waiting for connection...");
    delay(1000);
  }
}
