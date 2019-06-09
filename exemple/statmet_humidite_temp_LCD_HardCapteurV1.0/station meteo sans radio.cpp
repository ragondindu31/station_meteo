#include <DS3232RTC.h>
#include <Arduino.h>
#include <LiquidCrystal.h> // on importe la bibliothèque
#include <DHT12.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <SPI.h>
/*mise au point :

Mesures de temperature toutes les minutes
enregistrement dans l'EEPROM
traitement (moyenne et autre)
mise en veille

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
GND 
+5v

*/
volatile bool interruption =0;
int bouton=0;//test pour appui
int CMD5V = 12;
int sensorPin = A0; // sélection de la pin de mesure du capteur d'humidité de sol
int sensorHumiditySolValue = 0; // initialisation de la valeur du capteur
int seuil_alerte = 300; // valeur du seuil à partir duquel l'alerte est donnée
int NbreAppuis=0; //variable pour modifer l'affichage

// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;

LiquidCrystal monEcran(9,10,7,4,5,6); // on crée l'objet écran11,12 >9,8

/*fonction effectuer pendant interruption (appui sur bouton) */ 		
void appuiBouton (void) {
	
	interruption = HIGH;
}

/*mise en sommeil*/
void sleepNow (){
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);//choix du mode sleep
	sleep_enable();//autorisation de mise en sommeil
	attachInterrupt(bouton, appuiBouton , FALLING);//parametrage interruption
	sleep_mode();//mise en veille
	sleep_disable();
	/* first thing after waking from sleep: */	
	detachInterrupt(bouton);	
}

/*mesure et affichage de la temperature et de l'humidité de l'air*/
void ReadWriteTempeHeAir (){
	//activation com I2C
	Wire.begin();
	delay(90);

    // Reading temperature or humidity takes about 250 milliseconds!
	// Read temperature as Celsius (the default)
	float t12 = dht12.readTemperature();
	
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float h12 = dht12.readHumidity();

	bool dht12Read = true;
	// Check if any reads failed and exit early (to try again).
	if (isnan(h12) || isnan(t12) )
	{
		monEcran.clear(); // on efface l'écran
		monEcran.print("Failed to read from DHT12 sensor!");
		dht12Read = false;
	}

	if (dht12Read)
	{
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

	//mise en veille des pin 
	pinMode(A5,OUTPUT);
	pinMode(A4,OUTPUT);

	digitalWrite(A5,LOW);
	digitalWrite(A4,LOW);
	
}

/*mesure et affichage de l'humidité du sol*/
void ReadWriteHeSol (){
	//lecture de l'H°Sol
	sensorHumiditySolValue = analogRead(sensorPin); // lecture directe de la valeur
	monEcran.clear();	
	monEcran.setCursor(0,0);
	monEcran.print("humidite du sol : ");
	monEcran.setCursor(0,1);
	monEcran.print(sensorHumiditySolValue);
}

/*gestion de l'affichage*/
void affichage() {
	/*
	apres le premier appui sur bouton affichage des données T° et humidité de l'air (H°Air)
	2° appui : affichage Humidité du Sol (H°Sol)
	*/
	switch (NbreAppuis) {
		
		case 1 :
			ReadWriteTempeHeAir();
			break;
		case 2 :
			ReadWriteHeSol();
			NbreAppuis= 0;
			break;
		default :
			monEcran.clear();
			monEcran.setCursor(0,0);
			monEcran.print("probleme");
			delay (2000);
			break;
	}
}
void setup() {

	//initialisation de l'écran
	monEcran.begin(16,2); //on initialise la communication avec 16 colonnes et deux lignes
	monEcran.clear(); // on efface l'écran
	
	// Start sensor handshake
	dht12.begin();
	
	pinMode(CMD5V, OUTPUT);
}


void loop() {
	//activation des interruptions pour detection de appuis sur BP
	digitalWrite(CMD5V, LOW);

	
	pinMode(bouton, INPUT_PULLUP);
	attachInterrupt(bouton, appuiBouton , FALLING);//parametrage interruption
	
	//mise en veille des pin com I2C
	pinMode(A5,OUTPUT);
	pinMode(A4,OUTPUT);
	digitalWrite(A5,LOW);
	digitalWrite(A4,LOW);

	//mise en veille des pin com ecran
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	pinMode(5,OUTPUT);
	pinMode(6,OUTPUT);
	pinMode(9,OUTPUT);
	pinMode(10,OUTPUT);

	digitalWrite(3,LOW);
	digitalWrite(4,LOW);
	digitalWrite(5,LOW);
	digitalWrite(6,LOW);
	digitalWrite(9,LOW);
	digitalWrite(10,LOW);


	if (interruption!=0) {
		NbreAppuis++;
	}

	if (NbreAppuis!= 0) {
		digitalWrite(CMD5V, HIGH);
		delay (500);
		affichage();
	}

	delay (2000);
	monEcran.clear();
	digitalWrite(CMD5V, LOW);
	sleepNow(); 
}

