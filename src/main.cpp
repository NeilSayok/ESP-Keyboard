#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup() {
  USB.begin();         // Start USB stack
  Keyboard.begin();    // Start Keyboard
  delay(2000);         // Give OS time to recognize device
}

void loop() {
  // Generate a random number string
  String msg = "Random_" + String(random(1000, 9999));

  // Type it like a keyboard
  Keyboard.print(msg);
  Keyboard.write('\n');   // Press Enter

  delay(3000); // wait 3 seconds
}

