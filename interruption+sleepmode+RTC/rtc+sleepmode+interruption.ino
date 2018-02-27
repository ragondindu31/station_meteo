#include <Streaming.h>

/*-----------------------------------------------------------------------------*
 * DS3231/DS3232 Alarm Example #1                                              *
 *                                                                             *
 *alarme avec activation de la pin int toutes les minutes (RTC)                *
 *activation des interruptions sur pin2 (arduino)                              *
 *mise en mode sleep                                                           * 
 *reveil toute les minutes et affichage de l'heure                             *                                       *
 *                                                                             *
 *tout n'est pas encore au point  le premier affichage ce fait de façon bizarre*                            *
 *                                                                             *
 *                                                                             *                                                                             *
 * Hardware:                                                                   *
 * Arduino Uno, DS3231 RTC.                                                    *
 * Connect RTC SDA to Arduino pin A4.                                          *
 * Connect RTC SCL to Arduino pin A5.                                          *
 *                                                                             *
 * RC 27/01/2018                                                               *
 *-----------------------------------------------------------------------------*/
 
#include <DS3232RTC.h>      // http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/
#include <avr/sleep.h>


void setup()
{
    Serial.begin(115200);
    

    // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);

    // set the RTC time and date to the compile time
    RTC.set(compileTime());
    
    // set Alarm 1 to occur at 5 seconds after every minute
    RTC.setAlarm(ALM1_MATCH_SECONDS, 5, 0, 0, 1);
    // clear the alarm flag
    RTC.alarm(ALARM_1);
    

    Serial << millis() << " Start ";
    printDateTime(RTC.get());
    Serial << endl;
}

void loop()
{
    

    attachInterrupt(digitalPinToInterrupt(2), affichage, LOW);//parametrage interruption
    pinMode(digitalPinToInterrupt(2), INPUT_PULLUP);

    RTC.alarmInterrupt(ALARM_1, true); // enclenchement de l'interruption lors de l'alarme?

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//choix du mode sleep
    sleep_enable();//autorisation de mise en sommeil

    Serial.print("endormons nous !\n");//message de debug
    delay(500);
    sleep_mode();//mise en veille

    // first thing after waking from sleep: 
    detachInterrupt(digitalPinToInterrupt(2));

    sleep_disable();         


    Serial.print("je suis reveillé\n");
    delay(500);                    // no need to bombard the RTC continuously

    if ( RTC.alarm(ALARM_1) )    // check alarm flag, clear it if set
    {
        Serial << millis() << " ALARM_1 ";
        printDateTime(RTC.get());
        Serial << endl;
    }
    if ( RTC.alarm(ALARM_2) )    // check alarm flag, clear it if set
    {
        Serial << millis() << " ALARM_2 ";
        printDateTime(RTC.get());
        Serial << endl;
    }        

}

void printDateTime(time_t t)
{
    Serial << ((day(t)<10) ? "0" : "") << _DEC(day(t));
    Serial << monthShortStr(month(t)) << _DEC(year(t)) << ' ';
    Serial << ((hour(t)<10) ? "0" : "") << _DEC(hour(t)) << ':';
    Serial << ((minute(t)<10) ? "0" : "") << _DEC(minute(t)) << ':';
    Serial << ((second(t)<10) ? "0" : "") << _DEC(second(t));
}

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

void affichage ()
{
                
    Serial.print("l'interruption est survenue !\n");
    
}