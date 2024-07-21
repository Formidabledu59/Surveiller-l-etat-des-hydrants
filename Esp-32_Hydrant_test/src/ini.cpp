#include "almanach.hpp"

extern SoftwareSerial swSer1;

void ini(BME280 capteurBMP280, ADXL345 adxl, int prmAdresse)
{
    // initialisation de la communication série physique
    Serial.begin(9600);

    // initialisation de la communication série logicielle
    swSer1.begin(9600, SWSERIAL_8N1);

    // initialisation de la communication
    Wire.begin(sdaBMP280, sclBMP280);        // Conf de la liaison I2C
    capteurBMP280.setI2CAddress(prmAdresse); // adresse I2C du BMP280

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
}

void miseMarche(char *prmVecEtat[], char *prmVecTemp[], char *prmVecAngle[])
{
    // code mise en marche
}

void miseVeille()
{
    // code mise en veille
}