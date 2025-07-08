#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h> 
#include "RTClib.h"
#include "TM1637Display.h"


/* Libraries used:
RTC module: https://github.com/adafruit/RTClib
NTP client: https://github.com/arduino-libraries/NTPClient/tree/master
display: https://github.com/avishorp/TM1637/blob/master/TM1637Display.h
*/

//wemos d1 mini (clone)

    // NON-blocking timer function (delay() is EVIL)
    int myTimer(int everywhen){

      static unsigned long t1, diff_time;
      int ret=0;

      diff_time= millis() - t1;

      if(diff_time >= everywhen){

        t1= millis();
        ret=1;
      }
      return ret;
    }




//TM1637 DISPLAY SETUP
#define CLK 13
#define DIO 12

    TM1637Display mydisplay(CLK, DIO);
    byte brightness;
//END TM1637 DISPLAY SETUP


  //wifi credentials
  const char *ssid     = "your_ssid";
  const char *password = "your_passw";


  WiFiUDP ntpUDP;  //NTP server uses UDP communication protocol

// You can specify the time server pool and the offset, (in seconds, es:(3600=1h))
// additionally you can specify the update interval (in milliseconds).
  NTPClient timeClient(ntpUDP, "ntp1.inrim.it", 7200);

  RTC_DS1307 rtc;

void setup() {
    
    Serial.begin(115200);
  
    mydisplay.setBrightness(7); //display brightness
  
    pinMode(14, INPUT_PULLUP);

    
//SETUP NTP
    WiFi.begin(ssid, password);
    mydisplay.clear();
    int ct=4;   //counter used for waiting-4-wifi animation

    //cycle that waits for connection
      while (WiFi.status() != WL_CONNECTED ) {
          
           //setSegments(const uint8_t segments[], uint8_t length = 1, uint8_t pos = 0); i'll use for an other animation
  
           if(myTimer(300)){

                  //animation code
                  if(ct<4 && ct>-1){
                  mydisplay.showNumberDecEx(0, 0, false, 1, ct);
                  Serial.println( "Trying to connect. Press button 2 skip 2 RTC" );
                  ct--;
                  //Serial.println("ct FROM 3 TO 0");
                  }

                  // clear() needs to be called in order to refresh the display 
                  else if(ct==4){
                    mydisplay.clear();
                    //Serial.println("ct==4" );
                    ct--;
                  }

                  //resetting ct counter
                  else if(ct==-1){
                  ct=4;      
                  //Serial.println("ct== -1");
                  }

            }


              //when button is pressed, skips directly to RTC MODE
              if(digitalRead(14) == LOW){  
                  break;
              }     
      }
    
      if(WiFi.status() == WL_CONNECTED){
          Serial.println("connected!");
          timeClient.begin();
          timeClient.update();
      }
//END SETUP NTP

 
//SETUP RTC 

  //this checks if you wired the RTC module the right way
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  //checks if RTC hasn't data in it yet
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");

    // When time needs to be set on a new device, or after a power loss,
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    rtc.adjust(DateTime(2025, 3, 20, timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds()));
    //PS: __DATE__ e __TIME__ are macros that sets the RTC to the date & time this sketch was compiled
  }

}


void loop() {

  //NTP Mode
  if(WiFi.status() == WL_CONNECTED){
  
  timeClient.update(); //mandatory
 
  mydisplay.showNumberDecEx(timeClient.getHours(), 0b01000000, true, 2, 0);
  mydisplay.showNumberDecEx(timeClient.getMinutes(), 0b01000000, true, 2, 2);
  }

    
  //RTC Mode (when there's no wifi)
  else{
  DateTime adesso = rtc.now();
 
  mydisplay.showNumberDecEx(adesso.hour(), 0b01000000, true, 2, 0);
  mydisplay.showNumberDecEx(adesso.minute(), 0b01000000, true, 2, 2);
     }
  

}

   
