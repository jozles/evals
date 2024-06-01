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
  //Mirf.payload = sizeof(unsigned long); // Taille d'un message (maximum 32 octets)
  Mirf.payload =32;
  
  Mirf.setRADDR((byte *) "serv1"); // Adresse de réception
  Mirf.setTADDR((byte *) "clie1"); // Adresse de transmission

  Mirf.config(); // Sauvegarde la configuration dans le module radio

  Serial.println("Go !"); 
}

void loop() {
  //byte message[sizeof(long)];
  byte message[32];
  if(!Mirf.isSending() && Mirf.dataReady()){
    Serial.println("Ping !");
    Mirf.getData(message); // Réception du paquet
Serial.println((char*)message);
    Mirf.send(message); // Et on le renvoie tel quel
  }
}
