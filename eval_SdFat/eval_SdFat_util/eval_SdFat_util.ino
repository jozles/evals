#include "SdFat.h"
//#include "sdios.h"
//#include <sstream>
#include "FreeStack.h"

//#define SD_FAT_TYPE 1 // FAT16/FAT32

const uint8_t SD_CS_PIN = 4;
#define SPI_CLOCK SD_SCK_MHZ(8)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)

SdFat32 sd;
File32 file;

#define error(s) sd.errorHalt(&Serial, F(s))

//ArduinoOutStream cout(Serial);

void cidDmp();
void dumpStr(byte* a,int len);
void speedTest();

void setup() {
  Serial.begin(115200);

  sdInit();
  speedTest();
}

void loop() {
}

void speedTest()
{
  unsigned long t6,t5,t4,t3,t2,t1,t0,tc;
  
  #define LBUF 500
  byte buf[LBUF];
  memset(buf,'x',LBUF);
  dumpStr(buf,LBUF);
  #define FNAME "toto"  
  #define FSIZE LBUF

  file.remove(FNAME);

  Serial.print("\nstart test ");Serial.print((int)LBUF);Serial.println(" bytes file");

/* create file - contiguous clusters */

  tc=micros();

  if (!file.open(FNAME, O_RDWR | O_CREAT | O_TRUNC)) {
    error("open failed");
  }

// contiguous clusters
  file.truncate(0);
  if (!file.preAllocate(FSIZE)) {
    error("preAllocate failed");
  } 
  file.close();

/* open/write/close */  

  t0=micros();
  
  if (!file.open(FNAME, O_RDWR)) {
    error("open failed");
  }
  t1=micros();

  if (file.write(buf, LBUF) != LBUF) {
    error("write failed");
  }

  file.sync(); // wait write end
  t2=micros();

  file.close();
  t3=micros();

  if (!file.open(FNAME, O_RDWR)) {
    error("open failed");
  }

  if (file.read(buf, LBUF) != LBUF) {
    error("read failed");
  }

  file.close();
  t4=micros();

/* open/read/modify/write/close */

  if (!file.open(FNAME, O_RDWR)) {
    error("open failed");
  }

  if (file.read(buf, LBUF) != LBUF) {
    error("read failed");
  }

  memcpy(buf+16,"something",9);
  file.seek(0);
  if (file.write(buf, LBUF) != LBUF) {
    error("write failed");
  }
  file.sync();
  file.close();
  t5=micros();

/* open/read/write/close */
  
  if (!file.open(FNAME, O_RDWR)) {
    error("open failed");
  }

  if (file.read(buf, LBUF) != LBUF) {
    error("read failed");
  }

  file.seek(16);
  if(file.write("something",9) != 9){
    error("write failed");
  }
  file.sync();
  file.close();
  t6=micros();

// result 
  Serial.print("create/preallocate/close=");Serial.println(t0-tc);
  Serial.print("open=");Serial.print(t1-t0);Serial.print(" write=");Serial.print(t2-t1);Serial.print(" close=");Serial.println(t3-t2); 
  Serial.print("open/read/close=");Serial.println(t4-t3);
  Serial.print("open/read-modify-write/close=");Serial.println(t5-t4);
  Serial.print("open/read-write/close=");Serial.println(t6-t5);

  if (!file.open(FNAME, O_RDWR)) {error("open failed");}
  if (file.read(buf, LBUF) != LBUF) {error("read failed");}
  file.close();
  dumpStr(buf,LBUF);
}

void sdInit()
{
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
  }
  Serial.println("\nSD OK\n");

  Serial.print("FreeStack: ");Serial.println(FreeStack());
  Serial.print("Type is FAT");Serial.print((int)sd.fatType());

  uint32_t size = sd.card()->sectorCount();
  if (size == 0) {
    Serial.print("\nCan't determine the card size.\n");
    Serial.print("Try another SD card or reduce the SPI bus speed.\n");
    Serial.print("Edit SPI_SPEED in this program to change it.\n");
    while(1){delay(1);}
  }

  uint32_t sizeMB = 0.000512 * size + 0.5;
  Serial.print(" Card size: ");Serial.print(sizeMB);
  Serial.println(" MB (MB = 1,000,000 bytes)");
  Serial.print("Volume is FAT");Serial.print((int)(sd.vol()->fatType()));
  Serial.print(", Cluster size (bytes): ");Serial.println(sd.vol()->bytesPerCluster());

  cidDmp();
}


void cidDmp() {
  cid_t cid;
  if (!sd.card()->readCID(&cid)) {error("readCID failed");
  }
  Serial.print("Manufacturer ID: ");Serial.println(int(cid.mid),HEX);
  Serial.print("OEM ID: ");Serial.print(cid.oid[0]);Serial.println(cid.oid[1]);
  Serial.print("Product: ");
  for (uint8_t i = 0; i < 5; i++) {Serial.print(cid.pnm[i]);}
  Serial.print("\nVersion: ");Serial.print(int(cid.prv_n));Serial.print(".");Serial.println(int(cid.prv_m));
  Serial.print("Serial number: ");Serial.println(cid.psn,HEX);
  Serial.print("Manufacturing date: ");Serial.print(int(cid.mdt_month));Serial.print('/');
  Serial.println((2000 + cid.mdt_year_low + 10 * cid.mdt_year_high));
}


void dumpStr(byte* a,int len)
{
  for(int i=0;i<len/16;i++){
    Serial.print(i*16);Serial.print("   ");
    if(i==0){Serial.print(" ");}
    if((i*16)<100){Serial.print(" ");}
    for(int j=0;j<16;j++){  
      if(a[i*16+j]<16){Serial.print("0");}
      Serial.print(a[i*16+j],HEX);
      Serial.print(" ");
    }
    Serial.print("  ");
    for(int j=0;j<16;j++){
      byte b=a[i*16+j];
      if(b<0x20 || b>0x7e){Serial.print(".");}
      else Serial.print((char)b);
    }
    Serial.println();
  }
  Serial.println();
}
