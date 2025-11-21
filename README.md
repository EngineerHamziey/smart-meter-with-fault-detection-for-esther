/*
UART
The ESP32 supports up to three UART interfaces: UART0, UART1, and UART2, depending on the ESP32 board model you’re using.

UART0 is usually reserved for communication with the serial monitor during upload and debugging. However, you can also use it for communication with other devices after uploading the code if the Serial Monitor is not needed.
UART1 and UART2: available to communicate with external devices.
Like I2C and SPI, these UART pins can be mapped to any GPIO pin on the ESP32. However, they have a default pin assignment on most board models.

For most ESP32 boards the UART pin assignment is as follows:

UART Port	TX	RX	Remarks
UART0	GPIO 1	GPIO 3	Used for Serial Monitor and uploading code; Can be assigned to other GPIOs;
UART1	GPIO 10	GPIO 9	Must be assigned to other GPIOs
UART2	GPIO 17	GPIO 16	Can be assigned to other GPIOs


// Create UART2 manually
HardwareSerial PZEMSerial(2);   // Use UART2 controller

// Assign your own custom pins here:
#define PZEM_RX 4
#define PZEM_TX 15

PZEM004Tv30 pzem(PZEMSerial);
*/