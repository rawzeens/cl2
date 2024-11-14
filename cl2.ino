#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <MFRC522.h>      
#include <WiFiClient.h>  
#include <ESP8266HTTPClient.h>


const char *apSSID = "NodeMCU_AP";
const char *apPassword = "password";
ESP8266WebServer server(80);

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

#define RST_PIN D3
#define SS_PIN D4
MFRC522 mfrc522(SS_PIN, RST_PIN);

const int maxAttempts = 10;
int attempts = 0;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  WiFi.softAP(apSSID, apPassword);
  IPAddress apIP = WiFi.softAPIP();

  server.on("/", handleWiFiForm);
  server.on("/rfid-form", HTTP_GET, handleRFIDForm);
server.on("/submit-rfid", HTTP_POST, handleSubmitRFID); 
server.on("/rfid-scan", HTTP_GET, handleRFIDScan); 

  server.begin();

  retrieveWiFiCredentials();
  connectToWiFi();
}

void loop() {
  server.handleClient();
  if (!WiFi.isConnected()) {
    attempts++;
    if (attempts > maxAttempts) {
      startAPMode();
    } else {
      connectToWiFi();
    }
  }
}

void handleWiFiForm() {
  String html = "<html><body>";
  html += "<h1>Configure Wi-Fi</h1>";
  html += "<form method='get' action='/get'>";
  html += "SSID: <input type='text' name='ssid'><br>";
  html += "Password: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Submit'></form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void connectToWiFi() {
  String ssid = readEEPROM(0);
  String password = readEEPROM(ssid.length() + 1);

  WiFi.begin(ssid.c_str(), password.c_str());
  lcd.clear();
  lcd.print("Connecting to ");
  Serial.println("Connecting to ");
  lcd.print(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print(".");
  }

  lcd.clear();
  lcd.print("Connected!");
  Serial.println("Connected!");
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.print("IP:");
  lcd.print(WiFi.localIP());

  attempts = 0;
}

void startAPMode() {
  lcd.clear();
  lcd.print("Failed to connect");
  Serial.println("Failed to connect");
  lcd.setCursor(0, 1);
  lcd.print("Starting AP...");
  Serial.println("Starting AP...");
  WiFi.disconnect();
  delay(1000);

  server.close();
  WiFi.softAP(apSSID, apPassword);
}

void storeWiFiCredentials(String ssid, String password) {
  clearEEPROM();
  for (unsigned int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(i, ssid[i]);
  }
  EEPROM.write(ssid.length(), 0); 
  for (unsigned int i = 0; i < password.length(); ++i) {
    EEPROM.write(ssid.length() + 1 + i, password[i]);
  }
  EEPROM.commit();
}

void retrieveWiFiCredentials() {
  String ssid = readEEPROM(0);
  String password = readEEPROM(ssid.length() + 1);

  if (ssid != "" && password != "") {
    connectToWiFi();
  }
}

void clearEEPROM() {
  for (unsigned int i = 0; i < EEPROM.length(); ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

String readEEPROM(int start) {
  String value = "";
  char character;
  for (int i = start; i < EEPROM.length(); ++i) {
    character = EEPROM.read(i);
    if (character == 0) {
      break;
    }
    value += character;
  }
  return value;
}

void handleGet() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  storeWiFiCredentials(ssid, password);
  connectToWiFi();

  server.send(200, "text/plain", "Successfully connected to Wi-Fi");
}

void handleRFIDForm() {
  String html = "<html><body>";
  html += "<h1>Submit RFID Data</h1>";
  html += "<form method='post' action='/submit-rfid'>";
  html += "Gate: <input type='text' name='gate'><br>";
  html += "Airplane Location: <input type='text' name='location'><br>";
  html += "Luggage Name: <input type='text' name='name'><br>";
  html += "RFID: <input type='text' name='rfid' id='rfid' readonly><br>";
  html += "<input type='submit' value='Submit'></form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleSubmitRFID() {
    WiFiClient client;

  HTTPClient http; 
  
  String gate = server.arg("gate");
  String location = server.arg("location");
  String name = server.arg("name");
  String nric = server.arg("nric");
  String rfid = server.arg("rfid");

  String url = "/handleRFIDSubmit.php?";
  url += "gate=" + gate + "&";
  url += "location=" + location + "&";
  url += "name=" + name + "&";
  url += "nric=" + nric + "&";
  url += "rfid=" + rfid;

  http.begin(client, "http://r4-client.rf.gd/cl2");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void handleRFIDScan() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rfid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      rfid += String(mfrc522.uid.uidByte[i], HEX);
    }
    mfrc522.PICC_HaltA();

    server.send(200, "text/plain", rfid);
  } else {
    server.send(404, "text/plain", "No RFID tag detected");
  }
}
