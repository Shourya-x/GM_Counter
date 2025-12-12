#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Pin mapping: CLK, DIN, DC, CE, RST
Adafruit_PCD8544 display(D1, D2, D3, D4, D6);

void setup() {
  display.begin();
  display.setContrast(33);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Hello, ESP8266!");
  display.display();
}

void loop() {
  // Nothing needed here for test
}
