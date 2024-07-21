#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>             //pour accéder au bus I2C
#include "SparkFunBME280.h"   //inclusion de la lib BMP280 (SparkFun BME280 (SparkFun Electronics))
#include <SparkFun_ADXL345.h> //inclusion de la lib Adrl345 (SparkFun ADXL345 Arduino Library (SparkFun Electronics))

// Numéro des broches utilisées pour le port série logiciel
#define txPin 17 // sérigraphie D2 - SigFox
#define rxPin 16 // sérigraphie D1 - SigFox

#define sclBMP280 22 // broche SCL - BMP280
#define sdaBMP280 21 // broche SDA - BMP280

#define CS_PIN 15   // broche CS - ADXL345
#define MISO_PIN 12 // broche SD0 - ADXL345
#define MOSI_PIN 13 // broche SDA - ADXL345
#define SCK_PIN 14  // broche SCL - ADXL345

#define ILS_PIN 32 // broche 32 - ILS

String msgConsole = ""; // message pour la console

// caractère (ou byte) reçu
char incomingChar;

// création d'objet
SoftwareSerial swSer1(rxPin, txPin); // SoftwareSerial
BME280 capteurBMP280;                // BME280
ADXL345 adxl = ADXL345(CS_PIN);      // pour ADXL34 connecté en SPI

// Variables
int addrI2CBMP280 = 0x76; // adresses I2C BMP280
float pression = 0.0;     // pour stocker la pression
float temperature = 0.0;  // pour stocker la température
int x, y, z;              // valeurs d'accélération sur les 3 axes

// Varibles test
//  Définir des variables pour stocker les valeurs précédentes d'accélération
int previousX = 0;
int previousY = 0;
int previousZ = 0;

const int threshold = 100; // À ajuster selon votre cas d'utilisation

void setup()
{
  // initialisation de la communication série physique
  Serial.begin(9600);
  // initialisation de la communication série logicielle
  swSer1.begin(9600, SWSERIAL_8N1);
  Serial.println((String) "Recopie le moniteur série de PlatformIO sur Rx=" + rxPin + " et Tx=" + txPin);

  // initialisation de la communication
  Wire.begin(sdaBMP280, sclBMP280);           // Conf de la liaison I2C
  capteurBMP280.setI2CAddress(addrI2CBMP280); // adresse I2C du BMP280
  // Vérifie connexion
  if (capteurBMP280.beginI2C(Wire) == false)
  {
    Serial.println("BMP280 : communication impossible");
  }

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

  // Initialisation lecture etat ILS
  pinMode(ILS_PIN, INPUT_PULLUP); // GPIO34 en entrée numérique avec résistance de pull-up
}

void loop()
{
  // //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||SigFox|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  // Serial.print("---------------SigFox---------------\n");

  // //**** réception moniteur série ou Putty ****//
  // if (Serial.available())
  // {
  //   // lecture de l'octet reçu
  //   incomingChar = (char)Serial.read();
  //   // recopie de l'octet vers le kit BRKWS01
  //   // les commandes AT sont suivies d'un '\r' (caractère de la touche Entrée sous Linux)
  //   // mais surtout pas du '\r\n' de Windows
  //   if (incomingChar != '\n')
  //     swSer1.print(incomingChar);
  // }

  // //**** réception softwareserial RX et TX ****//
  // if (swSer1.available())
  // {
  //   // lecture du caractère reçu
  //   incomingChar = (char)swSer1.read();
  //   // recopie de l'octet vers le moniteur série
  //   Serial.print(incomingChar);
  //   //  après un '\r' on ajoute le caractère '\n' pour être conforme au retour à la ligne de Windows
  //   if (incomingChar == '\r')
  //     Serial.print('\n');
  // }
  // //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||Bmp 280||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  // Serial.print("---------------Bmp 280--------------\n");
  // // Pression atmosphérique
  // pression = capteurBMP280.readFloatPressure() / 100; // en hPa
  // msgConsole = "Pression : " + (String)pression + " hPa - ";

  // // température
  // temperature = capteurBMP280.readTempC();
  // msgConsole += "Température : " + (String)temperature + " °C";

  // Serial.println(msgConsole);

  // //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||Adrl 345|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  // Serial.print("---------------Adrl 345--------------\n");
  // // lecture des valeurs d'accélération
  // adxl.readAccel(&x, &y, &z);
  // // affichage dans la console
  // Serial.print(x);
  // Serial.print(", ");
  // Serial.print(y);
  // Serial.print(", ");
  // Serial.println(z);

  // // Comparaison avec les valeurs précédentes
  // int diffX = abs(x - previousX);
  // int diffY = abs(y - previousY);
  // int diffZ = abs(z - previousZ);

  // // Vérification si l'une des différences dépasse le seuil
  // if (diffX > threshold || diffY > threshold || diffZ > threshold)
  // {
  //   // Affichage de l'alerte dans le terminal
  //   Serial.println("Alerte : Grande différence dans les valeurs d'accélération !");
  // }

  // // Mise à jour des valeurs précédentes
  // previousX = x;
  // previousY = y;
  // previousZ = z;

  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||ILS||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  Serial.print("---------------ILS--------------\n");
  if (digitalRead(ILS_PIN) == LOW)
  {
    // ILS au fermé
    Serial.print("ILS au fermé");
  }
  else
  {
    // ILS ouvert
    Serial.print("ILS ouvert");
  }
  Serial.print('\n');
  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  delay(500);
}