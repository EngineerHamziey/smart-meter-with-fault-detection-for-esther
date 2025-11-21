#if 0

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD address may vary: 0x27 or 0x3F are common
LiquidCrystal_I2C lcd(0x27, 16, 2); 
void initLCD() {
  lcd.init();           // initialize the LCD
  lcd.backlight();      // turn on backlight
}
int displayDelay = 1500;
String projectTitle = "SMART METER WITH FAULT DETECTION   "; // add spaces for smooth scroll
int scrollSpeed = 300; // delay in ms



// firebase relayted
#define databaseReferenceUrl "https://smart-power-weather-prediction-default-rtdb.europe-west1.firebasedatabase.app/"
#define databaseApiKey "AIzaSyDnYHiwaaldFoJNM72sJSxkpkFdBz_kHOU"
bool signupSuccessful = false;
#include <WiFi.h> //to connect esp32 to the internet
#include <Firebase_ESP_Client.h> //to interface esp32 with firebase
#include "addons/TokenHelper.h" // for token generation
#include "addons/RTDBHelper.h" // for token RTDB payload printing and other helper functions


// we need to create 3 firebase object that we need
FirebaseData fbdo; // this handle the data when there is a change on a specific database node path
FirebaseAuth auth; // need for authentication
FirebaseConfig config; // needed for configuration

bool isConnect = true;


// network credentials
#define wifiSsid "project09"
#define wifiPassword "password"


// ------------------ Sensor Pin Assignments ------------------
#define humidSensorPin        32
#define rainSensorPin         25
#define buzzerPin 13
#define soil_moisture_sensorPin 33

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setPinMode() {
  // Configure analog input pins
  pinMode(humidSensorPin, INPUT);
  pinMode(rainSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

// Function to turn buzzer ON with a frequency (e.g., 1000 Hz)
void buzzerOn() {
  digitalWrite(buzzerPin, HIGH);
}

// Function to turn buzzer OFF
void buzzerOff() {
  digitalWrite(buzzerPin, LOW);
}

void scrollMsg(String message) {
  for (int i = 0; i < message.length() - 15; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message.substring(i, i + 16));
    delay(scrollSpeed);
  }
}

float temperature = 0.0;

void configureTimeUsingNtp() { 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Time synchronization complete");
}

void initializeWifiAndFirebase() {
  WiFi.begin(wifiSsid, "passwording");
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 30000; // 1 minute = 60,000 ms

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    Serial.println("connecting to wifi.....");
    lcd.clear();
    lcd.print("WIFI CONNECTING");
    lcd.setCursor(0, 1);
    lcd.print("................");
    delay(300);
  }

  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to wifi named love");
    lcd.clear();
    lcd.print(" WIFI");
    lcd.setCursor(0, 1);
    lcd.print("     CONNECTED");
    delay(displayDelay);
    lcd.clear();
    lcd.print("DATABASE");
    lcd.setCursor(0, 1);
    lcd.print("  CONNECTING");

    
    initializeFirebase();
  } else {
      Serial.println("WiFi connection failed.");
      lcd.clear();
      lcd.print("WIFI FAILED");
      isConnect = false;
  }
}

void initializeFirebase() {
  config.api_key = databaseApiKey;
  config.database_url = databaseReferenceUrl;
  // call the firebase signup method using the previously defined config and the firebaseAuth objects
  if(Firebase.signUp(&config, &auth, "", "")) { // the last two arguements are empty strings denoting an anonymous user signup
    // in this anonymous signup method, everytime the esp32 connects, it create a new anonymous user.
    Serial.println("signup successful");
    lcd.clear();
    lcd.print("CONNECTED TO");
    lcd.setCursor(0, 1);
    lcd.print("      DATABASE");
    delay(displayDelay);
    signupSuccessful = true;
  } else {
    Serial.println("Sign up error, with message: ");
    // Serial.printf("%s\n", config.signer.signupError.message.c_str());
    Serial.println(fbdo.errorReason()); // this fixes the compatibility error as it replaces the line above
  }
  // assign the callback function to the long running token generation task check addOns tokenhelper.h for more info
  config.token_status_callback = tokenStatusCallback;

  // now, we intialize and begin the firebase library with the same config and auth we defined earlier
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  configureTimeUsingNtp();
}


void blinkBuzzer(int duration = 100) {
  for (int i = 0; i < 3; i++) {
    buzzerOn();
    delay(duration);
    buzzerOff();
    delay(duration);
  }
}


byte lastTimeDataSent = 0; // 60 seconds max
byte dataTransferInterval = 5;//in seconds
String year = "", month = "", day = "", hour = "", minute = "", second = "";

// ------------------ Setup ------------------
void setup() {
  initLCD();
  // scrollMsg(projectTitle);
  Serial.begin(115200);
  delay(1000); // always wait a little after setting baud rate

  Serial.println("Initializing Sensors...");

  setPinMode();
  Serial.println("Sensors Ready");

  sensors.begin();
  
  initializeWifiAndFirebase();
  setPinMode();
  blinkBuzzer(300);
}

// ------------------ Loop ------------------
void loop() {
  // Read analog values (0–4095 range on ESP32)
  int humidRaw       = analogRead(humidSensorPin);
  bool isRain        = digitalRead(rainSensorPin);

  // Read temperature from DS18B20
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  float humidity = map(humidRaw, 0, 4095, 0, 100);
  humidity = constrain(humidity, 0, 100);
  humidity = random(50, 70);
  int soilMoisture = analogRead(soil_moisture_sensorPin);
  String soilMoistureState = "";
  

  if (soilMoisture >= 4000) {
    Serial.println(" => Soil is Dry");
    soilMoistureState = "Bad";
  } else {
    Serial.println(" => Soil is Wet");
    soilMoistureState = "Good";
  }


  if (isConnect == true) {
    sendDataAtRegularIntertal(humidity, isRain, soilMoistureState, soilMoisture);
  }

  Serial.println("---- Sensor Readings ----");
  Serial.printf("Temperature:               %.2f °C\n", temperature);
  Serial.printf("Rain Status:               %s\n", isRain ? "RAIN FALLING" : "NO RAIN");
  Serial.println("--------------------------\n");
  
  
  // 1. Humidity
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity:");
  lcd.setCursor(0, 1);
  lcd.print(humidity);
  lcd.print("%");
  delay(displayDelay);


  // 2. Temperature
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.print("C  ");
  delay(displayDelay);

  // 3. Rain
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rain:");
  lcd.setCursor(0, 1);
  lcd.print(isRain ? "RAIN FALLING" : "NO RAIN");
  delay(displayDelay);



  // 4. Soil moisture
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SOIL MOISTURE:");
  lcd.setCursor(0, 1);
  lcd.print(soilMoisture);
  delay(displayDelay);
}

int getSecondsInt() {
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {//update time
    Serial.println("Failed to obtain time");
    // maybe restart the esp32
    return 0;
  }
  return timeInfo.tm_sec;
}


// since padStart doesn't exist in Arduino like it does in JS
String padStart(String input, unsigned int length, char padChar = '0') {
  while (input.length() < length) {
    input = padChar + input;
  }
  return input;
}


void getCurrentTime() {
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  year = String(timeInfo.tm_year + 1900); // tm_year is years since 1900
  // TO ensure that day 5 comes before day 20, etc, we'll add .padStart
  month = padStart( String(timeInfo.tm_mon + 1), 2 ); // Ensure two digits
  day = padStart( String(timeInfo.tm_mday), 2 );      // Ensure two digits
  hour = padStart( String(timeInfo.tm_hour), 2 );     // Ensure two digits
  minute = padStart( String(timeInfo.tm_min), 2 );    // Ensure two digits
  second = padStart( String(timeInfo.tm_sec), 2 );    // Ensure two digits
  // with this we'll have 01, 02, 03, ... and the order in RTDB will be correct that way
}


//  soilMoistureState, soilMoisture, 
void saveSensorData(int humidity, bool isRain, String soilMoistureState, int soilMoisture) {
  // "2024-12-17T16:10:17Z" date-time format is a part of ISO-8601 date representation.
  String pathTime = year + "-" + month + "-" + day + "T" + hour + ":" + minute + ":" + second + "Z";
  
  // ===xxxxxxxxxxxxxxxxxx
  String sensorDataPath = "sensorReadings/" + pathTime + "/humidity";

  if (Firebase.RTDB.setFloat(&fbdo,  sensorDataPath, humidity)) {//if the node path doesn't exist, it will automatically create it
    Serial.println("\n " + String(humidity) + " saved successfully to " + fbdo.dataPath() + "\nas type : " + fbdo.dataType());
  } else {
    Serial.println("Failed to save to firebase: " + fbdo.errorReason());
    blinkBuzzer(300);
  }

  sensorDataPath = "sensorReadings/" + pathTime + "/isRain";

  if (Firebase.RTDB.setBool(&fbdo,  sensorDataPath, isRain)) {//if the node path doesn't exist, it will automatically create it
    Serial.println("\n " + String(isRain) + " saved successfully to " + fbdo.dataPath() + "\nas type : " + fbdo.dataType());
    lcd.clear();
    lcd.print("PROJECT NOW");
    lcd.setCursor(0, 1);
    lcd.print("    ONLINE");
    delay(displayDelay);
  } else {
    Serial.println("Failed to save to firebase: " + fbdo.errorReason());
    blinkBuzzer(300);
    // scrollMsg("DATABASE CONNECTION FAILED, RESTART THE SYSTEM");
  }

  //  soilMoisture
  sensorDataPath = "sensorReadings/" + pathTime + "/soilMoistureState";
  if (Firebase.RTDB.setString(&fbdo,  sensorDataPath, soilMoistureState)) {//if the node path doesn't exist, it will automatically create it
    Serial.println("\n " + soilMoistureState + " saved successfully to " + fbdo.dataPath() + "\nas type : " + fbdo.dataType());
    lcd.clear();
    lcd.print("PROJECT NOW");
    lcd.setCursor(0, 1);
    lcd.print("    ONLINE");
    delay(displayDelay);
  } else {
    Serial.println("Failed to save to firebase: " + fbdo.errorReason());
    blinkBuzzer(300);
    // scrollMsg("DATABASE CONNECTION FAILED, RESTART THE SYSTEM");
  }

  //  soilMoisture
  sensorDataPath = "sensorReadings/" + pathTime + "/soilMoisture";
  if (Firebase.RTDB.setInt(&fbdo,  sensorDataPath, soilMoisture)) {//if the node path doesn't exist, it will automatically create it
    Serial.println("\n " + String(soilMoisture) + " saved successfully to " + fbdo.dataPath() + "\nas type : " + fbdo.dataType());
    lcd.clear();
    lcd.print("PROJECT NOW");
    lcd.setCursor(0, 1);
    lcd.print("    ONLINE");
    delay(displayDelay);
  } else {
    Serial.println("Failed to save to firebase: " + fbdo.errorReason());
    blinkBuzzer(300);
    // scrollMsg("DATABASE CONNECTION FAILED, RESTART THE SYSTEM");
  }


  sensorDataPath = "sensorReadings/" + pathTime + "/temperature";
  if (Firebase.RTDB.setFloat(&fbdo,  sensorDataPath, temperature)) {//if the node path doesn't exist, it will automatically create it
    Serial.println("\n " + String(temperature) + " saved successfully to " + fbdo.dataPath() + "\nas type : " + fbdo.dataType());
    lcd.clear();
    lcd.print("PROJECT NOW");
    lcd.setCursor(0, 1);
    lcd.print("    ONLINE");
    delay(displayDelay);
  } else {
    Serial.println("Failed to save to firebase: " + fbdo.errorReason());
    blinkBuzzer(300);
    scrollMsg("DATABASE CONNECTION FAILED, RESTART THE SYSTEM");
  }

}



void sendDataToRtdb(int humidity, bool isRain, String soilMoistureState, int soilMoisture) {
  // all data should be gathered at the same time
  getCurrentTime();
  
  // the we need to: check if firebase is ready, if signup is successful and do it every 5s or at the start of this loop when lastTimeDatSent == 0
  if( Firebase.ready() && signupSuccessful ){
    saveSensorData(humidity, isRain,  soilMoistureState, soilMoisture);
  } 
}

void sendDataAtRegularIntertal(int humidity, bool isRain, String soilMoistureState, int soilMoisture) {
  int currentSecondsInt = getSecondsInt();
    if ((currentSecondsInt % dataTransferInterval == 0) && (currentSecondsInt != lastTimeDataSent)) {
      lastTimeDataSent = currentSecondsInt; // Update last sent time
      Serial.println("Data sent at: " + String(currentSecondsInt));
      sendDataToRtdb(humidity, isRain,  soilMoistureState, soilMoisture);
    }
}

#endif