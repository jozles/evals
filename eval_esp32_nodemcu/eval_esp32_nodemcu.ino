#include <WiFi.h>
#include "utilWifi.h"
#include "shconst2.h"
#include "shutil2.h"


#define LED 2

uint32_t chipId = 0;

char* ssid="devolo-5d3";
char* pswd="JNCJTRONJMGZEEQL";

void setup() {
  Serial.begin(115200);delay(1000);
  Serial.println("\n--- begin setup");
  pinMode(LED, OUTPUT);

  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.print("Chip ID: ");
  Serial.println(chipId);

wifiConnexion(ssid,pswd,true);
printWifiStatus(ssid);

Serial.println("\n--- end setup");

}

void loop() {
  digitalWrite(LED, HIGH);delay(50);digitalWrite(LED, LOW);delay(1000);     
}
