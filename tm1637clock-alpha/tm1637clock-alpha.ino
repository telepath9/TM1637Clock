#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h> 
#include "RTClib.h"
#include "TM1637Display.h"

/*LIBRERIE USATE:
modulo RTC: https://github.com/adafruit/RTClib
NTP client: https://github.com/arduino-libraries/NTPClient/tree/master
display: https://github.com/avishorp/TM1637/blob/master/TM1637Display.h
*/
  //wemos d1 mini

//TM1637 DISPLAY SETUP
#define CLK 13
#define DIO 12

    TM1637Display mydisplay(CLK, DIO);
    byte brightness;
//END TM1637 DISPLAY SETUP


  //credenziali wifi
  const char *ssid     = "your_ssid";
  const char *password = "your_password";

  WiFiUDP ntpUDP;


 


// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

//specifico il time server pool e l'offset in secondi (3600=1h) 
  NTPClient timeClient(ntpUDP, "ntp1.inrim.it", 3600);


  RTC_DS1307 rtc;

void setup() {

    mydisplay.setBrightness(7);

    Serial.begin(115200);

//SETUP NTP
    WiFi.begin(ssid, password);
    //ciclo per la connessione
      while ( WiFi.status() != WL_CONNECTED ) {
           delay ( 500 );
           Serial.print( "." );
      }
    
    timeClient.begin();
//FINE SETUP NTP

  //estraggo HH , MM, SS da timeClient.getFormattedTime()
  /*timeClient.getHours() e timeClient.getMinutes()
  timeClient.getSeconds()  */


//SETUP RTC 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");

    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled

    //imposto nel modulo RTC la data e l'ora estratta dal server NTP
   // rtc.adjust(DateTime(2025, 3, 15, timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds()));

    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    //PS: __DATE__ e __TIME__ sono macros che sets the RTC to the date & time this sketch was compiled
  }

}



void loop() {

  DateTime adesso = rtc.now();
  timeClient.update(); //obbligatoria
  

  /*Serial.print("NTP says ->");
  Serial.print(timeClient.getHours(), DEC);
  Serial.print('-');
  Serial.print(timeClient.getMinutes());
  Serial.print('-');
  Serial.println(timeClient.getSeconds());*/
  
  rtc.adjust(DateTime(2025, 3, 15, timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds()));

  //posso anche spegnere il wifi

 

  //Serial.println(timeClient.getFormattedTime());

  mydisplay.showNumberDecEx(adesso.hour(), 0b01000000, false, 2, 0);
  mydisplay.showNumberDecEx(adesso.minute(), 0b01000000, true, 2, 2);
}
