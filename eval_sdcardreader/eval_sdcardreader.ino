#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

File ftest;

int sdInit(char* fname);
int sdWrite(char* fname,char* chaine,int longueur);
int sdReWrite(char* fname,char* chaine,int longueur);
int sdRead(char* fname,char* chaine,int longueur);

void setup() 
{
}

void loop() 
{
}

int sdReWrite(char* fname,char* chaine,int longueur)
{
  if(ftest=SD.open(fname,FILE_WRITE)){
    ftest.seek(0);
    for(int i=0;i<longueur;i++){ftest.write(chaine[i]);}
    ftest.close();
    return 1;
  }
  return 0;
}

int sdWrite(char* fname,char* chaine,int longueur)
{
  if(ftest=SD.open(fname,FILE_WRITE)){
    for(int i=0;i<longueur;i++){ftest.write(chaine[i]);}
    ftest.close();
    return 1;
  }
  return 0;
}

int sdRead(char* fname,char* chaine,int longueur)
{
  if(ftest=SD.open(fname,FILE_READ)){
    for(int i=0;i<longueur;i++){chaine[i]=ftest.read();}
    ftest.close();
    return 1;
  }
  return 0;
}


int sdInit(char* fname)
{
  Serial.print("\nSD card ");
  while(!SD.begin(4)){Serial.println("KO");}

  Serial.println("OK");
  SD.remove(fname);
  ftest=SD.open(fname,FILE_WRITE);
  ftest.close();
}
