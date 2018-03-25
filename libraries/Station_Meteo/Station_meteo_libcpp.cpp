#include <Streaming.h>

#include <Streaming.h>

#include <Streaming.h>
#include <DS3232RTC.h>
#include <Arduino.h>

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
void RTC_Alarm1_test ()
{
    // set Alarm 1 to occur at 5 seconds after 2 minutes
    //RTC.setAlarm(ALM1_MATCH_HOURS,0, 47,0,1); = nok

    RTC.setAlarm(ALM1_MATCH_MINUTES,2, 05,0,1);//=ça pete 1 fois toute les heures
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

