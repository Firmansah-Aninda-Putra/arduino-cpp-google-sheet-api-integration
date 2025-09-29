#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>
#include "DHT.h"
#include <GS_SDHelper.h>

#define WIFI_SSID "3-JAGOANKU"
#define WIFI_PASSWORD "janganlupabismillah"

// Google Project ID
#define PROJECT_ID "iot-datalogging-472814"

// Service Account's client email
#define CLIENT_EMAIL "iot-datalogging@iot-datalogging-472814.iam.gserviceaccount.com"

// Service Account's private key (ganti dengan private key dari JSON file Anda)
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\n"
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQC2Lz81yAWFqmAG\n"
"oTN2HWPhiaZVfB9EEWIXT7Gsv0TnLmD8lbfXS/mNpbeqBTbWN1w8UIY9w6jXjQzt\n"
"5wR8jc9F3f5XUztYWASxPsm9oFpYs9jrbdWxgfMlpnPZHS/xaWPPlz1lVvqrLHQy\n"
"jf8KchqtCptQLxrXaRNFcJBBBYi7yb3t+yvZwq4c7gDwC1fgAP78Q9ugIWjNURpq\n"
"jfGYpGu+YuY7F2tSZkxnaMUDSh8SeD2O3w3FfTgk2Si/T3LxB6o/DvTctYX5BJa6\n"
"bJ5uvJWTCZHwtpF9GorMjrRWGPH1IS3WS9A0CW1SkFpGKiy4xCZogTqJbqDrCaZO\n"
"GRqzouxHAgMBAAECggEAH7UNXUDYpTDH0Z+Qo0fuMDyStGIL1v7ngYMfOx920H1j\n"
"59Hu0wun/R0QYAjUSGgOJ3kkQ++fnRQxYd5JC5xqrJA8Sdp0g6Wd+3MgDV6xGqsC\n"
"X8cX3/Lr7GuR0gGgbHWZJ3H8/3wp2Ey+VGYeMyU0u1UTPttT4pWpAvbaRFbAO15P\n"
"0I3DTI1Cuhf4OAGRcZYCSj0dM9Vkkey2iIbkrHukczkiBjsM0h06eslogbQVTuRb\n"
"VYp6Xg4s7WwHClPOhtVnkpxqxkgFHGGSMEftyTrm73bnKC4gPyrUfuCxa51LrNjV\n"
"161dg6+BmL8LfyYmF6Mh/x+rm/YAKl0jfxhbLwDHgQKBgQDn+CSjU87IJR50lrtc\n"
"sbRvgBoTE9Ps1m8JODlEW/woFsToqIRg2cDIRmzhYHau747NlJaEVvC3Rml/dcOd\n"
"rAbL7cWAIN3eP2AGhzg/x9HNH93eZ7axzwVKNyWuKVEfA8OGhesLG4intu8Kza5A\n"
"5se8SyBKx7mSqfMoFTaE8+/NpwKBgQDJDs47cT8aXhiqM0vona4bN8QrAzMJJ473\n"
"zoLilCNfd18HU+K9U5FCDWo9ytp2fRnHBOGb9XjR8mPj2+i7wieNpvrHrB0REeDs\n"
"/Pn31LulbbKq5Git3pVfP8fpe/K1ZyRnsfvVXeDCjMHwZGHEoyO+xN2e66RasQLK\n"
"QiKuAMoAYQKBgAjuIaL0CALmguykHORtpu+f8+EGI2N7wdyOAI5kKCn27m+yhR4h\n"
"Y+2R4IEwsID9kfbSIJVJr4yutkME6pn4VG6nYbIUEdaLhAvcSssRyz7lWipEveDM\n"
"gnyf8OBqkMfr4iwcnbbYwYMLZNp6npm7CLd0APNACyHdTxIc9vNZXMMDAoGAZ74T\n"
"++f7P4e+AlRlnql95N1pgygrdVXMF2Gfd3oopmHZztYh8L7l/McfQrMg9nKmkmKa\n"
"ZvbKWMpRcIlQFVGhRx4Z2F2ewegPvzjpYk+KY1mvDy68NLrdKECoznj+S0vlJPxZ\n"
"jUbbaodmqruhFu12zECYeh/stj8PyX1Gl7IgiQECgYBa2jvkyz/7FyjwGjDFr87O\n"
"M69Y1OKms1GW+BGZXi72Qgj0rmLaVvdVFgAVOmcZxXEPl90+4uQeiFxfH/LZWzAt\n"
"cu18ONNOVbH31oW02LLVP/UX/VG0hC+l7vvYz2V7jskl0F6GBwk2tbBppKF3ytJJ\n"
"obYWGthGecT3aRzxRtdgmw==\n"
"-----END PRIVATE KEY-----\n";

// The ID of the spreadsheet (ganti dengan ID spreadsheet Anda)
const char spreadsheetId[] = "1f4SmCgEJ2cXqdvmQB5Y0xIuDmoRsbl1pIJvQp30Csnc";

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