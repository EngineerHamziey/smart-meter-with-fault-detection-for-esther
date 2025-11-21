#if 0
// the math library is used to enable the check for "isnan"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PZEM004Tv30.h>

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
// #define projectTitle "SMART POWER METER WITH IOT APP"



PZEM004Tv30 pzem(Serial2, 16, 17); // pin 16 = esp32 RX, pin 17 = esp32 tx


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
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
//   scrollMsg(projectTitle);
  
  // Start the serial communication
  Serial.begin(115200);  
  Serial.println("BEGUN");
}


void loop() {
  
  calculateACParameters();
  
  delay(3000);// to slow things down a little 
}
#endif