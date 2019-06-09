#include <DS3232RTC.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Streaming.h>
#include <DS3232RTC.h>
#include <Arduino.h>
#include <DHT12.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <SPI.h>

/*mise au point :

mesure humidite air sol et temperature
mode sleep
rtc reveil toute les min et 5s
Affichage sur liaison serie toute les min et 5s
emission radio 
je suis pas sur qu'on puisse faire de la com avec plusieurs modules mais on verra plus tard

16/05 emission pas au point, neccessité de réalise recepteur sur avant de faire mise au point emetteur
03/06 emission simple j'en ai marre
        emission ok mais pas la facon de creer un message PROBLEME AFFICHAGE SUR RECEVEUR DE LA TEMPERATURE
        ecriture de float envoi de int reception int et traitement de float prbm!!!!
04/06        tableau de float en envoi et en reception. un float ok puis 000000. tenté d'envoyer d'autre caractere en premier
09/06 enfin j'envoie et je receptionne ce que je veux
emission toute les minute d'un message selon la description du tableur "def trames com radio"
reception utiliser soft NRF24recepteurV3.ino
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

Radio :
 SCK : D13
 MISO : D12
 MOSI : D11
 CS : D10
 CE : D9


LED debug sur shieldproto :
D8

*/

// pin definitions

const int LED=8;
int alarm=1;
int sensorPin = A0; // sélection de la pin de mesure du capteur d'humidité de sol
RF24 radio(9, 10); // CE, CSN


//constantes
//const byte address[6] = "00001";

const uint8_t SQW_PIN(2);   // connect this pin to DS3231 INT/SQW pin
const char text[] = "humidité du sol : ";



//variables
volatile boolean alarmIsrWasCalled = false;

//int HSol = 0; // initialisation de la valeur du capteur //transformation en char d'int 
//float t12=0;
//float h12=0;
struct data{
    char idmodule;// numéro de module en prevision
    char typemsg;// défini quel type de message est envoyé par le module
    unsigned int nummsg;// just counter to see if receiving new data
    int HSol;
    float t12; 
    float h12;
};
struct data msgradio;


byte addresses[][6] = {"1Node","2Node"};
// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;


//Lecture de l'humidité du sol
void ReadHeSol (void){
    //lecture de l'H°Sol
    msgradio.HSol = analogRead(sensorPin); // lecture directe de la valeur
}

// lecture de la temperature et de l'humidité de l'air
void ReadTempeHeAir (void){
    //activation com I2C
    
    Wire.begin();
    delay(190);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Read temperature as Celsius (the default)
    
    msgradio.t12 = dht12.readTemperature();
    
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    msgradio.h12 = dht12.readHumidity();
    
    bool dht12Read = true;
    // Check if any reads failed and exit early (to try again).
    if (isnan(msgradio.h12) || isnan(msgradio.t12) )
    {
        Serial.println("Failed to read from DHT12 sensor!");
        dht12Read = false;
    }
    
}

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
}

//affichage sur la laison série de la température de l'air et de l'humidté de l'air
void WriteTempeHeAir (void){
       
    
    Serial.print("temp: ");
    Serial.println(msgradio.t12);// affiche temperature
     
    
    Serial.print("humidite de l'air: ");
    Serial.println(msgradio.h12);// affiche temperature
    }

//affichage sur la laison série de l'humidité du sol
void WriteHeSol (void){
      
    Serial.print("humidite du sol : ");
    Serial.println(msgradio.HSol);
    
}

// fonction effectuer pendant interruption RTC
void AwAke (void) {
    alarmIsrWasCalled = true;
}

//mise en sommeil
void sleepNow (void){
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);//choix du mode sleep
	sleep_enable();//autorisation de mise en sommeil
    digitalWrite(LED,LOW);
	sleep_mode();//mise en veille
}

//parametrage des interruption de la RTC
void parametrageRTC (void) {
    pinMode(SQW_PIN, INPUT_PULLUP);
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
    attachInterrupt(INT0, AwAke, FALLING);
}


void setup() {
	
    msgradio.idmodule=1;
	
    /*DEBUG*/
    Serial.begin(112560);
    Serial.println("");
    Serial.println("start");
    printTime();
    ReadHeSol();
    ReadTempeHeAir();
    pinMode(LED,OUTPUT);

    // Start sensor handshake
    dht12.begin();

    //parametrage des interruption de la RTC
    parametrageRTC(); //pour debug
    
    /*parametrage radio*/
    radio.begin();
    radio.setAutoAck(1);                    // Ensure autoACK is enabled
    radio.enableAckPayload();               // Allow optional ack payloads
    radio.setRetries(5,15);                 // Smallest time between retries, max no. of retries
    radio.setPayloadSize(32);                // Here we are sending 1-byte payloads to test the call-response speed
    //radio.openWritingPipe(address);        // Both radios listen on the same pipes by default, and switch when writing
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
   
    radio.stopListening();  
    //radio.startListening();
    // Dump the configuration of the rf unit for debugging
    radio.printDetails();
}


void loop() {
    

    digitalWrite(LED, HIGH);
    sleepNow();

    if (alarmIsrWasCalled)
    {
        if (RTC.alarm(ALARM_1))
        {
            digitalWrite(LED, HIGH);
            Serial.println("");
            printTime();
            ReadHeSol();
            ReadTempeHeAir();
            delay(50);
            WriteTempeHeAir();
            WriteHeSol();
            delay(300);

    // emission radio de message de relevé de température
    ++msgradio.nummsg;
    msgradio.typemsg=1;


    Serial.println(F("Now sending"));
    Serial.print("numéro du msg : ");
    Serial.println(int(msgradio.nummsg));
    if (!radio.write( &msgradio, sizeof(msgradio) ))
    {
       Serial.println(F("failed"));
    }  
       
    //delay(5000);
    }
    alarmIsrWasCalled = false;
    }

}

    
    


