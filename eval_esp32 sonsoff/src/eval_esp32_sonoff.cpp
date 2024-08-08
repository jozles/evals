#include "Arduino.h"
#include <WiFi.h>
#include <shconst2.h>
#include <shutil2.h>
#include "utilWifi.h"
#include "const.h"

uint32_t chipId = 0;

const char* ssid="devolo-5d3";
const char* pswd="JNCJTRONJMGZEEQL";

void blink(uint32_t ttot,uint32_t ton,uint32_t toff,uint32_t nbflash)
{
  for(uint8_t i=0;i<nbflash;i++){
    digitalWrite(PINLED, LEDON);delay(ton);digitalWrite(PINLED, LEDOFF);delay(toff);
  }
  delay(ttot-nbflash*(ton+toff));
}

void setup() {
  Serial.begin(115200);delay(1000);
  Serial.println("\n--- begin setup");
  pinMode(PINLED, OUTPUT);
  pinMode(PINSWA,OUTPUT);

  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.print("Chip ID: ");
  Serial.println(chipId);

if(!wifiConnexion(ssid,pswd,true)){while(1){blink(2000,999,999,1);}}
printWifiStatus(ssid);

Serial.println("\n--- end setup");

}

void loop() {
  blink(2000,50,100,2);
  digitalWrite(PINSWA,CLOSA);
  blink(2000,50,100,4);
  digitalWrite(PINSWA,OPENA);
}
