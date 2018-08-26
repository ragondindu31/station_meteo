int CMDNPN = 11;



void setup()
{
pinMode(CMDNPN, OUTPUT);
Serial.begin(9600);
Serial.print("hello");
}

void loop()
{
	digitalWrite(CMDNPN, HIGH);
	Serial.print(CMDNPN);
	delay(500);
	digitalWrite(CMDNPN, LOW);
	Serial.print(CMDNPN);
	delay(500);
}

