#include <SPI.h>
#include "mcp4901.h"




uint8_t led = LED;

uint16_t volts=0;
uint16_t amps=0;

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

void getValue(const char* type,uint16_t* v)
{
  *v=0;
  char c='\0';
  while(c!=*type){c=getCh();Serial.print(c);}

  while(c!=0x1b){
    while((c<'0' || c>'9') && c!=0x1b){c=getCh();}
    if(c!=0x1b){Serial.print(c);c-='0';*v*=10;*v+=c;}
  }
}

uint16_t getVolts()
{
  uint16_t v=0;
  getValue((const char*)"V",&v);
  return v;
}

uint16_t getAmps()
{
  uint16_t v=0;
  getValue((const char*)"A",&v);
  return v;
}

void outVolts( uint16_t val)
{
  byte value[2];
  value[0]=val>>8;
  value[1]=val&0xff;
  
  value[0] |= (vbuf<<(BUF-8)) | (vga<<(GA-8)) | (vshdn<<(SHDN-8)) ;

  spi_Write(value,PORT_CSV,CSV_PIN);
}

void outAmps( uint16_t val)
{
  byte value[2];
  value[0]=val>>8;
  value[1]=val&0xff;
  
  value[0] |= (vbuf<<(BUF-8)) | (vga<<(GA-8)) | (vshdn<<(SHDN-8)) ;

  spi_Write(value,PORT_CSA,CSA_PIN);
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
  
  uint16_t val=0x07FF; //0x0db0;      //x0ffe;

  outVolts(val);

  Serial.println(" ok");
}

void loop()
{
  blink(2);
  delay(1000);

  Serial.print("\nvolts ? ");
  volts=getVolts();
  
  Serial.print("\namps ? ");
  amps=getAmps();

  outAmps(amps);
  outVolts(volts);
}

