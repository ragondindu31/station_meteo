
 	
const int bouton=2;
const int LED1=7;
int etatBouton=HIGH;

void setup()
{
	pinMode(bouton, INPUT);
	pinMode(LED1, OUTPUT);
}

void loop()
{
	etatBouton = digitalRead(bouton); 
    
    if(etatBouton == HIGH) //test si le bouton a un niveau logique HAUT
    {
        digitalWrite(LED1,LOW); //le bouton est relâché, la LED est allumée
    }
    else  //test si le bouton a un niveau logique différent de HAUT (donc BAS)
    {
        digitalWrite(LED1,HIGH); //la LED reste éteinte
    }
}

