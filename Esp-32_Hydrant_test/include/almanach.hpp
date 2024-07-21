#pragma once

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// Libraries - Reference
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>             //pour accéder au bus I2C
#include "SparkFunBME280.h"   //inclusion de la lib BMP280 (SparkFun BME280 (SparkFun Electronics))
#include <SparkFun_ADXL345.h> //inclusion de la lib Adrl345 (SparkFun ADXL345 Arduino Library (SparkFun Electronics))
#include <stdio.h>            // hexa

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// Numéro des broches utilisées pour le port série logiciel

#define txPin 17 // sérigraphie D2 - SigFox
#define rxPin 16 // sérigraphie D1 - SigFox

#define sclBMP280 22 // broche SCL - BMP280
#define sdaBMP280 21 // broche SDA - BMP280

#define CS_PIN 15   // broche CS - ADXL345
#define MISO_PIN 12 // broche SD0 - ADXL345
#define MOSI_PIN 13 // broche SDA - ADXL345
#define SCK_PIN 14  // broche SCL - ADXL345

#define ILS_PIN_T 32 // broche 32 - ILS technicien

#define ILS_PIN_OF 19 // broche 19 - ILS Ouverture Fermeture

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// état de l'hydrant
enum EtatHydrant
{
    OUVERT,
    FERME,
    RENVERSE,
    VEILLE
};

/**
 * @brief Initialisation de la communication (série physique/série logicielle/SPI/I2C/paramétrage de l'accéléromètre/pull-up)
 */
void ini(BME280 capteurBMP280, ADXL345 adxl, int prmAdresse);

void miseMarche(); // passage technicien (veille à marche)
void miseVeille(); // passage technicien (veille à marche) ou suite envoie trame (anomalie pas trame mensuel)

EtatHydrant etatVanneHydrant(); // donne etat de la vanne

float renversementHydrant(ADXL345 adxl, float prmInitialX, float prmInitialY); // donne angle de renversement.

void contructionTrame(); // construit la trame
void envoieTrame();      // envoie Trame
