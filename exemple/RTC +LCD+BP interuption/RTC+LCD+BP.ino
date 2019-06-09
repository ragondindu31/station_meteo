#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal.h>
#include <DS3232RTC.h>        // http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        // http://arduiniana.org/libraries/streaming/

/*-----------------------------------------------------------------------------*
 * DS3231/DS3232 Alarm Example #2                                              *
 *                                                                             *
 * AFFICHE SUR LCD de l'heure sur la ligne 1 en décomposé j/d/Y H:M:S          *   
 * toute les minutes par interruptiuon                                         *
 * si appui sur BP affichage humidité sol  pdt 2 secondes                      *
 *                                                                             *
 * Set Alarm 2 to occur at a given time hh:mm.                                 *
 * Configure the RTC INT/SQW pin to be asserted when alarm match occurs.       *
 * Detect the alarm by polling the INT/SQW pin.                                *
 *                                                                             *
 * Hardware:                                                                   *
 * Arduino Uno, DS3231 RTC.                                                    *
 * Connect RTC SDA to Arduino pin A4.                                          *
 * Connect RTC SCL to Arduino pin A5.                                          *
 * Connect RTC INT/SQW to Arduino pin 2.                                       *
 *                                                                             *
 * Jack Christensen 16Sep2017                                                  *
 *-----------------------------------------------------------------------------*/
 


// pin definitions
LiquidCrystal monEcran(9,10,7,4,5,6); // on crée l'objet écran11,12 >9,8
int CMD5V = 12;
int sensorPin = A0; // sélection de la pin de mesure du capteur d'humidité de sol
int BP=0;//BP
int alarm=1;
int LED=13;
int nombreappui=0;
int valeurAppui=0;
// print the time
//lecture et affichage sur ecran LCD de l'heure et de la date
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
    monEcran.print(tm.Hour);        
    monEcran.print(":");        
    monEcran.print(tm.Minute);
    monEcran.print(":");
    monEcran.print(tm.Second);
    delay (200);
}

/*mesure et affichage de l'humidité du sol*/
void ReadWriteHeSol (){
    //lecture de l'H°Sol
    monEcran.clear();   
    monEcran.setCursor(0,0);
    monEcran.print("humidite du sol : ");
    monEcran.setCursor(0,1);
    monEcran.print(analogRead(sensorPin));
    delay (2000);
    monEcran.clear();
}

void appuiBouton (void) {
    
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    valeurAppui=1;
}

void setup()
{
    //pin interruption RTC
    pinMode(alarm, INPUT_PULLUP);
    //pin interrution BP
    pinMode(BP, INPUT_PULLUP);

    digitalWrite(CMD5V, HIGH);
    monEcran.begin(16,2); //on initialise la communication avec 16 colonnes et deux lignes
    monEcran.clear();

    //initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.squareWave(SQWAVE_NONE);

    //set Alarm 1 to occur at 5 seconds after every minute
    RTC.setAlarm(ALM1_MATCH_SECONDS, 5, 0, 0, 1);
    //clear the alarm flag
    RTC.alarm(ALARM_1);

    //parametrage interruption BP
    attachInterrupt(BP, appuiBouton , LOW);

}

void loop()
{
    // check to see if the INT/SQW pin is low, i.e. an alarm has occurred
    if ( RTC.alarm(ALARM_1) )
    {
        RTC.alarm(ALARM_1);    // reset the alarm flag
        printTime();
        //delay (300);
        //monEcran.clear();
    }
    
    if ( valeurAppui!=0 )
    {
    ReadWriteHeSol ();
    digitalWrite(LED, LOW);
    valeurAppui=0;
    monEcran.clear();
    }  
  
}

