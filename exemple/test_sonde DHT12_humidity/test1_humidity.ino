/*
*effectue mesure de T°, 
*stocké en Eeprom
*lue et affiche lors de affichage de l'heure quand l'alarme sonne toute les minutes
*/
#include <Streaming.h>
#include <DS3232RTC.h>
#include <avr/sleep.h>
#include <DHT12.h>
#include <EEPROMex.h>
#include <EEPROMVar.h>
#include <Station_meteo_libh.h>


// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;


void setup()
{
	Serial.begin(112560);
	// Start sensor handshake
	dht12.begin();

	initializeRTC();
    // set the RTC time and date to the compile time
    //RTC.set(compileTime());   

	
    RTC_Alarm1_test ();
    //RTC.setAlarm(ALM1_MATCH_MINUTES,0,20,1);
    // clear the alarm flag
    RTC.alarm(ALARM_1);

    Serial << millis() << " Start ";
    printDateTime(RTC.get());
    Serial << endl;
}


void loop()
{
	
	
	// Reading temperature or humidity takes about 250 milliseconds!
		// Read temperature as Celsius (the default)
		float t12 = dht12.readTemperature();
		
		// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
		float h12 = dht12.readHumidity();

		bool dht12Read = true;
		// Check if any reads failed and exit early (to try again).
		if (isnan(h12) || isnan(t12) )
		{
		  Serial.println("Failed to read from DHT12 sensor!");

		  dht12Read = false;
		}

		if (dht12Read)
		{
		EEPROM.updateFloat( 0, t12);
		EEPROM.updateFloat( 8, h12);
		}
		

	RTC.alarmInterrupt(ALARM_1, true); // enclenchement de l'interruption lors de l'alarme?
	if ( RTC.alarm(ALARM_1) )    // check alarm flag, clear it if set
    {
        Serial << millis() << " ALARM_1 ";
        printDateTime(RTC.get());
        float eept12 = EEPROM.readFloat(0);
		float eeph12 = EEPROM.readFloat(8);
		Serial.print("\tDHT12=> Humidity: ");
		Serial.print(eeph12);
		Serial.print(" %\t");
		Serial.print("Temperature: ");
		Serial.print(eept12);
		Serial.print(" *C ");
        Serial << endl;
    }
    if ( RTC.alarm(ALARM_2) )    // check alarm flag, clear it if set
    {
        Serial << millis() << " ALARM_2 ";
        printDateTime(RTC.get());
        Serial << endl;
    }        
}




