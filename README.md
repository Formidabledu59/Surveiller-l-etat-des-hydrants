## README.md

# Surveillance de l'État des Hydrants Connectés

Ce projet vise à surveiller l'état des hydrants en les rendant connectés et intelligents. Les hydrants détectent les anomalies, transmettent les informations pour traitement, et enregistrent les données dans une base de données adaptée.

## Table des Matières

1. [Introduction](#introduction)
2. [Fonctionnalités](#fonctionnalités)
3. [Installation](#installation)
4. [Utilisation](#utilisation)
5. [Choix des Composants](#choix-des-composants)
6. [Architecture et Développement](#architecture-et-développement)
7. [Contraintes Techniques](#contraintes-techniques)
8. [Auteurs](#auteurs)

## Introduction

Dans ce projet, l'objectif est de transformer les hydrants en dispositifs capables de surveiller leur propre état et les conditions environnantes. Cela inclut la détection de l'ouverture ou de la fermeture de la vanne, l'inclinaison, et les changements de température. Les informations sont ensuite envoyées, traitées, et stockées de manière sécurisée, avec la possibilité de déclencher des alertes pour informer rapidement les autorités municipales en cas d'anomalie .

## Fonctionnalités

1. **Détection de l'ouverture et de la fermeture de la vanne** :
   - Surveillance en temps réel pour garantir une réponse rapide en cas d'utilisation non autorisée ou de fuite.

2. **Détection de l'inclinaison ou du renversement** :
   - Prévention des actes de vandalisme et détection des incidents susceptibles de compromettre l'intégrité de l'hydrant.

3. **Transmission des données** :
   - Envoi fiable des données collectées à un système cloud, incluant l'état de la vanne, l'inclinaison, et les conditions environnementales telles que la température.

4. **Intégration avec la base de données** :
   - Stockage structuré des informations transmises par les hydrants connectés pour un accès rapide et une analyse facilitée.

5. **Développement du service REST** :
   - Gestion des informations et des alertes via un service REST en PHP, capable d'envoyer des alertes par SMS ou email aux responsables concernés  .

## Installation

### Prérequis

- **ESP32-DevKitC V2**
- **ADXL345** (Capteur de renversement)
- **Interrupteur Lame Souple (ILS)** (Capteur d'ouverture et de fermeture)
- **BMP280** (Capteur de température)
- **Modem Sigfox BRKWS01**

### Étapes d'Installation

1. Cloner le dépôt :
   ```bash
   git clone https://github.com/votre-utilisateur/votre-projet.git
   cd votre-projet
   ```

2. Configurer l'ESP32 et les capteurs selon les schémas fournis dans la section d'architecture et de développement.

3. Installer les dépendances nécessaires pour le service REST en PHP.

## Utilisation

1. **Déploiement des Hydrants** :
   - Installer les capteurs sur chaque hydrant selon les instructions fournies.
   - Configurer le réseau Sigfox pour la transmission des données.

2. **Surveillance** :
   - Utiliser l'interface web pour surveiller l'état des hydrants en temps réel.
   - Recevoir des alertes par SMS ou email en cas de détection d'anomalies.

## Choix des Composants

1. **ESP32-DevKitC V2** :
   - Programmable en C++, faible consommation d'énergie, mode DeepSleep.

2. **ADXL345** :
   - Capteur de renversement, mesure précise de l'inclinaison.

3. **Interrupteur Lame Souple (ILS)** :
   - Détection de l'ouverture/fermeture, très basse consommation.

4. **BMP280** :
   - Capteur de température, faible consommation d'énergie.

5. **Modem Sigfox BRKWS01** :
   - Transmission fiable des données sur de longues distances, faible consommation d'énergie    .

## Architecture et Développement

### Diagramme de Déploiement

![Diagramme de Déploiement](docs_liens/DiagrammeDeploiement.png)

### DeepSleep

Le mode DeepSleep de l'ESP32 permet de réduire la consommation d'énergie en désactivant les parties non essentielles du microcontrôleur. L'ESP32 peut se réveiller en réponse à plusieurs types d'événements, tels que des signaux externes sur des pins spécifiques et des timers .

### Envoi des Données

Les données sont transmises via le réseau Sigfox et traitées par un service REST en PHP, qui gère également les alertes en envoyant des SMS ou des emails aux responsables concernés   .

## Contraintes Techniques

- **Base de données MySQL** pour le stockage des informations.
- **Langage C/C++** pour le développement du logiciel embarqué.
- **Étanchéité** des composants pour garantir leur fonctionnement dans toutes les conditions météorologiques.
- **Réseau Sigfox** pour la transmission des données.
- **Service REST en PHP** pour la gestion des alertes   .

## Auteurs

- **Ayoub Fathallah** - Développeur IOT et BackEnd.

---
