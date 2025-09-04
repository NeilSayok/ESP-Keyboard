#ifndef PROMPT_H
#define PROMPT_H
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "USBHIDKeyboard.h"

void setKeyboard(USBHIDKeyboard board);
String getCleanedPrompt(String prompt);
String extractFirstCodeBlock(String unCleanMarkdown);
String callPrimaryLLM(String prompt);
String callFallBackLLM(String prompt);
String getOfflineString();
String getPrompt();
String getCode();
#endif