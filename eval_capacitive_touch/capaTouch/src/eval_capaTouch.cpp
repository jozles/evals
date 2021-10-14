#include <Arduino.h>
#include "capaTouch.h"

#define LED 13

#if defined(__AVR__)
#define COMMON  4
#define KEY1  2
#define KEY2  6
#define SAMPLES 5
#endif //__AVR__

#if defined(ARDUINO_ARCH_ESP8266)
#define COMMON 13
#define KEY1  12
#define KEY2  14
#define SAMPLES 5
#endif //__8266__


#define KEYNB 2

uint8_t keys[]={KEY1,KEY2};

#define LOO 4
char loo[]={"OFF\0ON \0"};

Capat capaKeys;

void showPins()
{   
    Serial.print(GPI,BIN);Serial.print(" ");
/*    Serial.print(DIRECT_READ(0,12));Serial.print(" ");
    Serial.print(DIRECT_READ(0,13));Serial.print(" ");
    Serial.print(DIRECT_READ(0,14));Serial.print(" ");
*/    Serial.println();
}

void checkGpioControl()
{
    
    GPEC|=0b00000000000000000101000000000000;       // 12,14 input mode
    GPES|=0b00000000000000000010000000000000;       // 13 output mode

    while(1){

    GPOS|=0b00000000000000000010000000000000;       // 13 high
    delay(1);showPins();delay(1000);

    GPOC|=0b00000000000000000010000000000000;       // 13 low
    delay(1);showPins();delay(1000);
    }
}


void setup()                    
{
    Serial.begin(115200);delay(1000);
    Serial.println("\nstart");delay(1000);

    pinMode(LED,OUTPUT);
    
    capaKeys.init(SAMPLES,COMMON,keys,KEYNB);
    
    //while(1){capaKeys.esp_count();Serial.println(capaKeys.cntk[0]);delay(50);}
    
    capaKeys.calibrate();
}

void loop()                    
{
    delay(100);
    unsigned long beg=micros(); 

    capaKeys.capaKeysCheck();

    unsigned long end=micros()-beg;

    for(uint8_t i=0;i<2;i++){
        capaKeys.capaKeysGet(i);
        Serial.print(capaKeys.lowTresh[i]);Serial.print("/");Serial.print(capaKeys.highTresh[i]);
        Serial.print(" ");Serial.print(capaKeys.totalMem[i]);Serial.print(" ");
        uint16_t c=capaKeys.cntk[i];if(c<capaKeys.meank[i]){c=capaKeys.meank[i];}
        Serial.print(" ");Serial.print(capaKeys.cntk[i]);Serial.print("-");Serial.print(capaKeys.meank[i]);
        Serial.print("=");Serial.print((uint16_t)(c-capaKeys.meank[i]));
        Serial.print(" ");Serial.print(loo+capaKeys.keyVal[i]*LOO);
        Serial.print(" ");Serial.print(capaKeys.keyChge[i]);
        if(i<KEYNB-1){Serial.print(" | ");}
    }
    
    Serial.print("   t=");Serial.println(end);

    digitalWrite(LED,capaKeys.keyVal[0]&0x01);
    
}
