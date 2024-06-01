
#include <Servo.h>

Servo servo1;  // create servo object to control a servo
Servo servo2;
// twelve servo objects can be created on most boards

#define PIN1 9
#define PIN2 8

#define MIN1 90
#define MAX1 180
int pos1 = MAX1;
int min1 = MIN1;
int max1 = MAX1;
bool dir1=0;
uint32_t dly1=25;

#define MIN2 0
int pos2 = MIN2;
int min2 = MIN2;
int max2 = 180;
bool dir2=0;
uint32_t dly2=25;

void setup() {
  Serial.begin(115200);Serial.println("start");
  
  servo1.attach(PIN1);  // attaches the servo on pin 9 to the servo object
  servo2.attach(PIN2);  // attaches the servo on pin 9 to the servo object  
}

/*
void loop() {
  for (pos1 = min1; pos1 <= max1; pos1++) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(dly1);                       // waits 15ms for the servo to reach the position
  }
  for (pos1 = max1; pos1 >= min1; pos1--) { // goes from 180 degrees to 0 degrees
    servo1.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(dly1);                       // waits 15ms for the servo to reach the position
  }
}
*/

void servoWrite(uint8_t servo, int pos)
{
  switch(servo){
    case 1:servo1.write(pos);break;
    case 2:servo2.write(pos);break;
    default:break;
  }
}

void servoStep(bool* dir,int* pos,int min,int max,uint32_t dly,uint8_t servo,uint8_t factor)
{
  //Serial.print(*dir);Serial.print(' ');Serial.print(min);Serial.print(' ');Serial.print(max);Serial.print(' ');
  if(*dir==0){
    *pos=*pos+factor;
    Serial.println(*pos);
    servoWrite(servo,*pos);
    if((*pos)>=max){*dir=1;}
    delay(dly);
  }
  if(*dir==1){
    *pos=*pos-factor;
    Serial.println(*pos);    
    servoWrite(servo,*pos);
    if((*pos)<=min){*dir=0;}
    delay(dly);
  }
}

void loop()
{
  if(Serial.available()>0){
    char a=Serial.read();
     while(Serial.available()==0){}
    a=Serial.read();
  }
  servoStep(&dir1,&pos1,min1,max1,dly1,1,1);
  if(pos2==(min2+1)){delay(1000);}
  servoStep(&dir2,&pos2,min2,max2,dly2,2,1);
  if(pos2==(max2-1)){pos2=min2;delay(1000);}
}
