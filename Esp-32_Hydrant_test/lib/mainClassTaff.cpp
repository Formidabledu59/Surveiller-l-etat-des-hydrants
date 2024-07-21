#include "almanach.hpp"

// Variables
int addrI2CBMP280 = 0x76;         // adresses I2C BMP280
float valtemp;                    // pour stocker la température
int initialX, initialY, initialZ; // Variables de référence pour les valeurs initiales d'accélération
float valangle;

// Etat
bool actifVeille = false;     // EstActif
bool alerte = false;          // Alerte sur l'Hydrant
EtatHydrant valetat = VEILLE; // ouvert 0, fermé 1, renverse 2, veille 3.

String msgConsole = ""; // message pour la console

char incomingChar; // caractère (ou byte) reçu

// Trame SigFox
char etat[2];  // 4 bits (une valeur hexadécimale peut être représentée en 2 caractères)
char temp[5];  // 2 octets (une valeur hexadécimale peut être représentée en 4 caractères)
char angle[4]; // 1 octet (une valeur hexadécimale peut être représentée en 2 caractères)

// float angleProportion = 90 / 250; // valeur pour un angle de 1°

// création d'objet
SoftwareSerial swSer1(rxPin, txPin); // SoftwareSerial
BME280 capteurBMP280;                // BME280
ADXL345 adxl = ADXL345(CS_PIN);      // pour ADXL34 connecté en SPI

void setup()
{
  ini(capteurBMP280, adxl, addrI2CBMP280);

  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||IHM||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  Serial.println((String) "Recopie le moniteur série de PlatformIO sur Rx=" + rxPin + " et Tx=" + txPin); // IHM
  // Vérifie connexion
  if (capteurBMP280.beginI2C(Wire) == false)
  {
    Serial.println("BMP280 : communication impossible"); // IHM
  }
  if (actifVeille == true)
  {
    Serial.print("ILS est Activé"); // IHM
  }
  else
  {
    Serial.print("ILS est Désactivé"); // IHM
  }
  // actif ou veille (veille au demarage par defaut)
  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
}

void loop()
{
  //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||SigFox|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
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
      valetat = FERME;                                 // Fermé
      alerte = false;                                  // parfait
      adxl.readAccel(&initialX, &initialY, &initialZ); // valeur de depart - ADXL 345
      delay(2000);                                     // delai de 2s
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

      // void miseMarche();
    }
    else
    {
      actifVeille = false; // Veille
      valetat = VEILLE;    // Veille
      delay(2000);         // delai de 2s

      // void miseVeille();
    }
  }
  Serial.print('\n'); // IHM

  // Si Actif mesure
  if (actifVeille == true)
  {
    valetat = etatVanneHydrant();
    // Vérification si ouvert
    if (valetat == OUVERT)
    {
      alerte = true;
    }
    valangle = renversementHydrant(adxl, initialX, initialY);
    // Vérification si l'angle dépasse 8 degrés
    if (valangle > 8)
    {
      alerte = true;
      valetat = RENVERSE;
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
    // void contructionTrame();
    Serial.println("Trame: " + Trame); // IHM

    swSer1.println(Trame + '\r');
    // void envoieTrame();
  }
  delay(500); // delai 0.5s
}
