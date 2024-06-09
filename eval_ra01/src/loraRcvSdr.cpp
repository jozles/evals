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

#define INBUFLEN 100
char inbuf[INBUFLEN];
uint8_t inbufPtr=0;
#endif    // RCVR

uint8_t led = 4;
int val=0;

unsigned long t1=0;
unsigned long t2=0;
unsigned long t3=0;
unsigned long t4=0;

void blink(uint8_t num)
{
    for(uint8_t i=0;i<num;i++){digitalWrite(led,HIGH);delay(20);digitalWrite(led,LOW);delay(300);}
}

void setup() {
  
#ifdef SENDER
  pinMode(pot,INPUT);
#endif

  pinMode(led,OUTPUT);

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
  blink(3);
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
  
  if (packetSize) { 
    Serial.print("packetSize=");Serial.println(packetSize);
    // read packet    
    while (LoRa.available())
    {
      int inChar = LoRa.read();
/*
      inbuf[inbufPtr]=inChar;
      if(inbufPtr<(INBUFLEN-1)){inbufPtr++;}
*/      
    inString += (char)inChar;
    val = inString.toInt();       
    }
    inString = "";     
    LoRa.packetRssi();    
  
    Serial.print(val);Serial.print(' ');Serial.println(inString);
    //analogWrite(led, val);
    inString = ""; 
    blink(val);
  }
      
  
  /*inbuf[inbufPtr]='\0';  
  if(*inbuf!='\0'){Serial.println(inbuf);blink(1);}
  */
#endif

#ifdef SENDER 
  
  LoRa.beginPacket();  
  //int val = map(analogRead(pot),0,1024,0,255);
  
  t2=micros();

  LoRa.print(val);
  t3=micros();
  /*uint8_t mess[10];memcpy(mess,"hello",5);
  for(uint8_t i=0;i<5;i++){
    LoRa.write(*(mess+i));}
  */
  LoRa.endPacket();
  t4=micros();
  
  blink(val);
  val ++;val&=0x03;if(val==0){val=1;}
  Serial.print(t2-t1);Serial.print(' ');Serial.print(t3-t2);Serial.print(' ');Serial.println(t4-t3);
  delay(1000);
  //analogWrite(led, val);
  /*delay(10000);
  Serial.println("next");
  blink(1);*/
#endif

}
