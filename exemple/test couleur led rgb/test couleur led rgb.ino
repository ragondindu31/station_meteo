int ledRougePin = 9; // PWM
int ledVertPin = 10; // PWM
int ledBleuPin = 11; // PWM

void setup(){
  pinMode( ledRougePin, OUTPUT );
  pinMode( ledVertPin, OUTPUT );
  pinMode( ledBleuPin, OUTPUT );
  
 
  //
  //Pour résumer:
  //  On allume une LED en mettant la broche/pin à 5 volts
  //  On eteind une LED en mettant la broche/pin à 0 volts
  
  // Tout éteindre
  digitalWrite( ledRougePin, LOW ); 
  digitalWrite( ledVertPin, LOW ); 
  digitalWrite( ledBleuPin, LOW ); 
}

void loop()
{
// max 255
//orange ou presque	
int iRouge = 185;
int iVert = 30;
int iBleu = 0;
//analogWrite( ledRougePin, iRouge );
//analogWrite( ledVertPin, iVert );
//analogWrite( ledBleuPin, iBleu );
delay(200);

//vert joli	un peu bleu 
iRouge = 0;
analogWrite( ledRougePin, iRouge );
iVert = 190;
    analogWrite( ledVertPin, iVert );
iBleu = 10;
    analogWrite( ledBleuPin, iBleu );
delay(600);


}

