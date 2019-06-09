#include <Streaming.h>
#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal.h>
#include <DS3232RTC.h>        // http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        // http://arduiniana.org/libraries/streaming/

/*-----------------------------------------------------------------------------*
 * DS3231/DS3232 Alarm Example #2                                              *
 *                                                                             *
 * AFFICHE SUR LCD le temps sur la ligne 1 en décomposé j/d/Y H:M:S            *   
 * et en timestamp sur la ligne 2.                                             *
 *                                                                             *
 *                                                                             *
 * Set Alarm 2 to occur at a given time hh:mm.                                 *
 * Configure the RTC INT/SQW pin to be asserted when alarm match occurs.       *
 * Detect the alarm by polling the INT/SQW pin.                                *
 *                                                                             *
 * Hardware:                                                                   *
 * Arduino Uno, DS3231 RTC.                                                    *
 * Connect RTC SDA to Arduino pin A4.                                          *
 * Connect RTC SCL to Arduino pin A5.                                          *
 * Connect RTC INT/SQW to Arduino pin 3.                                       *
 *                                                                             *
 * Jack Christensen 16Sep2017                                                  *
 *-----------------------------------------------------------------------------*/
 


// pin definitions
const uint8_t alarmInput(3);
LiquidCrystal monEcran(9,10,7,4,5,6); // on crée l'objet écran11,12 >9,8
int CMD5V = 12;


void setup()
{
    pinMode(3, INPUT_PULLUP);

    digitalWrite(CMD5V, HIGH);
    monEcran.begin(16,2); //on initialise la communication avec 16 colonnes et deux lignes
    


    // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);

/*  
    tmElements_t tm;
    tm.Hour = 06;               // set the RTC time to 06:29:50
    tm.Minute = 29;
    tm.Second = 50;
    tm.Day = 16;
    tm.Month = 9;
    tm.Year = 2017 - 1970;      // tmElements_t.Year is the offset from 1970
    RTC.write(tm);              // set the RTC from the tm structure
*/

    
/*
    // set Alarm 2 for 06:30:00
    RTC.setAlarm(ALM2_MATCH_HOURS, 0, 30, 6, 0);
    // clear the alarm flags
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
    RTC.squareWave(SQWAVE_NONE);
    // enable interrupt output for Alarm 2 only
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, true);
*/
}

void loop()
{
    // check to see if the INT/SQW pin is low, i.e. an alarm has occurred
    if ( !digitalRead(alarmInput) )
    {
        RTC.alarm(ALARM_2);    // reset the alarm flag
        monEcran.print("ALARM_2");
        delay (200);

    }

    // print the time when it changes
    static time_t tLast;
    time_t t = RTC.get();
    if (t != tLast)
    {
        tLast = t;
        //lecture et affichage sur ecran LCD de l'heure et de la date
        tmElements_t tm;
        RTC.read(tm);
        //printDateTime(t);
        monEcran.clear();
        monEcran.print(tm.Day);        
        monEcran.print("/"); 
        monEcran.print(tm.Month);        
        monEcran.print("/");
        monEcran.print(tm.Year+30);
        monEcran.print(" ");
        monEcran.print(tm.Hour);        
        monEcran.print(":");        
        monEcran.print(tm.Minute);
        monEcran.print(":");
        monEcran.print(tm.Second);
        monEcran.setCursor(0,1);
        monEcran.print(tLast);

        delay (200);
    }
}

