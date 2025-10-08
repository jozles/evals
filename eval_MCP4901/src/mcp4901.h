#ifndef _MCP4901_H
#define _MCP4901_H

#define LED        4          // pin pour Led (13 pris par le clk du SPI)

#define CE_PIN     1          // pin pour CE du nrf
#define CSN_PIN    2         // pin pour CS du SPI
#define CLKPIN     5
#define MISOPIN    4
#define MOSIPIN    3

#define PORT_PWR_MCP PORTD
#define DDR_PWR_MCP DDRD
#define BIT_PWR_MCP 7
#define POWER_ON_MCP bitSet(DDR_PWR_MCP,BIT_PWR_MCP);bitClear(PORT_PWR_MCP,BIT_PWR_MCP);
#define POWER_OFF_MCP bitSet(PORT_PWR_MCP,BIT_PWR_MCP);

#define PORT_CSN PORTB
#define PORT_CE  PORTB
#define PORT_MOSI PORTB
#define PORT_MISO  PORTB
#define PORT_CLK PORTB
#define DDR_CSN DDRB
#define DDR_CE  DDRB
#define DDR_MOSI DDRB
#define DDR_MISO  DDRB
#define DDR_CLK DDRB

#define CE_INIT   bitSet(DDR_CE,CE_PIN); //pinMode(CE_PIN,OUTPUT);
#define CE_OFF    bitClear(DDR_CE,CE_PIN); //pinMode(CE_PIN,INPUT);
#define CSN_INIT  bitSet(DDR_CSN,CSN_PIN); //pinMode(CSN_PIN,OUTPUT);
#define CSN_OFF   bitClear(DDR_CSN,CSN_PIN); //pinMode(CSN_PIN,INPUT);
#define CE_HIGH   bitSet(PORT_CE,CE_PIN); //digitalWrite(CE_PIN,HIGH);
#define CE_LOW    bitClear(PORT_CE,CE_PIN); //digitalWrite(CE_PIN,LOW);
#define CSN_HIGH  bitSet(PORT_CSN,CSN_PIN); //digitalWrite(CSN_PIN,HIGH);
#define CSN_LOW   bitClear(PORT_CSN,CSN_PIN); //digitalWrite(CSN_PIN,LOW);


#define SPI_INIT  SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));
#define SPI_START SPI.begin();
#define SPI_OFF   SPI.end();bitClear(DDR_MOSI,MOSI_PIN);bitClear(DDR_CLK,CLK_PIN); //digitalWrite(CSN_PIN,LOW);


#define BUF 14
#define GA  13
#define SHDN 12

#endif // _MCP4901_H


