#if 0

#include <PZEM004Tv30.h>
#include <HardwareSerial.h>
#include <Wire.h>

// ================= UART DEFINITIONS =================
// UART2 for PZEM #1
HardwareSerial PZEMSerial1(2);
#define PZEM1_RX 27
#define PZEM1_TX 26

// UART1 for PZEM #2
HardwareSerial PZEMSerial2(1);
#define PZEM2_RX 16
#define PZEM2_TX 17

// ================= GLOBAL PZEM OBJECTS =================
PZEM004Tv30 pzem1(PZEMSerial1, PZEM1_RX, PZEM1_TX);
PZEM004Tv30 pzem2(PZEMSerial2, PZEM2_RX, PZEM2_TX);


// ================= SIMPLE READ FUNCTIONS =================
void readPZEM1() {
    float v = pzem1.voltage();   if (isnan(v)) v = 0;
    float c = pzem1.current();   if (isnan(c)) c = 0;
    float p = pzem1.power();     if (isnan(p)) p = 0;
    float e = pzem1.energy();    if (isnan(e)) e = 0;
    float f = pzem1.frequency(); if (isnan(f)) f = 0;
    float pf = pzem1.pf();       if (isnan(pf)) pf = 0;

    Serial.print("[PZEM1] V: "); Serial.print(v);
    Serial.print(" I: "); Serial.print(c);
    Serial.print(" P: "); Serial.print(p);
    Serial.print(" E: "); Serial.print(e, 3);
    Serial.print(" F: "); Serial.print(f);
    Serial.print(" PF: "); Serial.println(pf);
}

void readPZEM2() {
    float v = pzem2.voltage();   if (isnan(v)) v = 0;
    float c = pzem2.current();   if (isnan(c)) c = 0;
    float p = pzem2.power();     if (isnan(p)) p = 0;
    float e = pzem2.energy();    if (isnan(e)) e = 0;
    float f = pzem2.frequency(); if (isnan(f)) f = 0;
    float pf = pzem2.pf();       if (isnan(pf)) pf = 0;

    Serial.print("[PZEM2] V: "); Serial.print(v);
    Serial.print(" I: "); Serial.print(c);
    Serial.print(" P: "); Serial.print(p);
    Serial.print(" E: "); Serial.print(e, 3);
    Serial.print(" F: "); Serial.print(f);
    Serial.print(" PF: "); Serial.println(pf);
}


// ================= SETUP =================
void setup() {
    Serial.begin(115200);

    // Start UART2 for PZEM1
    PZEMSerial1.begin(9600, SERIAL_8N1, PZEM1_RX, PZEM1_TX);

    // Start UART1 for PZEM2
    PZEMSerial2.begin(9600, SERIAL_8N1, PZEM2_RX, PZEM2_TX);

    Serial.println("Dual PZEM system initialized!");
}


// ================= LOOP =================
void loop() {
    readPZEM1();
    readPZEM2();
    Serial.println("------------------------------");
    delay(1000);
}

#endif