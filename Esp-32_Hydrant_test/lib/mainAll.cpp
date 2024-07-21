// Adresse mail Gmail
// -------------------------------------
// Login : sigfox.armentieres@gmail.com
// passwd : BtsSnir2k24;

// Administration du backend sigfox
// -------------------------------------
// https://backend.sigfox.com
// Login : sigfox.armentieres@gmail.com
// passwd : SnirCiel2k24,

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>             //pour accéder au bus I2C
#include "SparkFunBME280.h"   //inclusion de la lib BMP280 (SparkFun BME280 (SparkFun Electronics))
#include <SparkFun_ADXL345.h> //inclusion de la lib Adrl345 (SparkFun ADXL345 Arduino Library (SparkFun Electronics))
#include <stdio.h>            // hexa
#include <cmath>

// Numéro des broches utilisées pour le port série logiciel
#define txPin 17 // sérigraphie D2 - SigFox
#define rxPin 16 // sérigraphie D1 - SigFox

#define sclBMP280 22 // broche SCL - BMP280
#define sdaBMP280 21 // broche SDA - BMP280

#define CS_PIN 15   // broche CS - ADXL345
#define MISO_PIN 12 // broche SD0 - ADXL345
#define MOSI_PIN 13 // broche SDA - ADXL345
#define SCK_PIN 14  // broche SCL - ADXL345

#define ILS_PIN_T 4  // broche 4 - ILS technicien
#define ILS_PIN_OF 2 // broche 2 - ILS Ouverture Fermeture

String msgConsole = "";  // message pour la console
String Trame = "AT$SF="; // Valeur par défault de la tram envoyé via sigfox
char frame[9];           // S'assurer qu'il y a suffisamment d'espace pour contenir tous les caractères (1 + 4 + 3 + 1 pour '\0')
char incomingChar;       // caractère (ou byte) reçu

enum EtatHydrant
{
  OUVERT,
  FERME,
  RENVERSE,
  VEILLE
};

// création d'objet
SoftwareSerial swSer1(rxPin, txPin); // SoftwareSerial
BME280 capteurBMP280;                // BME280
ADXL345 adxl = ADXL345(CS_PIN);      // pour ADXL34 connecté en SPI
#define PRECISION 0.004

// Variables
int addrI2CBMP280 = 0x76;               // adresses I2C BMP280
float valTemperature = 0.0;             // pour stocker la température
int x, y, z = 0.0;                      // valeurs d'accélération sur les 3 axesfloat;
int initialX, initialY, initialZ = 0.0; // Variables de référence pour les valeurs initiales d'accélération
float angleX, angleY, angleZ, valAngle = 0.0;

// double angleProportion = (250) / 90; // valeur pour un angle de 1°

// Etat
int etatHydrant = VEILLE; // ouvert 0, fermé 1, renverse 2, veille 3.

// // AVOIR...............
// void miseVeille(int prmEtat)
// {
//   Serial.print("ILS est maintenant Désactivé"); // IHM
//   prmEtat = VEILLE;                             // mise en VEILLE
//   delay(2000);                                  // delai de 2s
// }
// void miseMarche(int prmEtat)
// {
//   Serial.print("ILS est maintenant Activé");       // IHM
//   prmEtat = FERME;                                 // FERME pas de probleme
//   adxl.readAccel(&initialX, &initialY, &initialZ); // valeur de depart - ADXL 345
//   delay(2000);                                     // delai de 2s
// }

// Fonction pour construire la trame
void buildFrame(int prmEtat, float prmAngle, float prmTemperature, char *frame)
{
  int prmAngle10 = prmAngle * 10;               // angle * 10 pour avoir de la precision et supprimer la virgule
  int prmTemperature100 = prmTemperature * 100; // temperature * 100 pour avoir de la precision et supprimer la virgule

  // Vérifier que les valeurs sont dans les plages attendues
  if (prmEtat > 15)
    prmEtat = 15; // 15 est le maximum pour un demi-octet
  if (prmAngle10 > 4095)
    prmAngle10 = 4095; // 4095 est le maximum pour 3 caractères hexadécimaux
  if (prmTemperature100 > 65535)
    prmTemperature100 = 65535; // 65535 est le maximum pour 4 caractères hexadécimaux

  // Construire la trame
  sprintf(frame, "%1X%03X%04X", prmEtat, prmAngle10, prmTemperature100);
    //ou
  // >> 8 et & 0x00FF
}

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

  // // Initialisation lecture etat ILS
  // pinMode(ILS_PIN_T, INPUT_PULLUP);  // GPIO34 en entrée numérique avec résistance de pull-up
  // pinMode(ILS_PIN_OF, INPUT_PULLUP); // GPIO34 en entrée numérique avec résistance de pull-up

  // actif ou veille (veille au demarage par defaut)
  if (etatHydrant != VEILLE)
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
  if (digitalRead(ILS_PIN_T) == HIGH)
  {
    // Si en veille
    if (etatHydrant == VEILLE)
    {
      Serial.print("ILS est maintenant Activé");       // IHM
      etatHydrant = FERME;                             // FERME pas de probleme
      adxl.readAccel(&initialX, &initialY, &initialZ); // valeur de depart - ADXL 345
      delay(2000);                                     // delai de 2s
    }
    else
    {
      Serial.print("ILS est maintenant Désactivé"); // IHM
      etatHydrant = VEILLE;                         // mise en VEILLE
      delay(2000);                                  // delai de 2s
    }
  }
  Serial.print('\n'); // IHM

  // Si Actif mesure
  if (etatHydrant != VEILLE)
  {

    // ILS au fermé
    if (digitalRead(ILS_PIN_OF) == HIGH)
    {
      etatHydrant = OUVERT; // hydrant OUVERT
    }
    else
    {
      etatHydrant = FERME; // hydrant FERME
    }

    adxl.readAccel(&x, &y, &z); // lecture des valeurs d'accélération

    angleX = 90 * (abs(PRECISION * (x - initialX))); // calcule angle X
    angleY = 90 * (abs(PRECISION * (y - initialY))); // calcule angle Y
    angleZ = 90 * (abs(PRECISION * (z - initialZ))); // calcule angle Z
                                                     // valeur numerique pour obtenir 8° est 22.22222222222222222222222....22
    valAngle = angleX;
    if ((valAngle) < (angleY))
    {
      valAngle = angleY;
    }
    if ((valAngle) < (angleZ))
    {
      valAngle = angleZ;
    }

    // valangle = max(abs(angleX), abs(angleY), abs(angleZ)); // angle le plus grands (abs = value absolute)

    Serial.println("X: " + (String)angleX); // IHM
    Serial.println("Y: " + (String)angleY); // IHM
    Serial.println("Z: " + (String)angleZ); // IHM

    Serial.println("------------------------------"); // IHM

    Serial.println(valAngle); // IHM

    Serial.println("------------------------------"); // IHM
    Serial.println("------------------------------"); // IHM
    // Vérification si l'angle dépasse 8 degrés
    if (valAngle > 8)
    {
      etatHydrant = RENVERSE; // hydrant RENVERSE
    }
  }

  // si Alerte
  if ((etatHydrant == OUVERT) || (etatHydrant == RENVERSE))
  {
    // température
    valTemperature = capteurBMP280.readTempC();
    msgConsole = "Température : " + (String)valTemperature + " °C"; // IHM
    Serial.println(msgConsole);                                     // IHM

    // renversement
    msgConsole = "Alerte : Inclinaison supérieure à " + (String)valAngle + " degrés !"; // IHM
    Serial.println(msgConsole);                                                         // IHM

    // Construire la trame
    buildFrame(etatHydrant, valAngle, valTemperature, frame);
    Trame = "AT$SF=";
    for (int i = 0; i < 8; i++)
    {
      Trame = Trame + frame[i];
    }
    Serial.println("Trame: " + Trame);

    swSer1.println(Trame + '\r');
    delay(5000);                                  // delai 5s
    Serial.print("ILS est maintenant Désactivé"); // IHM
    etatHydrant = VEILLE;                         // mise en VEILLE
  }
  delay(500); // delai 0.5s
}