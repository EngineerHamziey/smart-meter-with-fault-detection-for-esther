#if 0
#include <PZEM004Tv30.h>
#include <Wire.h>

// For ESP32: PZEM on Serial2 (RX=16, TX=17)
PZEM004Tv30 pzem(Serial2, 16, 17);

void calculateACParameters() {
  float ACvoltage = pzem.voltage();
  if (isnan(ACvoltage)) ACvoltage = 0;
  Serial.print("Voltage: " + String(ACvoltage) + "V");

  float ACCurrent = pzem.current();
  if (isnan(ACCurrent)) ACCurrent = 0;
  Serial.print("\tCurrent: " + String(ACCurrent) + "A");

  float power = pzem.power();
  if (isnan(power)) power = 0;
  Serial.print("\tPower: " + String(power) + "W");

  float energy = pzem.energy();
  if (isnan(energy)) energy = 0;
  Serial.print("\tEnergy: " + String(energy, 3) + "kWh");

  float frequency = pzem.frequency();
  if (isnan(frequency)) frequency = 0;
  Serial.print("\tFrequency: " + String(frequency, 1) + "Hz");

  float pf = pzem.pf();
  if (isnan(pf)) pf = 0;
  Serial.print("\tPF: " + String(pf));
}

void setup() {
  Serial.begin(115200);

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);  // Power ON PZEM board if applicable
}

void loop() {
  calculateACParameters();
  Serial.println();
  delay(1000);
}

#endif
