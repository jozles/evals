/*
 * This program is a simple binary write/read benchmark.
 */
#include "SdFat.h"
#include "sdios.h"
#include <sstream>
#include "FreeStack.h"

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 1
/*
  Change the value of SD_CS_PIN if you are using SPI and
  your hardware does not use the default value, SS.
  Common values are:
  Arduino Ethernet shield: pin 4
  Sparkfun SD shield: pin 8
  Adafruit SD shields and modules: pin 10
*/

const uint8_t SD_CS_PIN = 4;

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(8)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

// Set PRE_ALLOCATE true to pre-allocate file clusters.
const bool PRE_ALLOCATE = true;

// Set SKIP_FIRST_LATENCY true if the first read/write to the SD can
// be avoid by writing a file header or reading the first record.
const bool SKIP_FIRST_LATENCY = true;

// Size of read/write.
const size_t BUF_SIZE = 512;

// File size in MB where MB = 1,000,000 bytes.
const uint32_t FILE_SIZE_MB = 5;

// Write pass count.
const uint8_t WRITE_COUNT = 2;

// Read pass count.
const uint8_t READ_COUNT = 2;
//==============================================================================
// End of configuration constants.
//------------------------------------------------------------------------------
// File size in bytes.
const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;

// Insure 4-byte alignment.
uint32_t buf32[(BUF_SIZE + 3)/4];
uint8_t* buf = (uint8_t*)buf32;

#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else  // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE

// Serial output stream
ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------
// Store error strings in flash to save RAM.
#define error(s) sd.errorHalt(&Serial, F(s))
//------------------------------------------------------------------------------

void diags();
void openWriteRead();
void benchtest();
void writeCheck();

void cidDmp() {
  cid_t cid;
  if (!sd.card()->readCID(&cid)) {

    error("readCID failed");
  }
  cout << F("Manufacturer ID: ");
  cout << hex << int(cid.mid) << dec << endl;
  cout << F("OEM ID: ") << cid.oid[0] << cid.oid[1] << endl;
  cout << F("Product: ");
  for (uint8_t i = 0; i < 5; i++) {
    cout << cid.pnm[i];
  }
  cout << F("\nVersion: ");
  cout << int(cid.prv_n) << '.' << int(cid.prv_m) << endl;
  cout << F("Serial number: ") << hex << cid.psn << dec << endl;
  cout << F("Manufacturing date: ");
  cout << int(cid.mdt_month) << '/';
  cout << (2000 + cid.mdt_year_low + 10 * cid.mdt_year_high) << endl;
}

void clearSerialInput() {
  uint32_t m = micros();
  do {
    if (Serial.read() >= 0) {
      m = micros();
    }
  } while (micros() - m < 10000);
}

void reformatMsg() {
  cout << F("Try reformatting the card.  For best results use\n");
  cout << F("the SdFormatter program in SdFat/examples or download\n");
  cout << F("and use SDFormatter from www.sdcard.org/downloads.\n");
}

void dumpStr(byte* a,int len)
{
  for(int i=0;i<len/16;i++){
    cout << int(i*16) << F("   ");
    if(i==0){cout << (" ");}
    if((i*16)<100){cout << (" ");}
    for(int j=0;j<16;j++){  
      if(a[i*16+j]<16){cout << ("0");}
      //cout << std::hex << uint8_t(a[i*16+j]);
      Serial.print((a[i*16+j]),HEX);
      cout << (" ");
    }
    cout << ("   ");
    for(int j=0;j<16;j++){
      byte b=a[i*16+j];
      if(b<0x20 || b>0x7e){cout << (".");}
      else cout << b;
    }
    cout << endl;
  }
  cout << endl;
}

void setup() {
  Serial.begin(115200);

  // Wait for USB Serial
  while (!Serial) {
    SysCall::yield();
  }
  delay(1000);
  cout << F("\nUse a freshly formatted SD for best performance.\n");
  if (!ENABLE_DEDICATED_SPI) {
    cout << F(
      "\nSet ENABLE_DEDICATED_SPI nonzero in\n"
      "SdFatConfig.h for best SPI performance.\n");
  }

  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
  }
  cout << F("\nCard successfully initialized.\n");
  cout << endl;

#if HAS_UNUSED_STACK
  cout << F("FreeStack: ") << FreeStack() << endl;
#endif  // HAS_UNUSED_STACK

  if (sd.fatType() == FAT_TYPE_EXFAT) {
    cout << F("Type is exFAT") << endl;
  } else {
    cout << F("Type is FAT") << int(sd.fatType()) << endl;
  }

  uint32_t size = sd.card()->sectorCount();
  if (size == 0) {
    cout << F("Can't determine the card size.\n");
    cout << F("Try another SD card or reduce the SPI bus speed.\n");
    cout << F("Edit SPI_SPEED in this program to change it.\n");
  while(1){delay(1);}
  }

  uint32_t sizeMB = 0.000512 * size + 0.5;
  cout << F("Card size: ") << sizeMB;
  cout << F(" MB (MB = 1,000,000 bytes)\n");
  cout << endl;
  cout << F("Volume is FAT") << int(sd.vol()->fatType());
  cout << F(", Cluster size (bytes): ") << sd.vol()->bytesPerCluster();
  cout << endl;

  cidDmp();
  
  // use uppercase in hex and use 0X base prefix
  cout << uppercase << showbase << endl;
}

void loop() {

  clearSerialInput();

  // F() stores strings in flash to save RAM
  cout << F("'b' for bench test ");
  cout << F("'h' for open,read,close 500 bytes file) ");
  cout << F("'w' for write check ");
  cout << F("'d' for dir : ");
  while (!Serial.available()) {
    SysCall::yield();
  }
  char test;
  test=Serial.read();
  cout << test << endl;

  switch (test){
      case 'b': benchTest();break;
      case 'h': openWriteClose();break;
      case 'w': writeCheck();break;
      case 'd': diags();break;
      default:  break;
  }
}

void benchTest()
{
  float s;
  uint32_t t;
  uint32_t maxLatency;
  uint32_t minLatency;
  uint32_t totalLatency;
  bool skipLatency;
  
  //SD.remove("bench.dat");
  // open or create file - truncate existing file.
  if (!file.open("bench.dat", O_RDWR | O_CREAT | O_TRUNC)) {
    error("open failed");
  }

  // fill buf with known data
  if (BUF_SIZE > 1) {
    for (size_t i = 0; i < (BUF_SIZE - 2); i++) {
      buf[i] = 'A' + (i % 26);
    }
    buf[BUF_SIZE-2] = '\r';
  }
  buf[BUF_SIZE-1] = '\n';

  cout << F("FILE_SIZE_MB = ") << FILE_SIZE_MB << endl;
  cout << F("BUF_SIZE = ") << BUF_SIZE << F(" bytes\n");
  cout << F("Starting write test, please wait.") << endl << endl;

  // do write test
  uint32_t n = FILE_SIZE/BUF_SIZE;
  cout <<F("write speed and latency") << endl;
  cout << F("speed,max,min,avg") << endl;
  cout << F("KB/Sec,usec,usec,usec") << endl;
  for (uint8_t nTest = 0; nTest < WRITE_COUNT; nTest++) {
    file.truncate(0);
    if (PRE_ALLOCATE) {
      if (!file.preAllocate(FILE_SIZE)) {
        error("preAllocate failed");
      }
    }
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    skipLatency = SKIP_FIRST_LATENCY;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
      uint32_t m = micros();
      if (file.write(buf, BUF_SIZE) != BUF_SIZE) {
        error("write failed");
      }
      m = micros() - m;
      totalLatency += m;
      if (skipLatency) {
        // Wait until first write to SD, not just a copy to the cache.
        skipLatency = file.curPosition() < 512;
      } else {
        if (maxLatency < m) {
          maxLatency = m;
        }
        if (minLatency > m) {
          minLatency = m;
        }
      }
    }
    file.sync();
    t = millis() - t;
    s = file.fileSize();
    cout << s/t <<',' << maxLatency << ',' << minLatency;
    cout << ',' << totalLatency/n << endl;
  }
  cout << endl << F("Starting read test, please wait.") << endl;
  cout << endl <<F("read speed and latency") << endl;
  cout << F("speed,max,min,avg") << endl;
  cout << F("KB/Sec,usec,usec,usec") << endl;

  // do read test
  for (uint8_t nTest = 0; nTest < READ_COUNT; nTest++) {
    file.rewind();
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    skipLatency = SKIP_FIRST_LATENCY;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
      buf[BUF_SIZE-1] = 0;
      uint32_t m = micros();
      int32_t nr = file.read(buf, BUF_SIZE);
      if (nr != BUF_SIZE) {
        error("read failed");
      }
      m = micros() - m;
      totalLatency += m;
      if (buf[BUF_SIZE-1] != '\n') {

        error("data check error");
      }
      if (skipLatency) {
        skipLatency = false;
      } else {
        if (maxLatency < m) {
          maxLatency = m;
        }
        if (minLatency > m) {
          minLatency = m;
        }
      }
    }
    s = file.fileSize();
    t = millis() - t;
    cout << s/t <<',' << maxLatency << ',' << minLatency;
    cout << ',' << totalLatency/n << endl;
  }
  cout << endl << F("Done") << endl;
  file.close();
}

void openWriteClose()
{
  #define LBUF 500
  //char buf[LBUF];
  memset(buf,'x',LBUF);
  dumpStr(buf,LBUF);
  #define FNAME "toto"  
  #define FSIZE LBUF

  cout << F("\nstart test ") << int(LBUF) << F(" bytes file (open/write/close)") << endl;

// create file 
  if (!file.open("bench.dat", O_RDWR | O_CREAT | O_TRUNC)) {
    error("open failed");
  }

// contiguous clusters
  file.truncate(0);
  if (PRE_ALLOCATE) {
    if (!file.preAllocate(FSIZE)) {
      error("preAllocate failed");
    }
  } 
  file.close();

// test can start
  unsigned long t3,t2,t1,t0=micros();

  if (!file.open("bench.dat", O_RDWR)) {
    error("open failed");
  }

  t1=micros();

  if (file.write(buf, LBUF) != LBUF) {
    error("write failed");
  }

// wait end of write
  file.sync();
  t2=micros();

// close
  file.close();
  t3=micros();

// result 
  cout << F("(uS) open=") << int(t1-t0) << F(" write=") << int(t2-t1) << F(" close=") << int(t3-t2) << endl;
  
}

void writeCheck()
{
  cout << F("write check") << endl;

  if (!file.open("bench.dat", O_RDWR)) {
    error("open failed");
  }
  file.seek(16);
  file.write("something",9);
  file.sync();
  file.close();

  if (!file.open("bench.dat", O_RDWR)) {
    error("open failed");
  }

  file.read(buf, LBUF);
  file.close();

  dumpStr(buf,LBUF);
}

void diags()
{

  cout << F("Files found (date time size name):\n");
  sd.ls(LS_R | LS_DATE | LS_SIZE);

  uint32_t size = sd.card()->sectorCount();
  uint32_t sizeMB = 0.000512 * size + 0.5;
  if ((sizeMB > 1100 && sd.vol()->sectorsPerCluster() < 64)
      || (sizeMB < 2200 && sd.vol()->fatType() == 32)) {
    cout << F("\nThis card should be reformatted for best performance.\n");
    cout << F("Use a cluster size of 32 KB for cards larger than 1 GB.\n");
    cout << F("Only cards larger than 2 GB should be formatted FAT32.\n");
    reformatMsg();
    return;
  }
  // Read any extra Serial data.
  clearSerialInput();

  cout << F("\nSuccess!  Type any character to restart.\n");
  while (!Serial.available()) {
    SysCall::yield();
  }

}
