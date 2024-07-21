#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>             //pour accéder au bus I2C
#include "SparkFunBME280.h"   //inclusion de la lib BMP280 (SparkFun BME280 (SparkFun Electronics))
#include <SparkFun_ADXL345.h> //inclusion de la lib Adrl345 (SparkFun ADXL345 Arduino Library (SparkFun Electronics))
#include <stdio.h>            // hexa

// Numéro des broches utilisées pour le port série logiciel
#define txPin 17 // sérigraphie D2 - SigFox
#define rxPin 16 // sérigraphie D1 - SigFox

#define sclBMP280 22 // broche SCL - BMP280
#define sdaBMP280 21 // broche SDA - BMP280

#define CS_PIN 15   // broche CS - ADXL345
#define MISO_PIN 12 // broche SD0 - ADXL345
#define MOSI_PIN 13 // broche SDA - ADXL345
#define SCK_PIN 14  // broche SCL - ADXL345

#define ILS_PIN_T 32  // broche 32 - ILS technicien
#define ILS_PIN_OF 19 // broche 19 - ILS Ouverture Fermeture

String msgConsole = ""; // message pour la console

char incomingChar; // caractère (ou byte) reçu

// création d'objet
SoftwareSerial swSer1(rxPin, txPin); // SoftwareSerial
BME280 capteurBMP280;                // BME280
ADXL345 adxl = ADXL345(CS_PIN);      // pour ADXL34 connecté en SPI
#define PRECISION 0.004

// Variables
int addrI2CBMP280 = 0x76;         // adresses I2C BMP280
float valtemp = 0.0;              // pour stocker la température
int x, y, z;                      // valeurs d'accélération sur les 3 axesfloat;
int initialX, initialY, initialZ; // Variables de référence pour les valeurs initiales d'accélération
float angleX, angleY, angleZ, valangle;

char etat[2];  // 4 bits (une valeur hexadécimale peut être représentée en 2 caractères)
char temp[5];  // 2 octets (une valeur hexadécimale peut être représentée en 4 caractères)
char angle[4]; // 1 octet (une valeur hexadécimale peut être représentée en 2 caractères)

// double angleProportion = (250) / 90; // valeur pour un angle de 1°

// Etat
bool actifVeille = false; // EstActif
bool alerte = false;      // Alerte sur l'Hydrant
int valetat = 3;          // ouvert 0, fermé 1, renverse 2, veille 3.

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
  pinMode(ILS_PIN_T, INPUT_PULLUP);  // GPIO34 en entrée numérique avec résistance de pull-up
  pinMode(ILS_PIN_OF, INPUT_PULLUP); // GPIO34 en entrée numérique avec résistance de pull-up

  // actif ou veille (veille au demarage par defaut)
  if (actifVeille == true)
  {
    Serial.print("ILS est Activé");
  }
  else
  {
    Serial.print("ILS est Désactivé");
  }
}

void loop()
{
  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||SigFox|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  // code SigFox
  //**** réception moniteur série ou Putty ****//
  if (Serial.available())
  {
    // lecture de l'octet reçu
    incomingChar = (char)Serial.read();
    // recopie de l'octet vers le kit BRKWS01
    // les commandes AT sont suivies d'un '\r' (caractère de la touche Entrée sous Linux)
    // mais surtout pas du '\r\n' de Windows
    if (incomingChar != '\n')
      swSer1.print(incomingChar);
  }

  //**** réception softwareserial RX et TX ****//
  if (swSer1.available())
  {
    // lecture du caractère reçu
    incomingChar = (char)swSer1.read();
    // recopie de l'octet vers le moniteur série
    Serial.print(incomingChar);
    //  après un '\r' on ajoute le caractère '\n' pour être conforme au retour à la ligne de Windows
    if (incomingChar == '\r')
      Serial.print('\n');
  }

  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  // ILS au fermé
  if (digitalRead(ILS_PIN_T) == LOW)
  {
    // Si en veille
    if (actifVeille == false)
    {
      actifVeille = true;                              // Actif
      adxl.readAccel(&initialX, &initialY, &initialZ); // valeur de depart - ADXL 345
      delay(2000);                                     // delai de 2s
      Serial.print("ILS est maintenant Activé");       // IHM
      valetat = 1;
      alerte = false; // parfait
      msgConsole = "Fermé";
    }
    else
    {
      actifVeille = false;                          // Veille
      delay(2000);                                  // delai de 2s
      Serial.print("ILS est maintenant Désactivé"); // IHM
      valetat = 3;
    }
  }
  Serial.print('\n'); // IHM

  // Si Actif mesure
  if (actifVeille == true)
  {
    alerte = false;

    for (int i = 0; i < 2; i++)
    {
      etat[i] = '0';
    }
    for (int i = 0; i < 5; i++)
    {
      temp[i] = '0';
    }
    for (int i = 0; i < 4; i++)
    {
      angle[i] = '0';
    }

    // ILS au fermé
    if (digitalRead(ILS_PIN_OF) == LOW)
    {
      alerte = true; // Ouvert
      valetat = 0;
      msgConsole = "Ouvert"; // IHM
    }
    else
    {
      valetat = 1;
      msgConsole = "Fermé"; // IHM
    }

    adxl.readAccel(&x, &y, &z); // lecture des valeurs d'accélération

    angleX = PRECISION * (x - initialX); // calcule angle X
    angleY = PRECISION * (y - initialY); // calcule angle Y
    angleZ = PRECISION * (z - initialZ); // calcule angle Y


    // valangle = angleX;
    // if ((valangle) < (angleY))
    // {
    //   valangle = angleY;
    // }
    // if ((valangle) < (angleZ))
    // {
    //   valangle = angleZ;
    // }
    valangle = max(abs(angleX), abs(angleY), abs(angleZ)); //angle le plus grands (abs = value absolute)

    // Serial.println(angleX); // IHM
    // Serial.println(angleY); // IHM
    // Serial.println(angleZ); // IHM

    // Vérification si l'angle dépasse 8 degrés
    if (valangle > 8)
    {
      alerte = true;
      valetat = 2;
    }
  }

  // si Alerte
  if (alerte == true)
  {
    // ouverture fermeture
    Serial.println(msgConsole); // IHM

    // température
    valtemp = capteurBMP280.readTempC();
    msgConsole = "Température : " + (String)valtemp + " °C"; // IHM
    Serial.println(msgConsole);                              // IHM

    // renversement
    msgConsole = "Alerte : Inclinaison supérieure à " + (String)valangle + " degrés !"; // IHM
    Serial.println(msgConsole);                                                         // IHM

    // Convertir entier en chaîne hexadécimale pour 'etat'
    sprintf(etat, "%1X", valetat);
    printf("Etat: %s\n", etat);

    // Convertir valeur à virgule flottante en entier (multiplier par 100) puis en chaîne hexadécimale pour 'temp'
    int temint = (int)(valtemp * 100); // Convertir en entier pour supprimer la partie décimale
    sprintf(temp, "%04X", temint);
    printf("Temp: %s\n", temp);

    // Convertir valeur à virgule flottante en entier (multiplier par 10) puis en chaîne hexadécimale pour 'angle'
    int angleint = (int)(valangle * 10); // Convertir en entier pour supprimer la partie décimale
    sprintf(angle, "%03X", angleint);
    printf("Angle: %s\n", angle);

    String Trame = "AT$SF="; // Valeur par défault de la tram envoyé via sigfox
    char data[9];            // S'assurer qu'il y a suffisamment d'espace pour contenir tous les caractères (1 + 4 + 3 + 1 pour '\0')

    // Combiner 'etat', 'temp' et 'angle' en 'data'
    snprintf(data, sizeof(data), "%s%s%s", etat, temp, angle);

    printf("data: %s\n", data);

    for (int i = 0; i < 8; i++)
    {
      Trame = Trame + data[i];
    }
    Serial.println("Trame: " + Trame);

    swSer1.println(Trame + '\r');
  }
  delay(500); // delai 0.5s
}
