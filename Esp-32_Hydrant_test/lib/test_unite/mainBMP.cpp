#include <Arduino.h>
#include "SparkFunBME280.h" //inclusion de la lib BMP280 (SparkFun BME280 (SparkFun Electronics))

// Numéro des broches utilisées pour le port série logiciel
#define sclBMP280 22 // broche SCL - BMP280
#define sdaBMP280 21 // broche SDA - BMP280

String msgConsole = ""; // message pour la console

// création d'objet
BME280 capteurBMP280; // BME280

// Variables
int addrI2CBMP280 = 0x76; // adresses I2C BMP280
float pression = 0.0;     // pour stocker la pression
float temperature = 0.0;  // pour stocker la température

void setup()
{
  // initialisation de la communication série physique
  Serial.begin(9600);

  // initialisation de la communication
  Wire.begin(sdaBMP280, sclBMP280);           // Conf de la liaison I2C
  capteurBMP280.setI2CAddress(addrI2CBMP280); // adresse I2C du BMP280
  // Vérifie connexion
  if (capteurBMP280.beginI2C(Wire) == false)
  {
    Serial.println("BMP280 : communication impossible");
  }
}

void loop()
{
  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||Bmp 280||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  Serial.print("---------------Bmp 280--------------\n");
  // Pression atmosphérique
  pression = capteurBMP280.readFloatPressure() / 100; // en hPa
  msgConsole = "Pression : " + (String)pression + " hPa - ";

  // température
  temperature = capteurBMP280.readTempC();
  msgConsole += "Température : " + (String)temperature + " °C";

  Serial.println(msgConsole);

  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  delay(500);
}