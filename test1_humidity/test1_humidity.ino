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
#include <Station_meteo.h>

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

    void RTC_Alarm1_2m5s();
    

    Serial << millis() << " Start ";
    printDateTime(RTC.get());
    Serial << endl;
}
int timeSinceLastRead = 0;

void loop()
{
	// Report every 30 seconds.
	if(timeSinceLastRead > 30000) {
	// Reading temperature or humidity takes about 250 milliseconds!
		// Read temperature as Celsius (the default)
		float t12 = dht12.readTemperature();
		// Read temperature as Fahrenheit (isFahrenheit = true)
		float f12 = dht12.readTemperature(true);
		// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
		float h12 = dht12.readHumidity();

		bool dht12Read = true;
		// Check if any reads failed and exit early (to try again).
		if (isnan(h12) || isnan(t12) || isnan(f12)) {
		  Serial.println("Failed to read from DHT12 sensor!");

		  dht12Read = false;
		}

		if (dht12Read){
		EEPROM.updateFloat( 0, t12);
		EEPROM.updateFloat( 4, f12);
		EEPROM.updateFloat( 8, h12);
		}
		timeSinceLastRead = 0;
	}
	delay(500);
	timeSinceLastRead += 500;

	RTC.alarmInterrupt(ALARM_1, true); // enclenchement de l'interruption lors de l'alarme?
	if ( RTC.alarm(ALARM_1) )    // check alarm flag, clear it if set
    {
        Serial << millis() << " ALARM_1 ";
        printDateTime(RTC.get());
        // Compute heat index in Fahrenheit (the default)
		//float hif12 = dht12.computeHeatIndex(f12, h12);
		// Compute heat index in Celsius (isFahreheit = false)
		//float hic12 = dht12.computeHeatIndex(t12, h12, false);
		// Compute dew point in Fahrenheit (the default)
		//float dpf12 = dht12.dewPoint(f12, h12);
		// Compute dew point in Celsius (isFahreheit = false)
		//float dpc12 = dht12.dewPoint(t12, h12, false);
        float eept12 = EEPROM.readFloat(0);
		float eepf12 = EEPROM.readFloat(4);
		float eeph12 = EEPROM.readFloat(8);
		Serial.print("\tDHT12=> Humidity: ");
		Serial.print(eeph12);
		Serial.print(" %\t");
		Serial.print("Temperature: ");
		Serial.print(eept12);
		Serial.print(" *C ");
		Serial.print(eepf12);
		Serial.print(" *F\t");
		// Serial.print("  Heat index: ");
		// Serial.print(hic12);
		// Serial.print(" *C ");
		// Serial.print(hif12);
		// Serial.print(" *F");
		// Serial.print("  Dew point: ");
		// Serial.print(dpc12);
		// Serial.print(" *C ");
		// Serial.print(dpf12);
		// Serial.println(" *F");
        Serial << endl;
    }
    if ( RTC.alarm(ALARM_2) )    // check alarm flag, clear it if set
    {
        Serial << millis() << " ALARM_2 ";
        printDateTime(RTC.get());
        Serial << endl;
    }        
}




