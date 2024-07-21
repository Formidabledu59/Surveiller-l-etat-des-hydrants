#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>             //pour accéder au bus I2C
#include "SparkFunBME280.h"   //inclusion de la lib BMP280 (SparkFun BME280 (SparkFun Electronics))
#include <SparkFun_ADXL345.h> //inclusion de la lib Adrl345 (SparkFun ADXL345 Arduino Library (SparkFun Electronics))

// Numéro des broches utilisées pour le port série logiciel
#define CS_PIN 15   // broche CS - ADXL345
#define MISO_PIN 12 // broche SD0 - ADXL345
#define MOSI_PIN 13 // broche SDA - ADXL345
#define SCK_PIN 14  // broche SCL - ADXL345

#define ILS_PIN 32 // broche 32 - ILS

String msgConsole = ""; // message pour la console

// caractère (ou byte) reçu
char incomingChar;

// création d'objet
ADXL345 adxl = ADXL345(CS_PIN); // pour ADXL34 connecté en SPI

// Variables
int x, y, z;                             // valeurs d'accélération sur les 3 axes
int initialX, initialY, initialZ;        // Variables de référence pour les valeurs initiales d'accélération
int previousX, previousY, previousZ = 0; //  Définir des variables pour stocker les valeurs précédentes d'accélération
bool actifVeille = false;

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

  // Initialisation lecture etat ILS
  pinMode(ILS_PIN, INPUT_PULLUP); // GPIO34 en entrée numérique avec résistance de pull-up
}

void loop()
{
  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||ILS||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  Serial.print("---------------ILS--------------\n");
  if (digitalRead(ILS_PIN) == LOW)
  {
    // ILS au fermé
    if (actifVeille = false)
    {
      actifVeille = true;
      Serial.print("ILS est maintenant Activé");
      // valeur de depart - ADXL 345
      adxl.readAccel(&initialX, &initialY, &initialZ);
    }
    else
    {
      actifVeille = false;
      Serial.print("ILS est maintenant Désactivé");
    }
  }
  else
  {
    // ILS ouvert
    if (actifVeille = false)
    {
      Serial.print("ILS est Désactivé");
    }
  }
  Serial.print('\n');
  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  delay(500);

  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||Adrl 345|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  Serial.print("---------------Adrl 345--------------\n");
  if (actifVeille = true)
  {
    // lecture des valeurs d'accélération
    adxl.readAccel(&x, &y, &z);
    // affichage dans la console
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.println(z);

    // Calcul de l'angle d'inclinaison par rapport aux valeurs initiales
    float angleX = atan2(x - initialX, z - initialZ) * 180 / PI;
    float angleY = atan2(y - initialY, z - initialZ) * 180 / PI;

    // Affichage de l'angle d'inclinaison dans la console
    Serial.print("Angle X: ");
    Serial.println(angleX);
    Serial.print("Angle Y: ");
    Serial.println(angleY);

    // Vérification si l'angle dépasse 8 degrés
    if (abs(angleX) > 8 || abs(angleY) > 8)
    {
      // Affichage de l'alerte dans le terminal
      Serial.println("Alerte : Inclinaison supérieure à 8 degrés !");
    }
  }

  delay(500);
}
