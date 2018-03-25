/*
V1.0.0 
fonction d'appel des différents éléments de la station meteo

auteur : Régis Chollat

date : 11/2/2018
*/
#ifndef Station_meteo_H_INCLUDED
#define Station_meteo_H_INCLUDED

//definition des entrées sorties en fonction du modèle d'arduino
#define PinDataRadio = 12;
#define PinLEDKeepAlive = 11;
#define PinBoutonPous = 10;
#define PinhumiditeSol = 0;
#define PinSDA = 4;
#define PinSCl = 5;

//definition des structures

//definition des zones memoires 

//initialisation de l'horloge et autorisation alarme 1 et 2
void initializeRTC ();

//enclenchement de l'alarme 1 de la RTC toute les minutes + 5 secondes
void RTC_Alarm_1_1m5s ();

//enclenchement de l'alarme 1 de la RTC toute les ?
void RTC_Alarm1_test ();

//fonction 
void printDateTime(time_t t);

//fonction provisoire pour le debug
//peut etre pas obligatoire ici
time_t compileTime();

#endif