/**
 * Exemple de code pour la bibliothèque Mirf – Client Ping Pong
 */
#include <SPI.h>      // Pour la communication via le port SPI
#include <Mirf.h>     // Pour la gestion de la communication
#include <nRF24L01.h> // Pour les définitions des registres du nRF24L01
#include <MirfHardwareSpiDriver.h> // Pour la communication SPI

unsigned long cnt=0;
unsigned long beg=0;
 
void setup() {
  Serial.begin(115200);
   
  Mirf.cePin = 9; // Broche CE sur D9
  Mirf.csnPin =10; // Broche CSN sur D10
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Initialise la bibliothèque

  Mirf.channel = 1; // Choix du canal de communication (128 canaux disponibles, de 0 à 127)
  Mirf.payload = 32; //sizeof(long); // Taille d'un message (maximum 32 octets)
  Mirf.config(); // Sauvegarde la configuration dans le module radio

  Mirf.setTADDR((byte *) "nrf02"); // Adresse de transmission
  Mirf.setRADDR((byte *) "nrf01"); // Adresse de réception
   
  Serial.println("Go client !"); 
}
 
void loop() {
  char message[32+1]={"abcdefghijklmnopqrstuvwxyz123456"}; //= micros(); // On garde le temps actuel retourné par millis()
  
  
  cnt++;  
  Serial.print(cnt);Serial.print(" Ping ... ");
  
  unsigned long beg = micros();
  Mirf.send((byte*)message); // On envoie le temps actuel en utilisant une astuce pour transformer le long en octets
  while(Mirf.isSending()); // On attend la fin de l'envoi
  //Serial.print("sent..."); 
  // Attente de la réponse
  while(!Mirf.dataReady()) { // On attend de recevoir quelque chose
    if (micros() - beg > 10000 ) { // Si on attend depuis plus de 0,01 seconde
      Serial.println("Pas de pong"); // C'est le drame ...
      return;
    }
  }
  
  // La réponse est disponible
  Mirf.getData((byte*)message); // On récupère la réponse
  long tend=micros();
  // On affiche le temps de latence (sans division par deux)
  Serial.print("Pong: ");
  Serial.print(tend - beg); 
  Serial.print("us  ");message[Mirf.payload]='\0';Serial.println(message);
  
  // Pas besoin de tester plus d'une fois par seconde
  delay(1000);
} 
