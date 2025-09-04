#ifndef TYPING_H
#define TYPING_H
#include <Arduino.h>
#include "USBHIDKeyboard.h"
#include "USB.h"

void setupKeyBoard();
void humanDelay();
void humanType(String text);


#endif