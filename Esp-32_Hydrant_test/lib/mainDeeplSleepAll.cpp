#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>             //pour accéder au bus I2C
#include "SparkFunBME280.h"   //inclusion de la lib BMP280 (SparkFun BME280 (SparkFun Electronics))
#include <SparkFun_ADXL345.h> //inclusion de la lib Adrl345 (SparkFun ADXL345 Arduino Library (SparkFun Electronics))
#include <stdio.h>            // hexa
#include <cmath>
#include "driver/rtc_io.h" //pour activer les résistances de pullup interne

#define CS_PIN 15   // broche CS - ADXL345
#define MISO_PIN 12 // broche SD0 - ADXL345
#define MOSI_PIN 13 // broche SDA - ADXL345
#define SCK_PIN 14  // broche SCL - ADXL345

#define ILS_PIN_T 4  // broche 4 - ILS technicien
#define ILS_PIN_OF 2 // broche 2 - ILS Ouverture Fermeture

ADXL345 adxl = ADXL345(CS_PIN); // pour ADXL34 connecté en SPI

int x, y, z = 0.0;                      // valeurs d'accélération sur les 3 axesfloat;
int initialX, initialY, initialZ = 0.0; // Variables de référence pour les valeurs initiales d'accélération

#define BUTTON_PIN_BITMASK 0x100000004 // GPIO32 et GPIO04 => 2^32 + 2^2 = 4294967300 = 0x100000004

RTC_DATA_ATTR int bootCount = 0;

/********************* ISR *********************/
/* Look for Interrupts and Triggered Action    */
void ADXL_ISR()
{

  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();

  // Free Fall Detection
  if (adxl.triggered(interrupts, ADXL345_FREE_FALL))
  {
    Serial.println("*** FREE FALL ***");
    // add code here to do when free fall is sensed
  }

  // Inactivity
  if (adxl.triggered(interrupts, ADXL345_INACTIVITY))
  {
    Serial.println("*** INACTIVITY ***");
    // add code here to do when inactivity is sensed
  }

  // Activity
  if (adxl.triggered(interrupts, ADXL345_ACTIVITY))
  {
    Serial.println("*** ACTIVITY ***");
    // add code here to do when activity is sensed
  }

  // Double Tap Detection
  if (adxl.triggered(interrupts, ADXL345_DOUBLE_TAP))
  {
    Serial.println("*** DOUBLE TAP ***");
    // add code here to do when a 2X tap is sensed
  }

  // Tap Detection
  if (adxl.triggered(interrupts, ADXL345_SINGLE_TAP))
  {
    Serial.println("*** TAP ***");
    // add code here to do when a tap is sensed
  }
}

void iniADXL()
{

  int interruptPin = 1; // Setup pin 1 to be the interrupt pin (for most Arduino Boards)

  // Initialisation de la communication SPI
  SPI.end();
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  SPI.setDataMode(SPI_MODE3);
  SPI.setFrequency(5000000); // bus SPI à 5MHz
  adxl.setSpiBit(0);         // 4 wire SPI mode
  adxl.powerOn();            // Power on the ADXL345

  // paramétrage de l'accéléromètre
  adxl.setRangeSetting(2);  // gamme de mesure jusqu'à 16G
  adxl.setFullResBit(true); // résolution sur 13 bits (Scale Factor = 4mg/LSB)
  adxl.setRate(10000);      // data rate = 10kHz

  adxl.setActivityXYZ(1, 1, 0); // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(3); // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)

  // Setting all interupts to take place on INT1 pin
  adxl.setImportantInterruptMapping(0, 0, 0, 1, 0); // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);"
  // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
  // This library may have a problem using INT2 pin. Default to INT1 pin.

  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.ActivityINT(1);

  attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING); // Attach Interrupt

  adxl.setLowPower(1);
}

int What_GPIO_wake_up()
{
  uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  int PinWakeUp = ((log(GPIO_reason)) / log(2));
  Serial.println(PinWakeUp);
  return PinWakeUp;
}

void activation()
{
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL); // disable all wakeup source

  // activer la résistance de pullup interne de GPIO2
  rtc_gpio_pullup_en(GPIO_NUM_4);
  rtc_gpio_pulldown_dis(GPIO_NUM_4);
  // Configure le GPIO2 comme source de réveil quand la tension vaut 0V
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, HIGH);

  iniADXL();
  // Configure le GPIO2 et GPIO32 comme source de réveil quand la tension vaut 0V
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH); // ESP_EXT1_WAKEUP_ANY_HIGH ou ESP_EXT1_WAKEUP_ALL_LOW

  Serial.println("Mise en Marche");
}

void desactivation()
{
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  adxl.ActivityINT(0);

  // activer la résistance de pullup interne de GPIO2
  rtc_gpio_pullup_en(GPIO_NUM_4);
  rtc_gpio_pulldown_dis(GPIO_NUM_4);
  // Configure le GPIO2 comme source de réveil quand la tension vaut 0V
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, HIGH);

  Serial.println("Mise en Veille");
}

void detecteAnomalie()
{
  int alerte = What_GPIO_wake_up();
  switch (alerte)
  {
  case 2:
    Serial.println("je suis ouvert aaaaaaaaaaa");
    break;
  case 32:
    Serial.println("je suis penché aaaaaaaaaaa");
    break;
  default:
    Serial.println("bizzare... ");
    break;
  }
  // activation();
  desactivation();
}

void way_by_wakeup_reason()
{
  esp_sleep_wakeup_cause_t source_reveil;

  source_reveil = esp_sleep_get_wakeup_cause();

  switch (source_reveil)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Réveil causé par un signal externe avec RTC_IO (ext0)");
    activation(); // fonction si manipulation de l'ILS Technitien
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Réveil causé par un signal externe avec RTC_CNTL (ext1)");
    detecteAnomalie();
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Réveil causé par un timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Réveil causé par un touchpad");
    Serial.println("Bizzare j'ai pas touché à ce truc mhhhhhh");
    break;
  default:
    Serial.printf("Réveil pas causé par le Deep Sleep: %d\n", source_reveil);
    Serial.println("\r");
    desactivation();
    break;
  }
}

void setup()
{
  Serial.begin(9600);

  delay(500); // 0.5

  Serial.println("----------------------");

  way_by_wakeup_reason();

  ++bootCount;
  Serial.println("C'était mon " + String(bootCount) + "eme Boot ");
  // Rentre en mode Deep Sleep
  Serial.println("Rentre en mode Deep Sleep");
  Serial.println("----------------------");
  esp_deep_sleep_start();
}

void loop() {}