#include <digitalWriteFast.h>

#define NBPORTS 3
#define NBPPPORT 8
#define NBPIN   NBPORTS*NBPPPORT

uint8_t ports[NBPORTS];
byte mask[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

uint8_t v=0,l=0;

uint8_t pin=6;

void setup() {
  Serial.begin(112500);
  Serial.println("ready");
}

void digitalSet(uint8_t pin,bool level)
{
  byte scratchPort;
  // read byte
  scratchPort=ports[pin/NBPPPORT];
  // modify byte
  scratchPort &= ~mask[pin%NBPPPORT];
  if(level){scratchPort |= mask[pin%NBPPPORT];}
  // write it
  ports[pin/NBPPPORT]=scratchPort;
}



char getch()
{
  if(Serial.available()){
    return Serial.read();
  }
  return 0;
}


void loop() {

// show pins
for(int i=0;i<NBPORTS;i++){
  for(int j=0;j<NBPPPORT;j++){
    Serial.print((ports[i]>>(j))&0x01);Serial.print(" ");
}}Serial.println();

// get new value
v=0;l=0;
Serial.print("enter pin number (0-9)");
while(v==0){v=getch();}v-=48;
Serial.println(v);
Serial.print("level (0-1) ");
while(l==0){l=getch();}l-=48;
Serial.println(l);

uint8_t v=4,l=1;
uint16_t nbLoop=10000;
char a=l;
uint16_t b=0;
uint16_t k;
long offset=0;
long time_end=0;
long time_beg;

  time_beg=micros();  
  for(k=0;k<nbLoop;k++){
    b=k&0x0001;

    if(b==0){
      a=(a>>k)+1;}
    else{
      a=(a>>k)+2;}
  }
  offset=micros();
  offset-=time_beg;
Serial.print(a);Serial.print(" offset time=");Serial.println((long)offset);

  a=l;
  time_beg=micros();
  for(k=0;k<nbLoop;k++){
    b=k&0x0001;

    if(b==0){
      digitalWrite(4,HIGH);      
      a=(a>>k)+1;}
    else{
      digitalWrite(4,LOW);
      a=(a>>k)+2;}
  }
time_end=micros();    
Serial.print(a);Serial.print("  total time=");Serial.println(time_end-time_beg);
Serial.print(a);Serial.print(" durat=");Serial.println((float)(time_end-time_beg-offset)/nbLoop);
}
