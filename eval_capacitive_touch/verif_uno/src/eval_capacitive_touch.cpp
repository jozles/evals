#include <Arduino.h>

#define COMMON  4
#define TOUCH1  2
#define TOUCH2  6
#define LED 13

#if defined(__AVR__)
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASk(pin)             (digitalPinToBitMask(pin))
#define IO_REG_TYPE uint8_t
#define DIRECT_READ(base, mask)         (((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) &= ~(mask), (*((base)+2)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+2)) |= (mask))
#endif

IO_REG_TYPE	sBit;
volatile IO_REG_TYPE *sReg; 

IO_REG_TYPE	rBit1;
IO_REG_TYPE	rBit2;
volatile IO_REG_TYPE *rReg1;
volatile IO_REG_TYPE *rReg2;

uint16_t cnt1,mincnt1,maxcnt1;
uint16_t cnt2,mincnt2,maxcnt2;
uint16_t highTresh1,lowTresh1;
uint16_t highTresh2,lowTresh2;

char prev1='O',prev2='O';
unsigned long k1=0, k2=0;
unsigned long debounce1=0, debounce2=0;
#define DEBOUNCE 50
#define SAMPLES 5

void count(uint8_t samples,uint16_t* cnt1, uint16_t* cnt2);
void calibrate();

void setup()                    
{
   
    Serial.begin(115200);
    Serial.println("\nstart");

    sBit = PIN_TO_BITMASk(COMMON);
    sReg = PIN_TO_BASEREG(COMMON);
    rBit1 = PIN_TO_BITMASk(TOUCH1);
    rReg1 = PIN_TO_BASEREG(TOUCH1);
    rBit2 = PIN_TO_BITMASk(TOUCH2);
    rReg2 = PIN_TO_BASEREG(TOUCH2);

    pinMode(LED,OUTPUT);

    pinMode(TOUCH1,INPUT);
    pinMode(TOUCH2,INPUT);
    pinMode(COMMON,OUTPUT);
    digitalWrite(COMMON,LOW);

    calibrate();
}

void calibrate()
{
    unsigned long beg=micros(); 
    cnt1=0;
    cnt2=0;
    mincnt1=9999;maxcnt1=0;
    mincnt2=9999;maxcnt2=0;
    #define CALIB 10
    count(SAMPLES,&cnt1,&cnt2);
    count(SAMPLES,&cnt1,&cnt2);
    for(uint8_t i=0;i<CALIB;i++){
        count(SAMPLES,&cnt1,&cnt2);
        if(cnt1<mincnt1){mincnt1=cnt1;}
        if(cnt1>maxcnt1){maxcnt1=cnt1;}
        if(cnt2<mincnt2){mincnt2=cnt2;}
        if(cnt2>maxcnt2){maxcnt2=cnt2;}
        Serial.print(i);Serial.print(" ");Serial.print(cnt1);Serial.print(" ");Serial.print(mincnt1);Serial.print(" ");Serial.print(maxcnt1);
        Serial.print(" | ");Serial.print(cnt1);Serial.print(" ");Serial.print(mincnt1);Serial.print(" ");Serial.println(maxcnt1);
    }
    lowTresh1=(maxcnt1-mincnt1)+(maxcnt1-mincnt1)/5;
    highTresh1=lowTresh1+lowTresh1/4;

    lowTresh2=(maxcnt2-mincnt2)+(maxcnt2-mincnt2)/5;
    highTresh2=lowTresh2+lowTresh2/4;

    Serial.print("low=");Serial.print(lowTresh1);Serial.print("/");Serial.print(lowTresh2);
    Serial.print(" high=");Serial.print(highTresh1);Serial.print("/");Serial.print(highTresh2);
    Serial.print(" t=");Serial.println(micros()-beg);
}

void count(uint8_t samples,uint16_t* cnt1, uint16_t* cnt2)
{ 
    *cnt1=0;
    *cnt2=0;

    for(uint8_t i=0;i<samples;i++){
    
        DIRECT_WRITE_LOW(sReg, sBit);               // common low    
        DIRECT_MODE_INPUT(rReg1, rBit1);            // receive input
        DIRECT_MODE_OUTPUT(rReg1, rBit1);           // receive output
        DIRECT_WRITE_LOW(rReg1, rBit1);             // receive low
        
        DIRECT_MODE_INPUT(rReg2, rBit2);            // receive input
        DIRECT_MODE_OUTPUT(rReg2, rBit2);           // receive output
        DIRECT_WRITE_LOW(rReg2, rBit2);             // receive low
        
        //delayMicroseconds(4);
        noInterrupts();
        DIRECT_MODE_INPUT(rReg1, rBit1);            // receive input (capacitor is empty)
        DIRECT_MODE_INPUT(rReg2, rBit2);            // receive input (capacitor is empty)
        DIRECT_WRITE_HIGH(sReg, sBit);              // begin filling

        bool r1=DIRECT_READ(rReg1, rBit1);
        bool r2=DIRECT_READ(rReg2, rBit2);
        while(!r1 && !r2){if(!r1){(*cnt1)++;r1=DIRECT_READ(rReg1, rBit1);} if(!r2){(*cnt2)++;r2=DIRECT_READ(rReg2, rBit2);}}
    
        interrupts();
        DIRECT_WRITE_HIGH(sReg, sBit);              // common high
        
        DIRECT_WRITE_HIGH(rReg1, rBit1);            // receive high    
        DIRECT_MODE_OUTPUT(rReg1, rBit1);           // receive output
        DIRECT_WRITE_HIGH(rReg1, rBit1);            // receive high
        
        DIRECT_WRITE_HIGH(rReg2, rBit2);            // receive high    
        DIRECT_MODE_OUTPUT(rReg2, rBit2);           // receive output
        DIRECT_WRITE_HIGH(rReg2, rBit2);            // receive high
        
        noInterrupts();
        DIRECT_MODE_INPUT(rReg1, rBit1);            // receive input (capacitor is empty)
        DIRECT_MODE_INPUT(rReg2, rBit2);            // receive input (capacitor is empty)
        DIRECT_WRITE_LOW(sReg, sBit);               // begin filling    

        while(DIRECT_READ(rReg1, rBit1)){(*cnt1)++;}

        interrupts();

    }
}

void loop()                    
{
    delay(30);
    unsigned long beg=micros(); 

    count(SAMPLES,&cnt1,&cnt2);

    unsigned long end=micros()-beg;
    if(cnt1<mincnt1){mincnt1=cnt1;}cnt1-=mincnt1;
    if(cnt2<mincnt2){mincnt2=cnt2;}cnt2-=mincnt2;

    Serial.print(lowTresh1);Serial.print("/");Serial.print(highTresh1);Serial.print("-");Serial.print(cnt1);
    
    if((millis()-debounce1)>DEBOUNCE){
        if(cnt1>highTresh1){Serial.print(" ON  ");if(prev1!='I'){prev1='I';k1++;debounce1=millis();}}
        else if(cnt1<lowTresh1){Serial.print(" OFF ");if(prev1!='O'){prev1='O';debounce1=millis();}}
        else {Serial.print("     ");}
    }

    Serial.print(lowTresh2);Serial.print("/");Serial.print(highTresh2);Serial.print("-");Serial.print(cnt2);

    if((millis()-debounce2)>DEBOUNCE){
        if(cnt2>highTresh2){Serial.print(" ON ");if(prev2!='I'){prev2='I';k1++;debounce2=millis();}}
        else if(cnt2<lowTresh2){Serial.print(" OFF ");if(prev2!='O'){prev2='O';debounce2=millis();}}
        else {Serial.print("     ");}
    }

    Serial.print("   t=");Serial.println(end);

    digitalWrite(LED,k1&0x01);
    
}
