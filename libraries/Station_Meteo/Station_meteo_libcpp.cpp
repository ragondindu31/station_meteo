#include <Arduino.h>
#include <Station_meteo_lib.h>
#include <LiquidCrystal.h>
#include <DHT12.h>
#include <Streaming.h>
#include <DS3232RTC.h>



// function to return the compile date and time as a time_t value
time_t compileTime()
{
    const time_t FUDGE(10);    //fudge factor to allow for upload time, etc. (seconds, YMMV)
    char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char compMon[3], *m;

    strncpy(compMon, compDate, 3);
    compMon[3] = '\0';
    m = strstr(months, compMon);

    tmElements_t tm;
    tm.Month = ((m - months) / 3 + 1);
    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);

    time_t t = makeTime(tm);
    return t + FUDGE;        //add fudge factor to allow for compile time
}
//initialisation de l'horloge et autorisation alarme 1 et 2
void initializeRTC ()
{
    // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);
    // set the RTC time and date to the compile time
    //mis en commentaire car fonctionne de façon aleatoire
    //RTC.set(compileTime());
}

//enclenchement de l'alarme 1 de la RTC toute les minutes + 5 secondes
void RTC_Alarm_1_1m5s ()
{
	// set Alarm 1 to occur at 5 seconds after every minute
	RTC.setAlarm(ALM1_MATCH_SECONDS, 5, 0, 0, 1);
	// clear the alarm flag
	RTC.alarm(ALARM_1);	
}

//enclenchement de l'alarme 1 de la RTC toute les heures à 05 min et 2 secondes
void RTC_Alarm1_Hours()
{
    // set Alarm 1 to occur at 5 minuts all hours
    //RTC.setAlarm(ALM1_MATCH_HOURS,0, 47,0,1); = nok
    RTC.setAlarm(ALM1_MATCH_MINUTES,2, 05,0,1);
    // clear the alarm flag
    RTC.alarm(ALARM_1); 
}


//fonction de traitement des données recuperer par RTC.get()
void printDateTime(time_t t)
{
    Serial << ((day(t)<10) ? "0" : "") << _DEC(day(t));
    Serial << monthShortStr(month(t)) << _DEC(year(t)) << ' ';
    Serial << ((hour(t)<10) ? "0" : "") << _DEC(hour(t)) << ':';
    Serial << ((minute(t)<10) ? "0" : "") << _DEC(minute(t)) << ':';
    Serial << ((second(t)<10) ? "0" : "") << _DEC(second(t));
}


//Lecture de l'humidité du sol
void ReadHeSol (){
    //lecture de l'H°Sol
    HSol = analogRead(sensorPin); // lecture directe de la valeur
}

// lecture de la temperature et de l'humidité de l'air
void ReadTempeHeAir (){
    //activation com I2C
    Wire.begin();
    delay(90);

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
void printTime() { 

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
void WriteTempeHeAir (){
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
    delay (500);
}

//affichage sur ecran LCD de l'humidité du sol
void WriteHeSol (){
    monEcran.clear();   
    monEcran.setCursor(0,0);
    monEcran.print("humidite du sol : ");
    monEcran.setCursor(0,1);
    monEcran.print(HSol);
}

//affichage nombre de mesure effctue et temps actuel
void WriteNbreMesureTime (){
    monEcran.clear();
    monEcran.print("nombre de mesure : ");
    monEcran.setCursor(0,1);
    monEcran.print(nombredemesure);
    delay (500);
    
    printTime();
}

/*gestion de l'affichage*/
void affichage() {
    /*
    apres le premier appui sur bouton affichage des données T° et humidité de l'air (H°Air)
    2° appui : affichage Humidité du Sol (H°Sol)
    */
    switch (NbreAppuis) {
        
        case 1 :
            WriteTempeHeAir();
            break;

        case 2 :
            WriteHeSol();
            delay (2000);
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