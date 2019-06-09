#include <EEPROMex.h>
#include <EEPROMVar.h>

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
4 : mise en veille et reveil par interruption BP ou RTC : 
    >abandon momentané du mode sleep. la sortie du mode sleep n'est
    faisable que sur une interruption et pas sur les deux.  
5 : enregistrement dans l'EEPROM : 0


version LCD mise en veille com avec liaison série pour map : 0
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
volatile bool Interruption =0;
int BP=0;//test pour appui
int CMD5V = 12;
int alarm=1;
int LED=13;
int CapteurHe1 = A0; // sélection de la pin de mesure du capteur d'humidité de sol
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

//Lecture de l'humidité du sol 1°capteur
void ReadHeSol1 (void){
    //lecture de l'H°Sol
    HSol = analogRead(CapteurHe1); // lecture directe de la valeur
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
        delay(700);
        monEcran.clear();
        backlightOFF();
    }
    backlightOFF();
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
    //monEcran.clear();
    monEcran.setCursor(0,0);
    monEcran.print(tm.Day);        
    monEcran.print("/"); 
    monEcran.print(tm.Month);        
    monEcran.print("/");
    monEcran.print(tm.Year-30);
    monEcran.print(" ");
    //monEcran.setCursor(0,1);
    monEcran.print(tm.Hour);        
    monEcran.print("h");        
    monEcran.print(tm.Minute);
    monEcran.print("m");
    monEcran.print(tm.Second);
    delay (900);
    //monEcran.clear();
    //backlightOFF();
}

//affichage sur ecran LCD de la température de l'air et de l'humidté de l'air
void WriteTempeHeAir (void){
    backlightON();
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
    backlightON();
    monEcran.clear();   
    monEcran.setCursor(0,0);
    monEcran.print("humidite du sol : ");
    monEcran.setCursor(0,1);
    monEcran.print(HSol);
}

//affichage nombre de mesure effctue et temps actuel
void WriteNbreMesureTime (void){
    backlightON();
    //monEcran.clear();
    monEcran.setCursor(0,1);
    monEcran.print("nbre mesure : ");
  
    monEcran.print(nombredemesure);
    delay (1300);
    //backlightOFF();
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
            NbreAppuis= 0;
            break;

        /*case 3 :
            NbreAppuis= 0;
            WriteNbreMesureTime();
            delay (1500);
            break;*/

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
	
    Interruption = HIGH;
}

// fonction effectuer pendant interruption RTC
void measurement (void) {
	timetomesure=1;
    digitalWrite(LED, HIGH);
    
    //clear the alarm flag
    //RTC.alarm(ALARM_1);
	//detachInterrupt(digitalPinToInterrupt(2));

}
//mise en sommeil
/*void sleepNow (void){

	backlightOFF();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);//choix du mode sleep
	sleep_enable();//autorisation de mise en sommeil

    attachInterrupt(alarm, measurement, LOW);//parametrage interruption RTC
	attachInterrupt(BP, appuiBouton, LOW);//parametrage interruption BP

	sleep_mode();//mise en veille
	sleep_disable();
	// first thing after waking from sleep: 	
	detachInterrupt(BP);
    detachInterrupt(alarm);	
}*/

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
	
	//backlightOFF();
	power_usart0_disable();
	//activation des interruptions pour detection de appuis sur BP
	pinMode(BP, INPUT_PULLUP);
    attachInterrupt(BP, appuiBouton, LOW);//parametrage interruption BP


	//mise en veille des pin com I2C
	pinMode(A5,OUTPUT);
	pinMode(A4,OUTPUT);
	digitalWrite(A5,LOW);
	digitalWrite(A4,LOW);
	
	//gestion de l'affichage

	if (Interruption!=0) {
		NbreAppuis++;
	}

	if ((NbreAppuis!= 0)&&(Interruption!=0))
    {
		backlightON();
        monEcran.clear(); // on efface l'écran
		monEcran.print("nbr appui :");
		monEcran.print(NbreAppuis);
        delay (800);
        affichage();
        //necessaire sans mise en veille du module
        Interruption = LOW;
        monEcran.clear();
        backlightOFF();
        //sleepNow();
	}

	


	//operation apres interruption de la RTC 
	if (RTC.alarm(ALARM_1))
	{
        RTC.alarm(ALARM_1);    // reset the alarm flag
        //mesure de temperature et humidite
		//digitalWrite(LED, HIGH);
        power_adc_enable(); // ebable ADC converter
		ReadTempeHeAir ();
		ReadHeSol1 ();
		/*for (nombredemesure= 0; nombredemesure < 25; ++nombredemesure)
		{
			int indexEEPROMh12=24;
			int indexEEPROMHeSol=48;

			EEPROM.update(nombredemesure, h12 );
			EEPROM.update(indexEEPROMh12+nombredemesure, t12 );
			EEPROM.update(indexEEPROMHeSol+nombredemesure, HSol);

			if (nombredemesure =24)
				{
					nombredemesure =0;
					indexEEPROMHeSol= 0;
				}	
		}*/
        ++nombredemesure;
		timetomesure=0;
        delay (300);
        digitalWrite(LED, LOW);
        WriteNbreMesureTime();
        power_adc_disable();
	}

	//sleepNow(); 	
}

