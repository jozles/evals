#include <SPI.h>
#include "mcp4901.h"




uint8_t led = LED;

uint16_t volts=0;
uint16_t vAdj=14;
float vFactor=2004;
uint16_t aAdj=0;
uint16_t aFactor=1;
uint16_t amps=0;

uint16_t voltsB,voltsE,ampsB,ampsE,stepsN,dly;

uint8_t vbuf=0;   // 1 buffered ; 0 unbuffered
uint8_t vga=1;    // 1 gain 1 ; 0 gain 2
uint8_t vshdn=1;  // 1 active ; 0 shutdown

void blink(uint8_t num)
{
  if(num!=0){
    for(uint8_t i=0;i<num;i++){digitalWrite(led,HIGH);delay(20);digitalWrite(led,LOW);delay(50);}
  }
}

void spi_Write(byte* data,uint8_t port,uint8_t pin)
{
    bitClear(PORT_CS,pin);
    SPI.transfer(*data);
    SPI.transfer(*(data+1));
    bitSet(PORT_CS,pin);
}
char getCh()
{
  while(!Serial.available()){}
  return Serial.read();
}

char peekCh()
{
  if(Serial.available()){return Serial.read();}
  else {return '\0';}
}

void getValSpec(const char* type,uint16_t* v)
{
  *v=0;
  char c='\0';
  while(c!=*type){c=getCh();Serial.print(c);}

  while(c!=0x1b){
    while((c<'0' || c>'9') && c!=0x1b){c=getCh();}
    if(c!=0x1b){Serial.print(c);c-='0';*v*=10;*v+=c;}
  }
}

uint16_t getValue()
{
  uint16_t v=0;
  char c='\0';
  while(c!=0x1b){
    while((c<'0' || c>'9') && c!=0x1b){c=getCh();}
    if(c!=0x1b){Serial.print(c);c-='0';v*=10;v+=c;}
  }
  return v;
}

float getFloatValue()
{
  float v=0;
  char c='\0';
  uint8_t decimal=0;
  while(c!=0x1b){
    while(((c<'0' || c>'9') && c!='.') && c!=0x1b){c=getCh();}
    if(c!=0x1b){
      Serial.print(c);
      if(c=='.' && decimal==0){decimal=1;}
      else {
        //Serial.print(c);
        c-='0';
        if(decimal==0){
          v*=10;v+=c;
        } else {
          float d=0.1;
          for(uint8_t i=0;i<decimal;i++){d*=0.1;}
          v+=c*d;
          decimal++;
        }
      }
    }
  }
  return v;
}

uint16_t getVolts()
{
  uint16_t v=0;
  getValSpec((const char*)"V",&v);
  return v;
}

uint16_t getAmps()
{
  uint16_t v=0;
  getValSpec((const char*)"A",&v);
  return v;
}

void outVolts( uint16_t val)
{
  byte value[2];
  val+=vAdj;
  value[0]=val>>8;
  value[1]=val&0xff;
  
  value[0] |= (vbuf<<(BUF-8)) | (vga<<(GA-8)) | (vshdn<<(SHDN-8)) ;

  spi_Write(value,PORT_CSV,CSV_PIN);
}

void outAmps( uint16_t val)
{
  byte value[2];
  val+=aAdj;
  value[0]=val>>8;
  value[1]=val&0xff;
  
  value[0] |= (vbuf<<(BUF-8)) | (vga<<(GA-8)) | (vshdn<<(SHDN-8)) ;

  spi_Write(value,PORT_CSA,CSA_PIN);
}

void ramp(char what,uint16_t valB,uint16_t valE,uint16_t stepsN,uint32_t dly)
{
  int16_t step=0;

  if(valB<valE){step=(valE-valB)/stepsN;}
  else {step=-((valB-valE)/stepsN);}
      /*  
        Serial.print(" valB=");Serial.print(valB);
        Serial.print(" valE=");Serial.print(valE);
        Serial.print(" step=");Serial.println(step);
      */ 
        uint16_t val=valB;
        for(uint8_t i=0;i<=stepsN;i++){
          //Serial.print(val);Serial.print(' ');
          if(what=='V'){outVolts(val);}
          else {outAmps(val);}
          delay(dly);
          val+=step;
        }
}

void setup() {

  Serial.begin(115200);
  Serial.print("+start MCP4901 ");delay(10);

  POWER_ON_MCP

  delay(100);

  CSA_INIT
  CSV_INIT
  CSV_HIGH
  CSA_HIGH
  SPI_START
  SPI_INIT
  
  uint16_t val=0; //0x0db0;      //x0ffe;
  outVolts(val);

  Serial.print("(");Serial.print(vAdj);
  Serial.print(") aAdj Volts ? ");
  vAdj=getValue();

  Serial.print("  (");Serial.print(aAdj);
  Serial.print(") Adj Amps ? ");
  aAdj=getValue();

  Serial.println(" ok");

}

void loop()
{
  blink(2);
  delay(1000);
  float bid;

  Serial.print("\n\n\rquoi (F fixe / Vv rampe V / Aa rampe A)?");

  char quoi='\0';
  quoi=getCh();Serial.println(quoi);

  switch(quoi){
    
    case 'F':
      Serial.print("\nVolts ? ");
      bid=getFloatValue();
      Serial.print("  ");Serial.print(bid);
      volts=(uint16_t)(bid*vFactor);

      //Serial.print("  ");Serial.print(bid*1000/vFactor);
      //volts=(uint16_t)(getFloatValue()*1000*vFactor);
      Serial.print(" ->");Serial.print(volts);
      Serial.print("\n\rmAmps ? ");
      amps=getValue();
      
      
      outAmps(amps);
      outVolts(volts);
    break;

    case 'V':
    {
      voltsB=0;voltsE=0;stepsN=0;dly=0;

      Serial.print("\ndurée step ?");
      dly=getValue();
      Serial.print("\namps maxi ?");
      amps=getValue();
      Serial.print("\nvolts beg ? ");
      bid=getFloatValue();
      voltsB=(uint16_t)(bid*vFactor);
      //voltsB=(uint16_t)(getFloatValue()*vFactor);
      Serial.print("\nvolts end ? ");
      bid=getFloatValue();
      voltsE=(uint16_t)(bid*vFactor);
      Serial.print("\nnbre steps ? ");
      stepsN=getValue();

      Serial.println();
      
      if(stepsN==0){break;}
      else { 
        outAmps(amps);
        quoi='\0';      
        while(quoi!=0x1b){
          ramp('V',voltsB,voltsE,stepsN,dly);      
          outVolts(0);delay(dly);
          quoi=peekCh();
        }
      }
    }
    break;

    case 'v':
      ramp('V',voltsB,voltsE,stepsN,dly);
    break;

   case 'A':
    {
      ampsB=0;ampsE=0;stepsN=0;dly=0;

      Serial.print("\ndurée step mS ?");
      dly=getValue();
      Serial.print("\nvolts ?");
      volts=getValue();
      Serial.print("\namps beg ? ");
      ampsB=getValue();
      Serial.print("\nvamps end ? ");
      ampsE=getValue();
      Serial.print("\nnbre steps ? ");
      stepsN=getValue();

      Serial.println();

      outAmps(ampsB);
      outVolts(volts);
      
      if(stepsN==0){break;}
      else {
         while(quoi!=0x1b){
          ramp('A',ampsB,ampsE,stepsN,dly);
          outAmps(0);delay(dly);
          quoi=peekCh();
         }
      }
    }
    break;

    case 'a':
      ramp('A',ampsB,ampsE,stepsN,dly);
    break;

    default:
    break;

  }

}

