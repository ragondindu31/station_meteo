#include <nRF24L01.h>
#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>

/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/




RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  Serial.println("11111");
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
  Serial.println(text);
}