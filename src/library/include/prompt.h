#ifndef PROMPT_H
#define PROMPT_H
String getCleanedPrompt(String prompt);
String extractFirstCodeBlock(String unCleanMarkdown);
String callPrimaryLLM(String prompt);
String callFallBackLLM(String prompt);
String getOfflineString();
String getPrompt();
String getCode();
#endif