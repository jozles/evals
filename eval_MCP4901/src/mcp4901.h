#ifndef _MCP4901_H
#define _MCP4901_H

#define LED        4          // pin pour Led (13 pris par le clk du SPI)

#define CSA_PIN    1          // pin pour CSA du nrf
#define CSV_PIN    2         // pin pour CS du SPI
#define CLKPIN     5
#define MISOPIN    4
#define MOSIPIN    3

#define PORT_PWR_MCP PORTD
#define DDR_PWR_MCP DDRD
#define BIT_PWR_MCP 7
#define POWER_ON_MCP bitSet(DDR_PWR_MCP,BIT_PWR_MCP);bitClear(PORT_PWR_MCP,BIT_PWR_MCP);
#define POWER_OFF_MCP bitSet(PORT_PWR_MCP,BIT_PWR_MCP);

#define PORT_CS PORTB
#define PORT_CSV PORTB
#define PORT_CSA  PORTB
#define PORT_MOSI PORTB
#define PORT_MISO  PORTB
#define PORT_CLK PORTB
#define DDR_CSV DDRB
#define DDR_CSA  DDRB
#define DDR_MOSI DDRB
#define DDR_MISO  DDRB
#define DDR_CLK DDRB

#define CSA_INIT   bitSet(DDR_CSA,CSA_PIN); //pinMode(CSA_PIN,OUTPUT);
#define CSA_OFF    bitClear(DDR_CSA,CSA_PIN); //pinMode(CSA_PIN,INPUT);
#define CSV_INIT  bitSet(DDR_CSV,CSV_PIN); //pinMode(CSV_PIN,OUTPUT);
#define CSV_OFF   bitClear(DDR_CSV,CSV_PIN); //pinMode(CSV_PIN,INPUT);
#define CSA_HIGH   bitSet(PORT_CSA,CSA_PIN); //digitalWrite(CSA_PIN,HIGH);
#define CSA_LOW    bitClear(PORT_CSA,CSA_PIN); //digitalWrite(CSA_PIN,LOW);
#define CSV_HIGH  bitSet(PORT_CSV,CSV_PIN); //digitalWrite(CSV_PIN,HIGH);
#define CSV_LOW   bitClear(PORT_CSV,CSV_PIN); //digitalWrite(CSV_PIN,LOW);


#define SPI_INIT  SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));
#define SPI_START SPI.begin();
#define SPI_OFF   SPI.end();bitClear(DDR_MOSI,MOSI_PIN);bitClear(DDR_CLK,CLK_PIN); //digitalWrite(CSV_PIN,LOW);


#define BUF 14
#define GA  13
#define SHDN 12

#endif // _MCP4901_H


