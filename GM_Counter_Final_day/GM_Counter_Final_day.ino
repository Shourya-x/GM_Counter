#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SPI.h>

// Pin mapping: CLK, DIN, DC, CE, RST
Adafruit_PCD8544 display(D1, D2, D3, D4, D6);
const int geigerPin = D5;

#define integratingTime 15000
#define oneMinute 60000

volatile unsigned long counts = 0;
unsigned long events = 0;
double cpm = 0;
unsigned int multiplier;
unsigned long previousMillis;

double avgCounts = 0;
double avgCPM = 0;
double avgUSV = 0;
double sumCPM = 0;
double sumUSV = 0;
double uSv = 0;

void centeredPrint(const char* text, int y) {
  int len = strlen(text);
  int x = (84 - (len * 6)) / 2;
  display.setCursor(x, y);
  display.print(text);
}

void ICACHE_RAM_ATTR tube_impulse() {
  counts++;
  events++;
}

void setup() {
  Serial.begin(115200);   // <--- important for Serial Plotter
  delay(200);

  display.begin();
  display.setContrast(33);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  centeredPrint("Geiger-Muller", 0);
  centeredPrint("Counter", 8);
  centeredPrint("Ready For", 16);
  centeredPrint("Radiation", 24);
  centeredPrint("Detection", 32);
  display.display();
  delay(2500);

  counts = 0;
  cpm = 0.00;
  multiplier = oneMinute / integratingTime;
  previousMillis = millis();

  pinMode(geigerPin, INPUT);
  // pinMode(geigerPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(geigerPin), tube_impulse, FALLING);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > integratingTime) {
    avgCounts++;
    previousMillis = currentMillis;

    cpm = (double)counts * multiplier;
    uSv = cpm / 151.0;

    sumCPM += cpm;
    sumUSV += uSv;
    avgCPM = sumCPM / avgCounts;
    avgUSV = sumUSV / avgCounts;

    // ---------- Display ----------
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);

    centeredPrint("GM Counter", 0);

    display.setCursor(0, 8);
    display.print("CPM     uSv/h");

    display.setCursor(0, 16);
    display.print("N ");
    display.print(cpm, 2);
    display.setCursor(48, 16);
    display.print(uSv, 4);

    display.setCursor(0, 24);
    display.print("A ");
    display.print(avgCPM, 2);
    display.setCursor(48, 24);
    display.print(avgUSV, 4);

    display.setCursor(0, 36);
    display.print("Counts: ");
    display.print(events);

    display.display();

    // ---------- Serial Plotter ----------
    // One line per time-step, values separated by commas:
    // time, cpm, uSv, avgCPM, avgUSV, events
    Serial.print(currentMillis);
    Serial.print(",");
    Serial.print(cpm, 2);
    Serial.print(",");
    Serial.print(uSv, 4);
    Serial.print(",");
    Serial.print(avgCPM, 2);
    Serial.print(",");
    Serial.print(avgUSV, 4);
    Serial.print(",");
    Serial.println(events);   // println ends the line -> new plot sample

    counts = 0;
  }
}
