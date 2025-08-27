#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

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
  // Configure USB device to appear as Logitech keyboard
  USB.VID(0x046D);  // Logitech vendor ID
  USB.PID(0xC31C);  // Generic keyboard product ID
  USB.productName("Logitech Generic Keyboard");
  USB.manufacturerName("Logitech");
  
  USB.begin();         // Start USB stack
  Keyboard.begin();    // Start Keyboard
  delay(2000);         // Give OS time to recognize device
}

void loop() {
  // Generate a random number string
  String msg = "Random_" + String(random(1000, 9999));

  // Type it with human characteristics
  humanType(msg);
  
  // Human-like pause before pressing Enter
  delay(random(200, 500));
  Keyboard.write('\n');   // Press Enter

  // Variable wait time between messages (3-7 seconds)
  delay(random(3000, 7000));
}