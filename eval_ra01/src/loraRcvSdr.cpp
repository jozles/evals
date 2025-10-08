#include "loraRcvSdr.h"
#include <SPI.h>
#include <LoRa.h> 

#define PWR 7

#ifdef SENDER
#define SDRCV "Sender"
int pot = A0;
#endif    // SENDER

#ifdef RCVR
#define SDRCV "Receiver"
String inString = "";    // string to hold input

#endif    // RCVR

uint8_t led = 4;
int val=0;

unsigned long t1=0;
unsigned long t2=0;
unsigned long t3=0;
unsigned long t4=0;

#define PAYLOADLENGTH 34
uint8_t valBuf[PAYLOADLENGTH];
uint32_t loopCnt=0;

void blink(uint8_t num)
{
  if(num!=0){
    for(uint8_t i=0;i<num;i++){digitalWrite(led,HIGH);delay(20);digitalWrite(led,LOW);delay(800);}
  }
}

void setup() {

#ifdef SENDER
  pinMode(pot,INPUT);
#endif

  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  delay(3000);
  digitalWrite(led,LOW);
  delay(1000);
  blink(3);
  delay(3000);


  digitalWrite(PWR,HIGH);   // power off
  pinMode(PWR,OUTPUT);      // power off
  digitalWrite(PWR,LOW);    // power on
  delay(100);

  Serial.begin(115200);
  while (!Serial);  
  Serial.print("LoRa ");Serial.println(SDRCV);
  if (!LoRa.begin(433E6)) { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1){blink(1); delay(500);};
  }
  Serial.println(" ok");
  
  
}

void loop()
{
  t1=micros();

#ifdef RCVR 
  /*
  inbufPtr=0;
  *inbuf='\0';
  */
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  
  uint8_t charCnt=0;
    
  if (packetSize) { 
    Serial.print("packetSize=");Serial.println(packetSize);
        
    while (LoRa.available()){

    //inString += (char)inChar;
    //val=inString.toInt();       
      
      int inChar = LoRa.read();  // Lora.read() returns uint8_t !?    
      Serial.print(inChar);Serial.print('_');
      if(charCnt<PAYLOADLENGTH){valBuf[charCnt]=inChar;}
      charCnt++;
    } 
  }
  LoRa.packetRssi();    
  if(charCnt!=0){
    Serial.print(' ');Serial.print(charCnt);Serial.print(' ');
    loopCnt=valBuf[PAYLOADLENGTH-2]+(uint32_t)valBuf[PAYLOADLENGTH-3]*256+(uint32_t)valBuf[PAYLOADLENGTH-4]*256*256+(uint32_t)valBuf[PAYLOADLENGTH-5]*256*256*256;
    Serial.println(loopCnt);
    if(valBuf[PAYLOADLENGTH-1]==5 && charCnt==packetSize){blink(valBuf[0]);delay(1000);blink(5);}
    else {blink(1);delay(1000);}
  }
    //analogWrite(led, val);
    //inString = ""; 
#endif // RCVR

#ifdef SENDER 
  
  LoRa.beginPacket();  
  //int val = map(analogRead(pot),0,1024,0,255);
  
  t2=micros();

  valBuf[0]=val;
  for(int i=0;i<PAYLOADLENGTH-5;i++){valBuf[i+1]=i;}
  valBuf[PAYLOADLENGTH-2]=(uint8_t)loopCnt;
  valBuf[PAYLOADLENGTH-3]=(uint8_t)(loopCnt>>8);
  valBuf[PAYLOADLENGTH-4]=(uint8_t)(loopCnt>>16);
  valBuf[PAYLOADLENGTH-5]=(uint8_t)(loopCnt>>24);
  valBuf[PAYLOADLENGTH-1]=5;

  LoRa.write(valBuf,PAYLOADLENGTH);
  t3=micros();
  LoRa.endPacket();
  t4=micros();
  
  blink(val);delay(1000);blink(5);
  val ++;val&=0x03;if(val==0){val=1;}
  Serial.print(loopCnt);Serial.print(' ');
  Serial.print(t2-t1);Serial.print(' ');Serial.print(t3-t2);Serial.print(' ');Serial.println(t4-t3);
  delay(2000);
  loopCnt++;
  //analogWrite(led, val);
  /*delay(10000);
  Serial.println("next");
  blink(1);*/
#endif // SENDER

}
