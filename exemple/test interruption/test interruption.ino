/* Programme pour tester les interuptions 
trouver ici :
https://itechnofrance.wordpress.com/2013/03/24/utilisation-des-interruptions-sur-larduino/
*/
int bouton = 0; // interruption 0, pin 2
int pinled = 13; // LED intégré pin 13
int etat = LOW;

void traite_int()
{
  // fonction exécutée si interruption détectée
  etat = !etat;
  digitalWrite(pinled, etat);
}

void setup()
{
  attachInterrupt(bouton, traite_int, FALLING);
  pinMode(pinled, OUTPUT);
  digitalWrite(pinled, etat);
}

void loop()
{
  delay(10);
}

