#include "../include/typing.h"
#include <Arduino.h>
#include "USBHIDKeyboard.h"
#include "USB.h"

USBHIDKeyboard Keyboard;

void setupKeyBoard()
{
    // Configure USB device to appear as Logitech keyboard
    USB.VID(0x046D); // Logitech vendor ID
    USB.PID(0xC31C); // Generic keyboard product ID
    USB.productName("Logitech Generic Keyboard");
    USB.manufacturerName("Logitech");

    USB.begin();      // Start USB stack
    Keyboard.begin(); // Start Keyboard

    Serial.println("Device Renamed to Logitech");

    delay(3000);

    Serial.println("USB HID marked as ready");
}

void humanDelay()
{
    delay(random(100, 300));
}

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
        Keyboard.write(c);
        humanDelay();
        if (c == '.' || c == ',' || c == ' ' || c == '!' || c == '?')
        {
            delay(random(200, 350));
        }
    }
}