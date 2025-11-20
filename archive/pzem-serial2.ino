#if 0

#include <PZEM004Tv30.h>
#include <HardwareSerial.h>
#include <Wire.h>

// Create UART2 manually
HardwareSerial PZEMSerial(2);   // Use UART2 controller

// Assign your own custom pins here:
#define PZEM_RX 27
#define PZEM_TX 26

PZEM004Tv30 pzem(PZEMSerial, PZEM_RX, PZEM_TX);
// PZEMSerial.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);

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
  

  // Initialize PZEM UART on your new pins
  // PZEMSerial.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
  // PZEM004Tv30 pzem(PZEMSerial, PZEM_RX, PZEM_TX);

  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  digitalWrite(32, HIGH); // relays test
  digitalWrite(33, HIGH); // relays test

  Serial.println("Custom UART serial for PZEM started...");
}

void loop() {
  calculateACParameters();
  Serial.println();
  delay(1000);
}

#endif