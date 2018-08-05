#include <Arduino.h>
#include <LiquidCrystal.h> // on importe la bibliothèque
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

/*branchement :
RS :9
E : 10
D4 : 5
D5 : 4
D6 : 3
D7 : 2
*/
LiquidCrystal monEcran(9,10,5,4,3,2); // on crée l'objet écran11,12 >9,8

int octetReception=0; // variable de stockage des valeurs reçues sur le port Série (ASCII)
char caractereRecu=0; // variable pour stockage caractère recu
int compt=0; // variable comptage caractères reçus

String chaineReception=""; // déclare un objet String vide pour reception chaine

void setup() {

  Serial.begin(9600);

  monEcran.begin(16,2); //on initialise la communication avec 16 colonnes et deux lignes

  monEcran.clear(); // on efface l'écran

  monEcran.print("bonjour"); // on écrit Bonjour

  monEcran.setCursor(0,1); //on passe à la ligne suivante


}


void loop() {
  
  while (Serial.available()>0) { // tant qu'un octet en réception

          octetReception=Serial.read(); // Lit le 1er octet reçu et le met dans la variable
          compt=compt+1;

          Serial.println("Ascii Caractere "+ String(compt) +" = "+ String(octetReception)); // affiche code ASCII Octet reçu

          if (octetReception==9) { // si Octet reçu est le saut de ligne
            monEcran.clear();
            Serial.println("Saut de ligne recu");
            Serial.println ("Chaine recue="+chaineReception); // affiche la chaine recue
            monEcran.print(chaineReception);
            chaineReception=""; //RAZ le String de réception
            compt=0; // RAZ compteur
            delay(100); // pause
            break; // sort de la boucle while

          }
          else { // si le caractère reçu n'est pas un saut de ligne

             caractereRecu=char(octetReception); // convertit l'octet reçu en caractère
             chaineReception=chaineReception+caractereRecu; // ajoute le caratère au String
          }
          }
}
