
#include <nRF24L01.h>
#include "nrf24l01p.h"
#include "nrfconst.h"

Nrfp nrfp;

#define CE_PIN  9
#define CSN_PIN 10


unsigned long cnt=0;
unsigned long cntko=0;
unsigned long time_beg;
unsigned long time_end;
uint8_t pipe;
uint8_t pldLength;

long readTo=0;

byte message[MAX_PAYLOAD_LENGTH+1]={"ABCDEFGHIJKLMNOPQRSTUVWXYZ012345"};

/* prototypes */

void timeStat(unsigned long* timeS0,unsigned long* timeS,byte* stat,char* lib);
void hprint(byte* stat);
void pingpong();
void broadcast();
char getch();
 
void setup() {
  
  Serial.begin(115200);

  nrfp.ce_pin=9;
  nrfp.csn_pin=10;  
  nrfp.hardInit();
  
  nrfp.channel=1;

  nrfp.mode=NRF_MODE;
  nrfp.r1_addr=(byte*)P1_ADDR;  // adresse concentrateur + '1'
  nrfp.tr_addr=(byte*)TX_ADDR;  // adresse concentrateur + n° périphérique
                                // adresse broadcast si balise
  nrfp.br_addr=(byte*)BR_ADDR;  // adresse commune de broadcast  

  nrfp.config();

  
  bool menu=true;
  while(1){
    if(menu){
      Serial.println("start p=pingpong b=broadcast ");
      menu=false;
    }
    char a=getch();
    switch(a){
      case 'p':pingpong();menu=true;break;
      case 'b':broadcast();menu=true;break;
      default:break;
    }
  }
}

void loop() {
/*  
  cnt++;
  sprintf(message,"%05d",cnt);
  message[strlen(message)]='*';
  time_beg = micros();

  nrfp.dataWrite(message);

  Serial.print((char*)message);
  
  
  while(nrfp.transmitting()){}
  
  //Serial.print("...transmitted...");
  readTo=0;
  while(!nrfp.available()){   
    readTo=10000-micros()+time_beg;
    if(readTo<0){break;}
  }
  
  if(readTo<0){cntko++;Serial.print(cntko);Serial.println(" time out");}
  else{
  
    nrfp.dataRead(message);
  
    time_end=micros();
    Serial.print(" received ");  
    Serial.print((char*)message);Serial.print(" in:");
    Serial.print(time_end - time_beg); 
    Serial.println("us");
    
  }

  delay(2000);
*/
} 

void pingpong()
{
  while (getch()!=(char)27){
        
    
    cnt++;
    sprintf(message,"%05d",cnt);
    message[5]='*';
    time_beg = micros();

    nrfp.dataWrite(message,'A',MAX_PAYLOAD_LENGTH);

    Serial.print((char*)message);

    int sttr=1;
    while(sttr>0){sttr=nrfp.transmitting();

      switch(sttr){
        case 0:Serial.print("...transmitted...");
                readTo=0;
                while(!nrfp.available() && (readTo>=0)){readTo=10000-micros()+time_beg;}
                if(readTo<0){cntko++;Serial.print(" time out (");Serial.print(cntko);Serial.println(")");}
                else{
                  nrfp.dataRead(message,&pipe,&pldLength);  
                  time_end=micros();
                  Serial.print(" received ");  
                  Serial.print((char*)message);
                  Serial.print(" p/l:");Serial.print(pipe);Serial.print("/");Serial.print(pldLength);
                  Serial.print(" in:");
                  Serial.print(time_end - time_beg); 
                  Serial.println("us");
                }break;
        case -1:Serial.println("...MAX_RT...");break;
        default:break;
      }
    }
    delay(2000);
  } 
}


char getch()
{
  if(Serial.available()){
    return Serial.read();
  }
  return 0;
}

void broadcast()
{
  time_beg = micros();
  byte regw;
  
  nrfp.regWrite(TX_ADDR,nrfp.br_addr,ADDR_LENGTH);
  
  cnt++;
  sprintf(message,"%05d",cnt);
  message[strlen(message)+1]='\0';  
  message[strlen(message)]='+';
  
  nrfp.dataWrite(message,'N',6);
  
  Serial.print((char*)message);
  
  while(nrfp.transmitting()){}

  time_end=micros();
  Serial.print(" transmitted ");  
  Serial.print((char*)message);Serial.print(" in:");
  Serial.print(time_end - time_beg); 
  Serial.println("us");

}

