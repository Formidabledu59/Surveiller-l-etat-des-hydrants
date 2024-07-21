#include <Arduino.h>
#include <SPI.h>
#include "SparkFun_ADXL345.h" //inclusion de la lib Adrl345 
//(SparkFun ADXL345 Arduino Library (SparkFun Electronics))

// Numéro des broches utilisées pour le port série logiciel
#define CS_PIN 15   // broche CS - ADXL345
#define MISO_PIN 12 // broche SD0 - ADXL345
#define MOSI_PIN 13 // broche SDA - ADXL345
#define SCK_PIN 14  // broche SCL - ADXL345

String msgConsole = ""; // message pour la console

// création d'objet
ADXL345 adxl = ADXL345(CS_PIN);      // pour ADXL34 connecté en SPI

// Variables
int x, y, z;              // valeurs d'accélération sur les 3 axes

void setup()
{
  // initialisation de la communication série physique
  Serial.begin(9600);

  // Initialisation de la communication SPI
  SPI.end();
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  SPI.setDataMode(SPI_MODE3);
  SPI.setFrequency(5000000); // bus SPI à 5MHz
  adxl.setSpiBit(0);         // 4 wire SPI mode
  adxl.powerOn();            // Power on the ADXL345

  // paramétrage de l'accéléromètre
  adxl.setRangeSetting(16); // gamme de mesure jusqu'à 16G
  adxl.setFullResBit(true); // résolution sur 13 bits (Scale Factor = 4mg/LSB)
  adxl.setRate(3200);       // data rate = 3200Hz
}

void loop()
{
  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||Adrl 345|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  Serial.print("---------------Adrl 345--------------\n");
  // lecture des valeurs d'accélération
  adxl.readAccel(&x, &y, &z);
  // affichage dans la console
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.println(z);
  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  delay(500);
}

