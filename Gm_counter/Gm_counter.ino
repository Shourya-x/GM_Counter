#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

#define integratingTime 15000
#define oneMinute 60000

unsigned long counts = 0;     //variable for GM Tube events
unsigned long events = 0; 
unsigned long cpm = 0;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement
double avgCounts = 0;
double avgCPM = 0;
double avgUSV = 0;
double sumCPM = 0;
double sumUSV = 0;
double uSv = 0;
double dose = 0;

const int ledPin =  9;

void tube_impulse() //subprocedure for capturing events from GM board
{       
  counts++;
  events++;
}



void setup() {
  display.begin();
  display.setContrast(60);
  display.display();
  display.clearDisplay();
  
  counts = 0;
  cpm = 0;
  multiplier = oneMinute / integratingTime;      //calculating multiplier, depend on your log period
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), tube_impulse, FALLING); //define external interrupts 
  pinMode(ledPin, OUTPUT);
  Serial.println("GM Tube - Send 's' to start...");
 

}

void loop() {


 if (Serial.available() > 0) {
      /* display.setTextSize(1);//row 5
       display.setTextColor(BLACK);
       display.setCursor(0, 32);
       display.println("counts");//counts
       display.println(events);// events
       display.display();
       //display.clearDisplay();*/

   char cin = Serial.read();
    if(cin = 's'){
     while(Serial.read() != 'N') {
      display.setTextSize(1);//row 5
       display.setTextColor(BLACK);
       display.setCursor(0, 0);
       display.println("counts");//counts
       display.println(events);// events
       display.display();
       //display.clearDisplay();

       unsigned long currentMillis = millis();
                
                if(currentMillis - previousMillis > integratingTime){
                  
                  avgCounts++;
                  previousMillis = currentMillis;
                  cpm = counts * multiplier; //cpm * 60/15 = cpm * 4
 
                  uSv =( cpm  / 151.0);
               
                
                  sumCPM = (double)cpm + sumCPM;
                  sumUSV = uSv + sumUSV;
              
                  avgCPM = sumCPM / avgCounts;
                  avgUSV = sumUSV / avgCounts;                
                
                 Serial.print((currentMillis)/1000);
                 Serial.print("\t");
                 Serial.print(cpm);
                 Serial.print("\t");
                 Serial.print(avgCPM);
                 Serial.print("\n");
                 
                  display.setTextSize(1);//row 5
                  display.setTextColor(BLACK);
                  display.setCursor(0, 16);
                  display.println("S CPM   uSv/hr");
                  display.display();
                  //display.clearDisplay()  ;

                  /*display.setCursor(0, 1);
                  display.setTextSize(1);
                  display.setTextColor(BLACK);
                  display.println("N ");
                  display.println(cpm);
                  display.println("  ");
                  display.println(uSv,4);
                  display.display();

                  display.setCursor(0,2);
                  display.println("A ");
                  display.println(avgCPM ,1);
                  display.println(" ");
                  display.println(avgUSV,4);*/


                  counts = 0 ;

   
                }
            }
            }
        
      
 }
}