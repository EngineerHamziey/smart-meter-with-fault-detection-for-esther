#if 1
// the math library is used to enable the check for "isnan"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

// Replace these with your Wi-Fi credentials
const char* ssid = "Smart_Power_meter_with_IOT_App";
const char* password = "123456789";

LiquidCrystal_I2C lcd(0x27, 16, 2); 

double
    voltageValue = 0.00,
    currentValue = 0.00,
    powerFactorValue = 0.00,
    frequencyValue = 0.00,
    powerValue = 0.00,
    energyValue = 0.00
;

#define relay1 32
#define relay2 33
#define displayDelay 1000
#define scrollSpeed 300
#define projectTitle "SMART POWER METER WITH IOT APP"



PZEM004Tv30 pzem(Serial2, 16, 17); // pin 16 = esp32 RX, pin 17 = esp32 tx


// WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);


void scrollMsg(String message) {
  for (int i = 0; i < message.length() - 15; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message.substring(i, i + 16));
    delay(scrollSpeed);
  }
}

void initLcd() {
  lcd.init();              // Initialize the LCD
  lcd.backlight();         // Turn on the backlight
  lcd.setCursor(0, 0);
}

void calculateACParameters() {
  voltageValue = pzem.voltage();
  voltageValue = (isnan(voltageValue)) ? 0.00 : voltageValue; // Validating the voltage value
  Serial.print("Voltage: " + String(voltageValue) + "V");

  currentValue = pzem.current();
  currentValue = (isnan(currentValue)) ? 0.00 : currentValue; // Validating the voltage value
  Serial.print("\tCurrent: " + String(currentValue) + "A");
  
  String col_1_values = "V ";
  voltageValue = (int)voltageValue;
  col_1_values += voltageValue;
  col_1_values += "V I ";
  col_1_values += currentValue;
  col_1_values += "A";

  powerValue = pzem.power();
  powerValue = (isnan(powerValue)) ? 0.00 : powerValue;
  Serial.print("\tPower: " + String(powerValue) + "W");

  energyValue = pzem.energy();
  energyValue = (isnan(energyValue)) ? 0.00 : energyValue;
  Serial.print("\tEnergy: " + String(energyValue, 3) + "kWh");

  String col_2_values = "P ";
  col_2_values += powerValue;
  col_2_values += "W E";
  col_2_values += energyValue;
  col_2_values += "KWH";

  frequencyValue = pzem.frequency();
  frequencyValue = (isnan(frequencyValue)) ? 0.00 : frequencyValue;
  Serial.print("\tFrequency: " + String(frequencyValue, 1) + "Hz");

  powerFactorValue = pzem.pf();
  powerFactorValue = (isnan(powerFactorValue)) ? 0.00 : powerFactorValue;
  Serial.println("\tPF: " + String(powerFactorValue));

  
  lcd.clear();
  lcd.print(col_1_values);
  // lcd.print("    APP NOW");
  lcd.setCursor(0, 1);
  lcd.print(col_2_values);
  // lcd.print("     ONLINE");
  // delay(displayDelay);
}

void setPinModes() {
  pinMode(relay2, OUTPUT); 
  pinMode(relay1, OUTPUT); 
}


void setup() {
  initLcd();
  setPinModes();
  scrollMsg(projectTitle);
  
  // Start the serial communication
  Serial.begin(115200);  
  
  // Create a WIfi with name ssid and password password
  WiFi.softAP(ssid, password);
  lcd.clear();
  lcd.print("WIFI NOW ON");
  lcd.setCursor(0, 1);
  lcd.print("READY TO CONNECT");
  delay(displayDelay);

  Serial.println("Wifi On");
  
  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Print the IP address
  Serial.print("WebSocket server started at ws://");
  Serial.print(WiFi.softAPIP());
  Serial.println(":81");
}


void loop() {
  // Handle WebSocket events
  webSocket.loop();
  
  // using a R"rawliteral string here to avoid using espcape sequences everywhere AND snprintf to make it dynamic, i.e accept variable.
  // not putting double quotes on the %.2f makes it a number type not string, in the resulting json
  calculateACParameters();
  
  String jsonPayload = "{";
  jsonPayload += "\"voltageValue\": " + String(voltageValue, 2) + ",";
  jsonPayload += "\"currentValue\": " + String(currentValue, 2) + ",";
  jsonPayload += "\"powerFactorValue\": " + String(powerFactorValue, 2) + ",";
  jsonPayload += "\"frequencyValue\": " + String(frequencyValue, 2) + ",";
  jsonPayload += "\"powerValue\": " + String(powerValue, 2) + ",";
  jsonPayload += "\"energyValue\": " + String(energyValue, 2);
  jsonPayload += "}";


  webSocket.broadcastTXT(jsonPayload);
  delay(1500);// to slow things down a little 
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      {
        // Client connected
        String message = "Client connected: " + WiFi.softAPIP().toString();
        Serial.println(message);
        webSocket.sendTXT(num, "Connected to WebSocket server");
      }
      break;

    case WStype_DISCONNECTED:
      {
        // Client disconnected
        String message = "Client disconnected";
        Serial.println(message);
      }
      break;

    case WStype_TEXT:
      {
        // Incoming message from client
        String message = String((char*)payload);
        Serial.println("Received message: " + message);
        char socketNo = message.charAt(0);
        char newState = message.charAt(1);
        if (socketNo == '1') {
          if (newState == '1') {
            digitalWrite(relay1, HIGH);
          } else {
            digitalWrite(relay1, LOW);
          }
        }else {
          if (newState == '1') {
            digitalWrite(relay2, HIGH);
          } else {
            digitalWrite(relay2, LOW);
          }
        }
        // webSocket.sendTXT(num, "Message received: " + message);
      }
      break;

    case WStype_BIN:
      // Handle binary messages (optional)
      break;
  }
}

#endif