
#include <MirfHardwareSpi85Driver.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <Mirf_nRF24L01.h>

#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
 
 
#define sensorTrigPin    4
#define sensorEchoPin    2
 long distance;
 long time;
 int cdeID1 = 2;
  
void setup()
{
 
  Mirf.cePin = 9;
  Mirf.csnPin = 10;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
 
  Mirf.channel = 1;
  Mirf.payload = 32;
  Mirf.config();
 
  Mirf.setTADDR((byte *) "nrf02"); //Adresse a qui on envoie
  Mirf.setRADDR((byte *) "nrf01"); //Adresse du module
 
    Serial.begin(9600);
   
    pinMode(sensorTrigPin, OUTPUT);
    pinMode(sensorEchoPin, INPUT);
}
 
 
void loop(){
    digitalWrite(4, HIGH);  
    delayMicroseconds(10);  
    digitalWrite(4, LOW);   
                    
                                                
    time = pulseIn(2, HIGH);
                                            
    distance = int(0.017*time);             
 
  Serial.println(distance);
  if(distance < 50)
  {
 
   Mirf.send((byte *) &cdeID1); // On envoie le message
  while(Mirf.isSending());
  Serial.println("Done");
  Serial.println(cdeID1);
 }
   
   delay(400);
   
}