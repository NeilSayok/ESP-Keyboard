#ifndef MOUSE_H
#define MOUSE_H
#include <Arduino.h>
#include "USBHIDMouse.h"
#include "USB.h"

void setupMouse();
void doMouseMovement();
void moveMouseCircleHuman(int radius, int baseDelay);
void moveMouseHorizontalHuman(int radius, int baseDelay);
void moveMouseVerticalHuman(int radius, int baseDelay);
void moveMouseDiagonalHuman(int distance, int baseDelay, bool downRight);

#endif