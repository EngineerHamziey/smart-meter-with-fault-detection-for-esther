#if 0

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD (I2C address, columns, rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Function to scroll long text across the screen
void scrollText(String message, int delayTime) {
  int messageLength = message.length();

  // Scroll the message across the display
  for (int i = 0; i < messageLength - 15; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message.substring(i, i + 16)); // Display 16 characters at a time
    delay(delayTime);
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  scrollText("SMART POWER METER WITH FAULT DETECTION", 300);
  delay(1000); // Pause before repeating
}

#endif