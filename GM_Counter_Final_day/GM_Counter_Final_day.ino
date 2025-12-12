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

// Center text helper
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
  display.begin();
  display.setContrast(33);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  // -------- Boot screen exactly as requested --------
  centeredPrint("Geiger-Muller", 0);    // row 1
  centeredPrint("Counter", 8);         // row 2
  centeredPrint("Ready For", 16); // row 3
  centeredPrint("Radiation", 24);     // row 4
  centeredPrint("Detection", 32);     // row 5
  display.display();
  delay(2500);

  counts = 0;
  cpm = 0.00;
  multiplier = oneMinute / integratingTime;
  previousMillis = millis();

  // UNCOMMENT THE LINE NUMBER 61 IF EVERY THING WORKS FINE , IF NOT COMMENT THE LINE AND UNCOMMENT THE LINE NUMBER 62
  
  pinMode(geigerPin, INPUT);
  //pinMode(geigerPin, INPUT_PULLUP);
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

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);

    // Row 1: GM Counter title
    centeredPrint("GM Counter", 0);

    // Row 2: header
    display.setCursor(0, 8);
    display.print("CPM     uSv/h");

    // Row 3: N line (current)
    display.setCursor(0, 16);
    display.print("N ");
    display.print(cpm, 2);        // e.g. 44.25
    display.setCursor(48, 16);    // fixed column for uSv/h
    display.print(uSv, 4);        // e.g. 0.0728

    // Row 4: A line (average)
    display.setCursor(0, 24);
    display.print("A ");
    display.print(avgCPM, 2);     // e.g. 55.27
    display.setCursor(48, 24);
    display.print(avgUSV, 4);     // e.g. 0.0915

    // Row 5: counts
    display.setCursor(0, 36);
    display.print("Counts: ");
    display.print(events);

    display.display();

        // ----- Serial logging -----
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
    Serial.println(events);
    
    counts = 0;
  }
}
