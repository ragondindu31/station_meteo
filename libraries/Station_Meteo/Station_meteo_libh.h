/*
V1.0.0 
fonction d'appel des différents éléments de la station meteo

auteur : Régis Chollat

date : 11/2/2018
*/
#ifndef Station_meteo_lib_INCLUDED
#define Station_meteo_lib_INCLUDED

#include <LiquidCrystal.h>
#include <DS3232RTC.h>
#include <DHT12.h>
#include <Arduino.h>

//definition des entrées sorties en fonction du modèle d'arduino
/*#define PinDataRadio = 12;
#define PinLEDKeepAlive = 11;
#define PinBoutonPous = 10;
#define PinhumiditeSol = 0;
#define PinSDA = 4;
#define PinSCl = 5;*/
#define CMD5V = 12;
#define alarm=1;
#define LED=13;
#define sensorPin = A0;
#define BP=0;

//definition des structures

//definition des zones memoires 

//initialisation de l'horloge et autorisation alarme 1 et 2
void initializeRTC ();

//enclenchement de l'alarme 1 de la RTC toute les minutes + 5 secondes
void RTC_Alarm_1_1m5s ();

//enclenchement de l'alarme 1 de la RTC toute les ?
void RTC_Alarm1_Hours ();

//fonction 
void printDateTime(time_t t);

//fonction provisoire pour le debug
//peut etre pas obligatoire ici
time_t compileTime();

// lecture de la temperature et de l'humidité de l'air
void ReadTempeHeAir ();

//affichage sur ecran LCD de la température de l'air et de l'humidté de l'air
void WriteTempeHeAir ();

//Lecture de l'humidité du sol
void ReadHeSol ();

//affichage sur ecran LCD de l'humidité du sol
void WriteHeSol ();

//affichage de l'heure et de la date sur écran LCD
void printTime();

//affichage nombre de mesure effctue et temps actuel
void WriteNbreMesureTime();

/*gestion de l'affichage*/
void affichage();

#endif