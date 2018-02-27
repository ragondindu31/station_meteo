#include <avr/sleep.h>
#include <avr/power.h>

//placer une pullup 100k sur l'entrée 2
//fonctionne sur contact gnd sur entrée 2
// testé avec detachInterrrupt dans l'interruption = pas de remise en mode sleep
void setup()
{
	Serial.begin(9600);
	

}

void loop()
{
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);//choix du mode sleep

	attachInterrupt(digitalPinToInterrupt(2), affichage, LOW);//parametrage interruption


	sleep_enable();//autorisation de mise en sommeil

	
	Serial.print("endormons nous !\n");//message de debug
	delay(500);
	sleep_mode();//mise en veille

	// first thing after waking from sleep:	
	detachInterrupt(digitalPinToInterrupt(2));

  	sleep_disable();         
	


	Serial.print("je suis reveillé\n");
  	delay(500);  
}

void affichage ()
{
	    	    
	Serial.print("l'interruption est survenue !\n");
		    
}