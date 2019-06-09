/*
*proto du module emetteur
*effectue mesure de T° et d'humidité de l'air et du sol,
*stocke les données en Eeprom
*lie et affiche toutes les heures/minutes sur terminal rs232
*liste des branchements hard :
*pin2 : int rtc
*
*reste : 
*emission radio
*define suivant hard
*define radio
*
*/
#include <VirtualWire.h>
//#include <VirtualWire_Config.h>
#include <Streaming.h>
#include <DS3232RTC.h>
#include <avr/sleep.h>
#include <DHT12.h>
#include <EEPROMex.h>
#include <EEPROMVar.h>
#include <Station_meteo_libh.h>
// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;
// sélection de la pin de mesure du capteur d'humidité de sol
int sensorPin = A0; 
 // utile pour debug
const int LED1 = 13;// a changer utiliser par radio

//compteur message radio
int comptMsgRadio;

void setup()
{
	Serial.begin(112560);
	// Start sensor handshake
	dht12.begin();
	//start RTC
	initializeRTC();
	//start radio
	vw_set_ptt_inverted(true); // Required for DR3100
	vw_setup(2000);
	vw_rx_start();       // Start the receiver PLL running
	comptMsgRadio=0;

	/*debbug*/
	Serial << millis() << " Start ";
	printDateTime(RTC.get());
	Serial << endl;
}

void loop()
{
	uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

	int humiditeSolMoy = 0;
	int humiditeSolEEP = 0;

	struct MesureMeteo
	{
	int *tempAir;
	float *humiditeSol;
	float *humiditeAir;	
	};

	struct MsgRadioHoraire
	{
	char *ffdebut;
	int *refReseau;
	int *refModuleRadio;
	int *typeMsg;
	int *CRC;
	struct MesureMeteo;
	char *ffend;
	};

	// parametrage interruption
	attachInterrupt(digitalPinToInterrupt(2), reveil, LOW);
	pinMode(digitalPinToInterrupt(2), INPUT_PULLUP);
	// enclenchement de l'interruption lors de l'alarme
	RTC.alarmInterrupt(ALARM_1, true); // enclenchement de l'interruption lors de l'alarme
	//commenter / decommenter l'alarme choisie
	//RTC_Alarm1_Hours ();
	RTC_Alarm_1_1m5s();

	/*mesure des capteurs */
	// mesure de l'humidité du sol moyenne de 20 mesures *20µs = 2s
	for (int i = 0; i < 20; ++i)
	{
		humiditeSolMoy += analogRead(sensorPin);
		delay(20);
	}
	// calcul de la moyenne
	humiditeSolEEP=(humiditeSolMoy/20); 

	// Read temperature as Celsius (the default)
	int t12 = dht12.readTemperature();
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	int h12 = dht12.readHumidity();
	bool dht12Read = true;
	// Check if any reads failed and exit early (to try again).
	if (isnan(h12) || isnan(t12))
	{
		Serial.println("Failed to read from DHT12 sensor!");
		dht12Read = false;
	}
	if (dht12Read)
	{
		EEPROM.updateInt(0, t12);
		EEPROM.updateInt(4, h12);
		EEPROM.updateInt(8, humiditeSolEEP);
	}
	/*fin des mesures */

	/*mise en mode sommeil*/

/*	// choix du mode sleep
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
	// autorisation de mise en sommeil
	sleep_enable(); 
	// mise en veille
	sleep_mode(); 
*/
	/* sortie du mode sommeil */
	// first thing after waking from sleep:
	//detachInterrupt(digitalPinToInterrupt(2));
	//sleep_disable();

	/* emission radio */
	// affichage des données en memoire
	//if (RTC.alarm(ALARM_1))
		// check alarm flag, clear it if set
	{
		//debbug
		/*Serial << millis() << " ALARM_1 ";
		printDateTime(RTC.get());
		float eept12 = EEPROM.readFloat(0);
		float eeph12 = EEPROM.readFloat(8);
		int eepghs = EEPROM.readInt(16);
		Serial.print("\tDHT12=> Humidity: ");
		Serial.print(eeph12);
		Serial.print(" %\t");
		Serial.print("Temperature: ");
		Serial.print(eept12);
		Serial.print(" °C ");
		Serial.print("humidité du sol : ");
		Serial.print(eepghs);
		Serial << endl;*/
		
		//lecture des mesures en EEprom
		MesureMeteo mesureHoraire;
		mesureHoraire.humiditeSol=EEPROM.readInt(8);
		mesureHoraire.humiditeAir=EEPROM.readInt(4);
		mesureHoraire.tempAir=EEPROM.readInt(0);
		Serial.print(EEPROM.readInt(0));
		Serial.print(EEPROM.readInt(4));
		Serial.print(EEPROM.readInt(8));

		//creation du msg radio
		MsgRadioHoraire MsgRadioHoraireToSend;
		MsgRadioHoraireToSend.ffdebut='ff';
		MsgRadioHoraireToSend.refReseau=1;
		MsgRadioHoraireToSend.refModuleRadio=1;
		MsgRadioHoraireToSend.typeMsg=1;
		MsgRadioHoraireToSend.CRC=4;
		MsgRadioHoraireToSend.MesureMeteo=MesureHoraire.humiditeAir;
		MsgRadioHoraireToSend.ffend='ff';

		
		/*emission radio*/
		//emission d'un message radio 1/heure
		digitalWrite(13, true); // Flash a light to show transmitting
		//envoi d'une trame
		comptMsgRadio+=1;
		vw_send((byte *) &comptMsgRadio, sizeof(comptMsgRadio)); // On envoie le message
    	vw_wait_tx(); // Wait until the whole message is gone
    	delay(200);
    	
  		vw_send((byte *) &MsgRadioHoraireToSend, sizeof(MsgRadioHoraireToSend));
	    vw_wait_tx(); // Wait until the whole message is gone
	    

    	// Wait at most 200ms for a reply
    if (vw_wait_rx_max(200))
    {
	if (vw_get_message(buf, &buflen)) // Non-blocking
	{
	    int i;
	    
	    // Message with a good checksum received, dump it.
	    Serial.print("Got: ");
	    
	    for (i = 0; i < buflen; i++)
	    {
		Serial.print(buf[i], HEX);
		Serial.print(" ");
	    }
	    Serial.println("");
	    Serial.println("Sent");
	    digitalWrite(13, false);
	    delay(200);
	}
    }
    else
	Serial.println("Timout");
	}
}


/*interruption*/
void reveil()
{
	/*debbug*/
	digitalWrite(LED1, HIGH); 
	delay(20);
	digitalWrite(LED1, LOW); 
}
