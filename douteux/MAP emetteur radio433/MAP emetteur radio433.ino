/**
 * Exemple de code pour la bibliothèque VirtualWire – Client d'envoi de structure
 */
 
#include <VirtualWire.h>


void setup() {
  Serial.begin(9600);

  // Initialisation de la bibliothèque VirtualWire
  // Vous pouvez changez les broches RX/TX/PTT avant vw_setup() si nécessaire
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);
  vw_rx_start();       // Start the receiver PLL running
  
  Serial.println("Go !"); 
}
 
void loop() 
{
  const char *msg = "bierduino";
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    Serial.println("Sent");
    delay(200);
    digitalWrite(13, false);

    // Wait at most 200ms for a reply
    if (vw_wait_rx_max(200))
    {
	if (vw_get_message(buf, &buflen)) // Non-blocking
	{
	    int i;
	    
	    // Message with a good checksum received, dump it.
	    Serial.print("Got: ");
	    
	    for (i = 0; i < buflen; i++)
	    {
		Serial.print(buf[i], HEX);
		Serial.print(" ");
	    }
	    Serial.println("");
	}
    }
    else
	Serial.println("Timout");
	
} 