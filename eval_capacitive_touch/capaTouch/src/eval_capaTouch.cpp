#include <Arduino.h>
#include "capaTouch.h"

#define LED 13

#define COMMON  4
#define KEY1  2
#define KEY2  6
#define KEYNB 2

#define SAMPLES 5

uint8_t keys[]={KEY1,KEY2};

#define LOO 4
char loo[]={"OFF\0ON \0"};

Capat capaKeys;

void setup()                    
{
    Serial.begin(115200);
    Serial.println("\nstart");

    pinMode(LED,OUTPUT);

    capaKeys.init(SAMPLES,COMMON,keys,KEYNB);
    capaKeys.calibrate();
}

void loop()                    
{
    delay(200);
    unsigned long beg=micros(); 

    capaKeys.capaKeysCheck();

    unsigned long end=micros()-beg;

    for(uint8_t i=0;i<1;i++){
        capaKeys.capaKeysGet(i);
        Serial.print(capaKeys.lowTresh[i]);Serial.print("/");Serial.print(capaKeys.highTresh[i]);
        Serial.print(" ");Serial.print(capaKeys.totalMem[i]);Serial.print(" ");
        Serial.print(" ");Serial.print(capaKeys.cntk[i]);Serial.print("-");Serial.print(capaKeys.meank[i]);
        Serial.print("=");Serial.print((uint16_t)(capaKeys.cntk[i]-capaKeys.meank[i]));
        Serial.print(" ");Serial.print(loo+capaKeys.keyVal[i]*LOO);if(i<KEYNB-1){Serial.print(" | ");}
    }
    
    Serial.print("   t=");Serial.println(end);

    digitalWrite(LED,capaKeys.keyVal[0]&0x01);
    
}
