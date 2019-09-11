#include <WireData.h>

#include <DS3232RTC.h>
#include <SFE_MicroOLED.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

//////////////////////////
// MicroOLED Definition //
//////////////////////////
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 9  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 0 

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration

RF24 radio(9,10); // CE, CSN
byte addresses[][6] = {"1Node","2Node"};

struct data{
    char idmodule;
    char typemsg;
    unsigned int nummsg;
    int HSol; // just counter to see if receiving new data
    float t12;
    float h12;
};
struct data msgradio;

void setup() {
  //debug
  Serial.begin(115200);

  //pinMode(LED, OUTPUT );

  //radio
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(5,15);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(32);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  //OLED
  delay(100);
  Wire.begin();
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
}
void loop() {
  if (radio.available())
  {
    
    while (radio.available())
	    {                                   // While there is data ready
	        radio.read(&msgradio, sizeof(msgradio));            // Get the payload
	    }
       

    
    oled.setFontType(1);  // Set font to type 0
    oled.clear(PAGE);     // Clear the screen
    oled.setCursor(0, 0); // Set cursor to top-left


    Serial.print("compteur de messageRadio : ");
        oled.print("cptr msgRadio : ");
    oled.print(int(msgradio.nummsg));  // Write a byte out as a character
    oled.display(); // Draw on the screen
    delay(500);


    Serial.println(int(msgradio.nummsg));
    Serial.print("identifiant du module : ");
    Serial.println(int(msgradio.idmodule));
    Serial.print("type de messageRadio : ");
    Serial.println(int(msgradio.typemsg));
    

    Serial.print("temp: ");
    Serial.println(msgradio.t12);// affiche temperature
    
    
    Serial.print("humidite de l'air: ");
    Serial.println(msgradio.h12);// affiche temperature
    

    Serial.print("humidite du sol : ");
    Serial.println(msgradio.HSol);
    Serial.println();
}

}