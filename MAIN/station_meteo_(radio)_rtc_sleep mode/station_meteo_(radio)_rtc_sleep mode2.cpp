#include <Streaming.h>
#include <DS3232RTC.h>
#include <Arduino.h>
#include <DHT12.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <SPI.h>
/*mise au point :
Affichage sur liaison serie toute les min et c'est tout

*/

/*branchements :

dht12:
+ : +5v
- : gnd
SDA : A4
SCl : A5

bouton poussoir : 
D7

humidité du sol :
A0 : S
GND : -
+5v : +

RTC : 
32kHz : NC
Int : D2
SCL : A5
SDA : A4
GND : GND
VCC : +5V

LED debug sur shieldproto :
D8

*/

// pin definitions
const uint8_t SQW_PIN(2);   // connect this pin to DS3231 INT/SQW pin

int alarm=1;
int LED=8;
int sensorPin = A0; // sélection de la pin de mesure du capteur d'humidité de sol


//variables
int HSol = 0; // initialisation de la valeur du capteur
float t12=0;
float h12=0;



// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;

volatile boolean alarmIsrWasCalled = false;


/*//Lecture de l'humidité du sol
void ReadHeSol (void){
    //lecture de l'H°Sol
    HSol = analogRead(sensorPin); // lecture directe de la valeur
}*/

/*// lecture de la temperature et de l'humidité de l'air
void ReadTempeHeAir (void){
    //activation com I2C
    
    Wire.begin();
    delay(190);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Read temperature as Celsius (the default)
    t12 = dht12.readTemperature();
    
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h12 = dht12.readHumidity();

    bool dht12Read = true;
    // Check if any reads failed and exit early (to try again).
    if (isnan(h12) || isnan(t12) )
    {
        Serial.println("Failed to read from DHT12 sensor!");
        dht12Read = false;
    }

    
}*/

//affichage de l'heure et de la date sur la liason série
void printTime(void) { 

    static time_t t = RTC.get();    
    tmElements_t tm;
    RTC.read(tm);
    //printDateTime(t);
    
    Serial.print(tm.Day);        
    Serial.print("/"); 
    Serial.print(tm.Month);        
    Serial.print("/");
    Serial.print(tm.Year-30);
    Serial.print(" \t");
    
    Serial.print(tm.Hour);        
    Serial.print(":");        
    Serial.print(tm.Minute);
    Serial.print(":");
    Serial.println(tm.Second);
    Serial.print("\n");
    delay (200);
}

/*//affichage sur la laison série de la température de l'air et de l'humidté de l'air
void WriteTempeHeAir (void){
       
    
    Serial.print("\ttemp: ");
    Serial.print(int(t12));
    Serial.print(".");
    Serial.print((int)((float)(t12 - (int)t12) * 10)); // affiche temperature

    
    Serial.print("\nhumidite : ");
    Serial.print(int (h12));
    Serial.print(".");
    Serial.println((int)((float)(h12 - (int)h12) * 10)); // affiche temperature
    
}*/

/*//affichage sur la laison série de l'humidité du sol
void WriteHeSol (void){
   
   
    Serial.print("\thumidite du sol : ");
    
    Serial.print(HSol);
}*/


// fonction effectuer pendant interruption RTC
void measurement (void) {
    alarmIsrWasCalled = true;
}

//mise en sommeil
void sleepNow (void){
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);//choix du mode sleep
	sleep_enable();//autorisation de mise en sommeil

	//attachInterrupt(BP, appuiBouton , FALLING);//parametrage interruption BP
	//attachInterrupt(digitalPinToInterrupt(2), measurement, LOW);//parametrage interruption RTC

	sleep_mode();//mise en veille
    //detachInterrupt(digitalPinToInterrupt(2));
	//sleep_disable();
	// first thing after waking from sleep: 	    	
}





void setup() {
	
	
    /*DEBUG*/
    Serial.begin(112560);

//pin interruption RTC
    
    pinMode(SQW_PIN, INPUT_PULLUP);
	
	// Start sensor handshake
	dht12.begin();

	// initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.squareWave(SQWAVE_NONE);
    
    // set Alarm 1 to occur at 5 seconds after every minute
    RTC.setAlarm(ALM1_MATCH_SECONDS, 5, 0, 0, 1);
    // clear the alarm flag
    RTC.alarm(ALARM_1);
    RTC.alarmInterrupt(ALARM_1, true);
    attachInterrupt(INT0, measurement, FALLING);
    /*DEBUG*/
    printTime();
    Serial.print("start\n");

}


void loop() {
    
    sleepNow;
    if (alarmIsrWasCalled)
    {
        if (RTC.alarm(ALARM_1))
        {
            Serial << " --> Alarm 1\n";
            printTime();
        }
        
        alarmIsrWasCalled = false;
    }

    
    
}

