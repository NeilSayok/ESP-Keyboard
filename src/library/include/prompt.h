#ifndef PROMPT_H
#define PROMPT_H
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "USBHIDKeyboard.h"

String getCleanedPrompt(String prompt);
String extractFirstCodeBlock(String unCleanMarkdown);
String callLLM1(String prompt);
String callLLM2(String prompt);
String callLLM3(String prompt);
String getOfflineString();
String getPrompt();
String getCode();
#endif