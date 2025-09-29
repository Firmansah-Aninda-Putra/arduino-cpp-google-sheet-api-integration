#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>
#include "DHT.h"
#include <GS_SDHelper.h>

#define WIFI_SSID "wifiid"
#define WIFI_PASSWORD "wifipassword"

// Google Project ID
#define PROJECT_ID "project-id"

// Service Account's client email
#define CLIENT_EMAIL "client_email"

// Service Account's private key (ganti dengan private key dari JSON file Anda)
const char PRIVATE_KEY[] PROGMEM = "private_key";

// The ID of the spreadsheet (ganti dengan ID spreadsheet Anda)
const char spreadsheetId[] = "char_spreadsheet";

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;

// Token Callback function
void tokenStatusCallback(TokenInfo info);

// DHT
#define DHTPIN 4 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// Variables to hold sensor readings
float suhu;
float kelembapan;

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

// Variable to save current epoch time
unsigned long epochTime; 

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup(){

    Serial.begin(115200);
    Serial.println();
    Serial.println("ESP32 Google Sheets Data Logger");
    Serial.println("================================");

    //Configure time (GMT+7 for WIB - Indonesia)
    configTime(7 * 3600, 0, ntpServer);

    // Initialize DHT
    dht.begin();

    GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);

    // Connect to Wi-Fi
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    // Set the callback for Google API access token generation status (for debug only)
    GSheet.setTokenCallback(tokenStatusCallback);

    // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
    GSheet.setPrerefreshSeconds(10 * 60);

    // Begin the access token generation for Google API authentication
    GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}

void loop(){
    // Call ready() repeatedly in loop for authentication checking and processing
    bool ready = GSheet.ready();

    if (ready && millis() - lastTime > timerDelay){
        lastTime = millis();

        FirebaseJson response;

        Serial.println("\nAppending data to spreadsheet...");
        Serial.println("--------------------------------");

        FirebaseJson valueRange;
        suhu = dht.readTemperature();
        kelembapan = dht.readHumidity();
        
        // Check if sensor readings are valid
        if (isnan(suhu) || isnan(kelembapan)) {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }
        
        // Get timestamp
        epochTime = getTime();

        // Display readings
        Serial.println("Sensor readings:");
        Serial.printf("Temperature: %.2f°C\n", suhu);
        Serial.printf("Humidity: %.2f%%\n", kelembapan);
        Serial.printf("Timestamp: %lu\n", epochTime);

        valueRange.add("majorDimension", "ROWS");
        valueRange.set("values/[0]/[0]", epochTime);
        valueRange.set("values/[0]/[1]", suhu);
        valueRange.set("values/[0]/[2]", kelembapan);

        // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/append
        // Append values to the spreadsheet
        bool success = GSheet.values.append(&response /* returned response */, 
                                          spreadsheetId /* spreadsheet Id to append */, 
                                          "Sheet1!A1" /* range to append */, 
                                          &valueRange /* data range to append */);
        if (success){
            Serial.println("Data successfully sent to Google Sheets!");
            response.toString(Serial, true);
            valueRange.clear();
        }
        else{
            Serial.println("Error sending data to Google Sheets:");
            Serial.println(GSheet.errorReason());
        }
        Serial.println();
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        Serial.println("Waiting for next reading...\n");
    }
}

void tokenStatusCallback(TokenInfo info){
    if (info.status == token_status_error){
        Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        Serial.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}
