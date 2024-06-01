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
    
    //capaKeys.calibrate();
}

void loop()                    
{
    delay(100);
    unsigned long beg=micros(); 

    //capaKeys.capaKeysCheck();

    unsigned long end=micros()-beg;

    pinMode(capaKeys.ktouch[0],INPUT);
    pinMode(13,INPUT);
    uint16_t rB=capaKeys.rBit[1];
    Serial.print("\n(rb=");Serial.print(rB,BIN);Serial.println(')');

    uint16_t a;
    unsigned long t1=micros(),t2;

noInterrupts();

#define NBL 100
    for(uint8_t i=0;i<NBL;i++){
        a=GPI&rB;
    }

    t2=micros();
    Serial.print(NBL);Serial.print(" fois a=GPI&rB ");
    Serial.print("\na=");Serial.print(a,BIN);Serial.print(" t=");Serial.print(t2-t1);Serial.println("uS");

    uint16_t l1=0,l2=0,h1=0,h2=0;

    t1=micros();
    //ll1: if((GPI&rB) == 0){l1++;goto ll1;} 
    //while((GPI&rB) == 0){l1++;}
    for(uint16_t i=0;i<MAXCOUNT;i++){           // attente low
        if((GPI&rB) == 0){break;}
        l1++;
    }
    //hh1: if((GPI&rB) != 0){h1++;goto hh1;} 
    //while((GPI&rB) != 0){h1++;}
    for(uint16_t i=0;i<MAXCOUNT;i++){           // attente high 
        if((GPI&rB) != 0){break;}
        h1++;
    }
    //ll2: if((GPI&rB) == 0){l2++;goto ll2;} 
    //while((GPI&rB) == 0){l2++;}
    for(uint16_t i=0;i<MAXCOUNT;i++){           // attente low  (high est mesuré dans l2)
        if((GPI&rB) == 0){break;}
        l2++;
    }
    //hh2: if((GPI&rB) != 0){h2++;goto hh2;} 
    //while((GPI&rB) != 0){h2++;}
    for(uint16_t i=0;i<MAXCOUNT;i++){           // attente high  (low est mesuré dans h2)
        if((GPI&rB) != 0){break;}
        h2++;
    }
    t2=micros();
    
    Serial.println(" 4 fois (l1,l2,h1,h2) while((GPI&rB) == 0){xy++;} ");

    //Serial.print("\n rB=");Serial.println(capaKeys.rBit[0],BIN);
    Serial.print("\n l1=");Serial.print(l1);Serial.print(" h1=");Serial.print(h1);
    Serial.print("\n l2=");Serial.print(l2);Serial.print(" h2=");Serial.print(h2);
    Serial.print("\n t=");Serial.print(t2-t1);Serial.println("uS");
    
    #define NBSPL 500
    uint32_t b[NBSPL];memset(b,0x00,NBSPL*sizeof(uint32_t));
    for(uint16_t i=0;i<NBSPL;i++){
        b[i]=GPI; //&capaKeys.rBit[0];
    }
    
    #define FRA 4
    for(uint16_t i=0;i<NBSPL/FRA;i++){
        for(uint8_t j=0;j<FRA;j++){
            Serial.print(b[i*FRA+j],BIN);Serial.print(" ");
        }
        Serial.println();
    }

    interrupts();
    while(1){delay(1);}

    

    t2=micros();
    Serial.print("\na=");Serial.print(a,BIN);Serial.print(" t=");Serial.println(t2-t1);while(1){delay(1);}

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
