#include "../include/mouse.h"
#include <Arduino.h>
#include "USBHIDMouse.h"
#include "USB.h"

USBHIDMouse Mouse;

void setupMouse()
{
    Mouse.begin();
    delay(2000);
}

void doMouseMovement(){
    int movementType = random(1,4);
    int distance = random(10.0,30.0);
    int baseDelay = random(1,3);

    switch(movementType){
        case 1 : 
            moveMouseCircleHuman(distance,baseDelay);
            break;
        case 2 : 
            moveMouseHorizontalHuman(distance,baseDelay);
            break;
        case 3 : 
            moveMouseVerticalHuman(distance,baseDelay);
            break;
        case 4 : 
            bool downRight = random(0, 2) == 1 ;
            moveMouseDiagonalHuman(distance,baseDelay,downRight);
            break;
        
    }

}


void moveMouseCircleHuman(int radius, int baseDelay) {
  const int steps = 360; // more steps = smoother circle
  for (int i = 0; i < steps; i++) {
    float angle = (2 * PI * i) / steps;

    // Base movement
    int dx = (int)(radius * cos(angle));
    int dy = (int)(radius * sin(angle));

    // Add small random jitter (like hand shaking)
    dx += random(-2, 3);
    dy += random(-2, 3);

    Mouse.move(dx, dy);

    // Randomize delay a bit (not constant speed)
    int jitterDelay = baseDelay + random(-20, 30);
    if (jitterDelay < 5) jitterDelay = 5;
    delay(jitterDelay);

    // Occasionally pause longer
    if (random(0, 20) == 0) {
      delay(300 + random(0, 300));
    }
  }
}

// Horizontal movement
void moveMouseHorizontalHuman(int distance, int baseDelay) {
  int step = (distance > 0) ? 1 : -1;
  for (int i = 0; i != distance; i += step) {
    int dx = step + random(-1, 2); // jitter
    int dy = random(-1, 2);        // slight vertical jitter
    Mouse.move(dx, dy);

    int jitterDelay = baseDelay + random(-10, 15);
    if (jitterDelay < 5) jitterDelay = 5;
    delay(jitterDelay);

    if (random(0, 40) == 0) delay(200 + random(0, 200));
  }
}

// Vertical movement
void moveMouseVerticalHuman(int distance, int baseDelay) {
  int step = (distance > 0) ? 1 : -1;
  for (int i = 0; i != distance; i += step) {
    int dx = random(-1, 2);        // slight horizontal jitter
    int dy = step + random(-1, 2); // jitter
    Mouse.move(dx, dy);

    int jitterDelay = baseDelay + random(-10, 15);
    if (jitterDelay < 5) jitterDelay = 5;
    delay(jitterDelay);

    if (random(0, 40) == 0) delay(200 + random(0, 200));
  }
}

// Diagonal movement
void moveMouseDiagonalHuman(int distance, int baseDelay, bool downRight) {
  int step = (distance > 0) ? 1 : -1;
  for (int i = 0; i != distance; i += step) {
    int dx = step + random(-1, 2);
    int dy = (downRight ? step : -step) + random(-1, 2);
    Mouse.move(dx, dy);

    int jitterDelay = baseDelay + random(-10, 15);
    if (jitterDelay < 5) jitterDelay = 5;
    delay(jitterDelay);

    if (random(0, 40) == 0) delay(200 + random(0, 200));
  }
}