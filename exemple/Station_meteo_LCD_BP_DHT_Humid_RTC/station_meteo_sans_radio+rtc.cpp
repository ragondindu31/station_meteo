#include <Streaming.h>
#include <DS3232RTC.h>
#include <Arduino.h>
#include <LiquidCrystal.h> // on importe la bibliothèque
#include <DHT12.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <SPI.h>
/*mise au point :
1 : separation affichage et prise de mesure : 1
2 : affichage suivant nombre appui temp ou humidté : 1
3 : Mesures de temperature et humidite toutes les minutes : 1
4 : mise en veille et reveil par interruption BP ou RTC : 1

version LCD mise en veille com avec liaison série pour map : 0
gestion du reveil par les 2 interruptions : 1/2
*/

/*branchements :
ecran:
RS :9
E : 10
D4 : 7
D5 : 4
D6 : 5
D7 : 6

dht12:
+ : +5v
- : gnd
SDA : A4
SCl : A5

bouton poussoir : 
D2

humidité du sol :
A0 : S
GND : -
+5v : +

RTC : 
32kHz : NC
Int : D3
SCL : A5
SDA : A4
GND : GND
VCC : +5V

LED arduino :
13

*/

// pin definitions
volatile bool interruption =0;
int BP=0;//test pour appui
int CMD5V = 12;
int alarm=1;
int LED=13;
int sensorPin = A0; // sélection de la pin de mesure du capteur d'humidité de sol
LiquidCrystal monEcran(9,10,7,4,5,6); // on crée l'objet écran

//variables
int HSol = 0; // initialisation de la valeur du capteur
int seuil_alerteHumiditeSol = 300; // valeur du seuil à partir duquel l'alerte est donnée
int NbreAppuis=0; //variable pour modifer l'affichage
int timetomesure =0;
float t12=0;
float h12=0;
int nombredemesure=0;



// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;

void backlightON (void) {
    
    digitalWrite(CMD5V, HIGH);
}

void backlightOFF (void) {
    
    digitalWrite(CMD5V, LOW);
}

//Lecture de l'humidité du sol
void ReadHeSol (void){
    //lecture de l'H°Sol
    HSol = analogRead(sensorPin); // lecture directe de la valeur
}

// lecture de la temperature et de l'humidité de l'air
void ReadTempeHeAir (void){
    //activation com I2C
    backlightON();
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
        monEcran.clear(); // on efface l'écran
        monEcran.print("Failed to read from DHT12 sensor!");
        dht12Read = false;
    }

    //mise en veille des pin 
    // pinMode(A5,OUTPUT);
    // pinMode(A4,OUTPUT);

    // digitalWrite(A5,LOW);
    // digitalWrite(A4,LOW);
}

//affichage de l'heure et de la date sur écran LCD
void printTime(void) { 

    static time_t t = RTC.get();    
    tmElements_t tm;
    RTC.read(tm);
    //printDateTime(t);
    monEcran.clear();
    monEcran.print(tm.Day);        
    monEcran.print("/"); 
    monEcran.print(tm.Month);        
    monEcran.print("/");
    monEcran.print(tm.Year-30);
    monEcran.print(" ");
    monEcran.setCursor(0,1);
    monEcran.print(tm.Hour);        
    monEcran.print(":");        
    monEcran.print(tm.Minute);
    monEcran.print(":");
    monEcran.print(tm.Second);
    delay (200);
}

//affichage sur ecran LCD de la température de l'air et de l'humidté de l'air
void WriteTempeHeAir (void){
    monEcran.clear();   
    monEcran.setCursor(0,0);
    monEcran.print("temp: ");
    monEcran.print(int(t12));
    monEcran.print(".");
    monEcran.print((int)((float)(t12 - (int)t12) * 10)); // affiche temperature

    monEcran.setCursor(0,1);
    monEcran.print("humidite : ");
    monEcran.print(int (h12));
    monEcran.print(".");
    monEcran.print((int)((float)(h12 - (int)h12) * 10)); // affiche temperature
    
}

//affichage sur ecran LCD de l'humidité du sol
void WriteHeSol (void){
    monEcran.clear();   
    monEcran.setCursor(0,0);
    monEcran.print("humidite du sol : ");
    monEcran.setCursor(0,1);
    monEcran.print(HSol);
}

//affichage nombre de mesure effctue et temps actuel
void WriteNbreMesureTime (void){
    monEcran.clear();
    monEcran.print("nombre de mesure : ");
    monEcran.setCursor(0,1);
    monEcran.print(nombredemesure);
    delay (800);
    
    printTime();
}

//gestion de l'affichage
void affichage(void) {
    /*
    apres le premier appui sur bouton affichage des données T° et humidité de l'air (H°Air)
    2° appui : affichage Humidité du Sol (H°Sol)
    */
    switch (NbreAppuis) {
        
        case 1 :
            WriteTempeHeAir();
            delay (2000);
            break;

        case 2 :
            WriteHeSol();
            delay (2000);
            //NbreAppuis= 0;
            break;

        case 3 :
            NbreAppuis= 0;
            WriteNbreMesureTime();
            delay (1000);
            break;

        default :
        
            monEcran.clear();
            monEcran.setCursor(0,0);
            monEcran.print("probleme");
            delay (2000);
            NbreAppuis= 0;
            break;
    }
}

//fonction effectuer pendant interruption (appui sur bouton)		
void appuiBouton (void) {
	
	//pinMode(LED, OUTPUT);
    //digitalWrite(LED, HIGH);
    interruption = HIGH;
}



// fonction effectuer pendant interruption RTC
void measurement (void) {
	timetomesure=1;
	pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    delay (200);
    //clear the alarm flag
    RTC.alarm(ALARM_1);
	//detachInterrupt(digitalPinToInterrupt(2));

}
//mise en sommeil
void sleepNow (void){
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);//choix du mode sleep
	sleep_enable();//autorisation de mise en sommeil

	attachInterrupt(BP, appuiBouton , FALLING);//parametrage interruption BP
	attachInterrupt(digitalPinToInterrupt(1), measurement, LOW);//parametrage interruption RTC

	sleep_mode();//mise en veille
	sleep_disable();
	// first thing after waking from sleep: 	
	detachInterrupt(BP);	
}





void setup() {
	
	//pin interruption RTC
    pinMode(alarm, INPUT_PULLUP);
    //pin interrution BP
    pinMode(BP, INPUT_PULLUP);
    //activation commande 5V
	pinMode(CMD5V, OUTPUT);

    //initialisation de l'écran
	monEcran.begin(16,2); //on initialise la communication avec 16 colonnes et deux lignes
	monEcran.clear(); // on efface l'écran
	
	// Start sensor handshake
	dht12.begin();

	// initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.squareWave(SQWAVE_NONE);
   
    // set Alarm 1 to occur at 5 seconds after every minute
    RTC.setAlarm(ALM1_MATCH_SECONDS, 5, 0, 0, 1);
    // clear the alarm flag
    RTC.alarm(ALARM_1); 

}


void loop() {
	
	backlightOFF();
	
	//activation des interruptions pour detection de appuis sur BP
	pinMode(BP, INPUT_PULLUP);
	attachInterrupt(BP, appuiBouton , FALLING);

	//mise en veille des pin com I2C
	pinMode(A5,OUTPUT);
	pinMode(A4,OUTPUT);
	digitalWrite(A5,LOW);
	digitalWrite(A4,LOW);
	
	//gestion de l'affichage
	if (interruption!=0) {
		NbreAppuis++;
		//necessaire sans mise en veille du module
		//interruption = LOW;
	}

	if (NbreAppuis!= 0) {
		digitalWrite(CMD5V, HIGH);
		//delay (1000);
        monEcran.clear();
        monEcran.print("nbr appui :");
        monEcran.print(NbreAppuis);
        delay (500);
		affichage();

	}

	monEcran.clear();
	backlightOFF();
	sleepNow();


	//operation apres interruption de la RTC 
	if (RTC.alarm(ALARM_1))
	{
		//mesure de temperature et humidite
		ReadTempeHeAir ();
		ReadHeSol ();
		timetomesure=0;
		delay (500);
		digitalWrite(LED, LOW);
		nombredemesure++;
		delay (500);
	}

	sleepNow(); 	
}

