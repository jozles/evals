/**
 * Exemple de code pour la bibliothèque Mirf – Serveur Ping Pong
 */
#include <SPI.h>      // Pour la communication via le port SPI
#include <Mirf.h>     // Pour la gestion de la communication
#include <nRF24L01.h> // Pour les définitions des registres du nRF24L01
#include <MirfHardwareSpiDriver.h> // Pour la communication SPI

void setup() {
  Serial.begin(115200);

  Mirf.cePin = 9; // Broche CE sur D9
  Mirf.csnPin = 10; // Broche CSN sur D10
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Initialise la bibliothèque

  Mirf.channel = 1; // Choix du canal de communication (128 canaux disponibles, de 0 à 127)
  Mirf.payload = sizeof(long); // Taille d'un message (maximum 32 octets)
  Mirf.config(); // Sauvegarde la configuration dans le module radio

  Mirf.setTADDR((byte *) "nrf01"); // Adresse de transmission
  Mirf.setRADDR((byte *) "nrf02"); // Adresse de réception

  Serial.print(Mirf.channel);Serial.println(" Go server !"); 
}

void loop() {
  byte message[sizeof(long)];

  if(!Mirf.isSending() && Mirf.dataReady()){
    Serial.print("Ping ! ");
    Mirf.getData(message); // Réception du paquet
    Serial.println((long)*message);
    
    Mirf.send(message); // Et on le renvoie tel quel
  }
  //else {Serial.println((uint8_t)Mirf.getStatus(),HEX);}
}
