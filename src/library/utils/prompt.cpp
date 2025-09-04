#include "../include/prompt.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "USBHIDKeyboard.h"

// API endpoint
const char *primaryLLMUrl = "https://api.groq.com/openai/v1/chat/completions";
const char *fallbackLLMUrl = "https://apifreellm.com/api/chat";

String groqToken = "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";
String contentType = "application/json";
String userAgent = "ESP32";

USBHIDKeyboard internalKeyboard;

void setKeyboard(USBHIDKeyboard board)
{
    internalKeyboard = board;
}

void KeyboardPrint(String from, String data)
{
    // internalKeyboard.print(from);
    // internalKeyboard.print(" : ");
    // internalKeyboard.print(data);
    // internalKeyboard.println();
    // internalKeyboard.print("------------------------------------------\n");
    // internalKeyboard.println();
}

String getCleanedPrompt(String prompt)
{
    String escapedPrompt = prompt;
    escapedPrompt.replace("\"", "\\\"");
    escapedPrompt.replace("\n", "\\n");
    escapedPrompt.replace("\r", "\\r");
    return escapedPrompt;
}

String extractFirstCodeBlock(String unCleanMarkdown)
{

    String markdown = unCleanMarkdown;
    markdown.replace("\"", "\\\"");
    markdown.replace("\n", "\\n");
    markdown.replace("\r", "\\r");

    int startPos = markdown.indexOf("```");
    if (startPos == -1)
        return "";

    // Skip language line
    int lineEnd = markdown.indexOf('\n', startPos);
    if (lineEnd == -1)
        return "";

    // Find closing fence
    int endPos = markdown.indexOf("```", lineEnd);
    if (endPos == -1)
        return "";

    return markdown.substring(lineEnd + 1, endPos);
}

// String callPrimaryLLM(String prompt)
// {
//     HTTPClient http;

//     http.setTimeout(65535);
//     http.setConnectTimeout(65535);
//     http.begin(primaryLLMUrl);
//     http.addHeader("Content-Type", contentType);
//     http.addHeader("User-Agent", userAgent);
//     http.addHeader("Authorization", "Bearer " + groqToken);

//     String payload = "{"
//                      "  \"model\": \"llama-3.1-8b-instant\","
//                      "  \"messages\": [{\"role\": \"user\", \"content\": \"" +
//                      getCleanedPrompt(prompt) + " and only give the java code.\"}],"
//                                                 "  \"temperature\": 0.7,"
//                                                 "  \"max_tokens\": 1024"
//                                                 "}";

//     int httpResponseCode = http.POST(payload);

//     String response = "";

//     if (httpResponseCode > 0)
//     {
//         String responseBody = http.getString();
//         // Parse JSON response
//         DynamicJsonDocument doc(4096);
//         DeserializationError error = deserializeJson(doc, responseBody);

//         if (!error)
//         {
//             if (doc["choices"] && doc["choices"].size() > 0)
//             {
//                 response = doc["choices"][0]["message"]["content"].as<String>();
//             }
//         }
//     }
//     http.end();

//     return response;
// }

String callFallBackLLM(String prompt)
{
    HTTPClient http;

    http.setTimeout(65535);
    http.setConnectTimeout(65535);
    http.begin(fallbackLLMUrl);
    http.addHeader("Content-Type", contentType);
    http.addHeader("User-Agent", userAgent);

    String payload = "{\"message\": \"" + getCleanedPrompt(prompt) + "\"}";

    int httpResponseCode = http.POST(payload);
    String response = "";

    if (httpResponseCode > 0)
    {
        String responseBody = http.getString();

        // Parse JSON response
        DynamicJsonDocument doc(3000);
        DeserializationError error = deserializeJson(doc, responseBody);
        if (!error)
        {
            if (doc["status"] == "success")
            {
                response = doc["response"].as<String>();
            }
        }
    }
    http.end();

    return response;
}

String getOfflineString()
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    String result = "";
    for (int i = 0; i < random(5, 10); i++)
    {
        int index = random(0, sizeof(charset) - 1);
        result += charset[index];
    }
    return result;
}

String getPrompt()
{
    String prompt = "give me a new prompt related to java in one line to write a program and it should be related to automation, just a question nothing else.";
    return callFallBackLLM(prompt);
}

String getCode()
{
    String prompt = getPrompt();

    String response = callFallBackLLM(prompt);
    response = extractFirstCodeBlock(response);

    KeyboardPrint("getCode-callFallBackLLM", String(response));

    // if (response.length() <= 0)
    // {
    //     response = callFallBackLLM(prompt);
    //     KeyboardPrint("getCode-callFallBackLLM", String(response));
    // }
    // KeyboardPrint("getCode-extractFirstCodeBlock", String(response));

    if (response.length() <= 0)
    {
        response = getOfflineString();
    }
    KeyboardPrint("getCode-getOfflineString", String(response));

    return response;
}