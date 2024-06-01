/**
 * Exemple de code pour la bibliothèque Mirf – Client Ping Pong
 */
#include <SPI.h>      // Pour la communication via le port SPI
#include <Mirf.h>     // Pour la gestion de la communication
#include <nRF24L01.h> // Pour les définitions des registres du nRF24L01
#include <MirfHardwareSpiDriver.h> // Pour la communication SPI




unsigned long cnt=0;
unsigned long cntko=0;
unsigned long time_message;
unsigned long time_beg;
unsigned long time_end;

byte message[32]={"ABCDEFGHIJKLMNOPQRSTUVWXYZ012345"};
 
void setup() {
  Serial.begin(115200);
   
  Mirf.cePin = 10; // Broche CE sur D9
  Mirf.csnPin = 9; // Broche CSN sur D10
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Initialise la bibliothèque

  Mirf.channel = 1; // Choix du canal de communication (128 canaux disponibles, de 0 à 127)
  //Mirf.payload = sizeof(unsigned long);
  Mirf.payload = 32;

  Mirf.setRADDR((byte *) "clie1"); // Adresse de réception

  Mirf.config(); // Sauvegarde la configuration dans le module radio
   
  Serial.println("Go !"); 
}
 
void loop() {
  
  Mirf.setTADDR((byte *) "serv1");
  
  cnt++;
  delay(100);
  Serial.print(cnt);Serial.print(" Ping ... ");

  time_beg = micros();
  time_message = micros();

  //Mirf.send((byte *) &time_message);
  Mirf.send(message);
  
  while(Mirf.isSending()){};
  //Serial.print("sent ...");

  while(!Mirf.dataReady()) {
    
    if (micros() - time_beg > 10000 ) {    // to?
      cntko++;
      Serial.print(cntko);Serial.println(" Pas de pong");

      return;    
    }
  }
  
  //Mirf.getData((byte *) &time_message);
  Mirf.getData(message);

  time_end=micros();
  
  Serial.print("Pong: ");
  Serial.print(time_end - time_beg); 
  Serial.println("us");
 
} 
