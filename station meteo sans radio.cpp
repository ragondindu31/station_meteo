#include <Arduino.h>
#include <LiquidCrystal.h> // on importe la bibliothèque
#include <DHT12.h>
#include <avr/sleep.h>
#include <avr/power.h>
/*
description :
l'arduino se met en veille, puis se reveille lors de l'appui sur le bouton est en pull up.
la fonction sleep now fonctionne enfin sans bugg elle provient du site arduino
Il manque la mesure de l'humidité du sol pour etre complet.
*/

/*branchement
ecran:
RS :9
E : 10
D4 : 5
D5 : 4
D6 : 3
D7 : 6

dht12:
+ : +5v
- : gnd
SDA : A4
SCl : A5

bouton poussoir : 
D2
*/

volatile bool interruption =0;
int bouton=0;//test pour appui

// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;

LiquidCrystal monEcran(9,10,5,4,3,6); // on crée l'objet écran11,12 >9,8

/*fonction qui sert à signaler interruption bouton*/ 		

void appuiBouton (void) {
		interruption =1;		    
}

void sleepNow (){
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);//choix du mode sleep
	sleep_enable();//autorisation de mise en sommeil
	attachInterrupt(bouton, appuiBouton , FALLING);//parametrage interruption
	sleep_mode();//mise en veille
	sleep_disable();
	/* first thing after waking from sleep: */	
	detachInterrupt(bouton);	
}

void setup() {

	monEcran.begin(16,2); //on initialise la communication avec 16 colonnes et deux lignes
	monEcran.clear(); // on efface l'écran
	
	// Start sensor handshake
	dht12.begin();
	
}


void loop() {

	

	if (interruption == 1) {
	
	monEcran.print("Bonjour inconnu");
	delay (1500);
	/*mesure et affichage de la temprature et de l'humidité*/

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

		delay (3000);
		interruption=0;
		monEcran.clear();

	}
	sleepNow(); 
}

