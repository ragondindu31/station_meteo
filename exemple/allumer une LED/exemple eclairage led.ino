/*allumer une LED.*/

const int L1 = 7;
void setup()
{
	 pinMode(L1, OUTPUT);
}

void loop()
{
digitalWrite(L1, HIGH); //allumer L1
delay(1000); // attendre 1 seconde
digitalWrite(L1, LOW); // Eteindre L1
delay(2000); // attendre 2 seconde	
}

