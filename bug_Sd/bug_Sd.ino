#include <SPI.h>
#include <SD.h>

#define FNAME "toto"
#define FSIZE 128

File myFile;

#define SOMEWHERE 16
#define SOMETHING "something"
#define ANOTHERTHING "anotherthing"

void setup() {

  Serial.begin(115200);delay(100);
  Serial.println("start\n");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  /* create file and fill 256 bytes */
  
  SD.remove(FNAME);

  myFile=SD.open(FNAME,FILE_WRITE);
  if(!myFile){Serial.print(" OPEN WRITE KO");}
  byte c=0x01;
  for(int i=0;i<FSIZE;i++){myFile.write(c);}
  myFile.close();

  /* verify it */

  byte a[FSIZE*2];memset(a,0x32,FSIZE);  /* fill field with 0x32 to verify that 0x01 is written */             
  Serial.println("data in the field");dumpStr(a,FSIZE);
  myFile=SD.open(FNAME,FILE_READ);
  if(!myFile){Serial.print(" OPEN READ KO");}
    for(int i=0;i<FSIZE;i++){a[i]=myFile.read();}
  myFile.close();
  Serial.println("data read");
  dumpStr(a,FSIZE);


  /* try to write somewhere */

  myFile=SD.open(FNAME,FILE_WRITE);
  if(!myFile){Serial.print(" OPEN WRITE KO");}
  myFile.seek(0);
  Serial.print(" position=");Serial.print(myFile.position());
  myFile.seek(SOMEWHERE);
  Serial.print(" ; position after seek=");Serial.print(myFile.position());
  Serial.print(" ; write ");Serial.print(strlen(SOMETHING));Serial.print(" char");
  myFile.write(SOMETHING);
  Serial.print(" ; position after write=");Serial.print(myFile.position());
  myFile.close();
  Serial.println("\n");

  /* verify it */

  memset(a,0xff,FSIZE);
  myFile=SD.open(FNAME,FILE_READ);
  if(!myFile){Serial.print(" OPEN READ KO");}
    for(int i=0;i<FSIZE*2;i++){a[i]=myFile.read();}
  myFile.close();
  Serial.println("modified data read");dumpStr(a,FSIZE*2);

  /* try to print somewhere */

  myFile=SD.open(FNAME,FILE_WRITE);
  if(!myFile){Serial.print(" OPEN WRITE KO");}
  myFile.seek(0);
  Serial.print(" position=");Serial.print(myFile.position());
  myFile.seek(SOMEWHERE);
  Serial.print(" ; position after seek=");Serial.print(myFile.position());
  Serial.print(" ; print ");Serial.print(strlen(SOMETHING));Serial.print(" char");
  myFile.write(ANOTHERTHING);
  Serial.print(" ; position after print=");Serial.print(myFile.position());
  myFile.close();
  Serial.println("\n");

  /* verify it */

  memset(a,0xff,FSIZE);
  myFile=SD.open(FNAME,FILE_READ);
  if(!myFile){Serial.print(" OPEN READ KO");}
    for(int i=0;i<FSIZE*2;i++){a[i]=myFile.read();}
  myFile.close();
  Serial.println("modified data read");dumpStr(a,FSIZE*2);


  while(1){}
}

void loop() {

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
