# Keylogger Pédagogique

Projet académique - École 42 - Module de cybersécurité

---

## Overview

Ce projet est un keylogger développé dans un cadre académique pour comprendre les mécanismes de surveillance système et les techniques de hooking sous Windows. Il permet de capturer les frappes clavier et d'exfiltrer les données vers des webhooks Discord.

### Objectifs pédagogiques

- Comprendre le fonctionnement des hooks clavier Windows (`WH_KEYBOARD_LL`)
- Étudier les techniques d'injection de code et de surveillance
- Analyser les méthodes d'exfiltration de données
- Apprendre les contre-mesures de sécurité

---

## Features

### Capture des frappes

- Hook clavier global (`WH_KEYBOARD_LL`)
- Détection de fenêtre active
- Gestion des touches spéciales (Shift, Ctrl, Alt, etc.)
- Formatage configurable (lisible, décimal, hexadécimal)

### Exfiltration des données

- Envoi périodique (configurable, défaut: 60 secondes)
- Webhooks Discord pour la réception des données
- Collecte d'informations système au démarrage
- Gestion de session (connexion/déconnexion)

### Fonctionnalités avancées

- Mode furtif (console masquée)
- Gestion du démarrage (attente si boot système)
- Nettoyage automatique des fichiers temporaires
- Gestion des erreurs (Ctrl+C handler)

---

## Architecture

### Stack technique

- **Langage**: C++ (Windows API)
- **Plateforme**: Windows x64
- **IDE**: Visual Studio (Release)
- **Librairies**: ws2_32, iphlpapi, psapi, shell32, ole32

### Flux de données

```
Hook Clavier → Traitement → Buffer local → Fichier log → Webhook Discord
```

---

## Configuration

### Paramètres modifiables

Modifiez ces constantes en haut du fichier source (`KEYLOGGER.cpp`):

```cpp
#define visible        // Console visible
// #define invisible   // Console masquée
#define bootwait       // Attendre si le système démarre
#define FORMAT 0       // 0 = lisible; 10 = décimal; 16 = hexadécimal
#define mouseignore    // Ignorer les clics souris
#define SEND_INTERVAL_SECONDS 60  // Intervalle d'envoi en secondes
```

### Webhooks Discord

Remplacez les URLs par vos propres webhooks:

```cpp
#define WEBHOOK_INFO "https://discord.com/api/webhooks/VOTRE_WEBHOOK_INFO"
#define WEBHOOK_CONNEXION "https://discord.com/api/webhooks/VOTRE_WEBHOOK_CONNEXION"
#define WEBHOOK_LOGS "https://discord.com/api/webhooks/VOTRE_WEBHOOK_LOGS"
#define WEBHOOK_DESACTIVATION "https://discord.com/api/webhooks/VOTRE_WEBHOOK_DESACTIVATION"
```

**Important**: Ne publiez jamais vos webhooks en clair dans un dépôt public.

---

## Installation

### Pré-requis

- Windows 10/11 (x64)
- Visual Studio 2019 ou supérieur
- Windows SDK
- cURL (doit être dans le PATH)

### Compilation

1. Ouvrez le projet dans Visual Studio
2. Configurez: Release / x64
3. Les librairies sont déjà liées via `#pragma comment`
4. Build → Build Solution (Ctrl+Shift+B)

### Vérification de cURL

```bash
curl --version
```

Si cURL n'est pas installé, téléchargez-le depuis [curl.se](https://curl.se/windows/)

---

## Usage

### Lancement

```bash
KEYLOGGER.exe
```

### Mode furtif

Décommentez `#define invisible` dans le code et recompilez.

### Arrêt

- Normal: Fermeture propre avec envoi des logs finaux
- Forcé (Ctrl+C): Envoi d'un message de désactivation
- Fermeture fenêtre: Nettoyage automatique

### Structure des fichiers

```
KEYLOGGER.exe
├── logs/
│   ├── log_TARGET_xxx_xxx.txt
│   └── log_TARGET_xxx_FINAL_xxx.txt
└── temp_*.json (auto-supprimés)
```

---

## Structure des données

### Format des logs

```
[Window: Nom de la fenêtre - at 2025-12-11T17:05:42] 
texte tapé ici...
```

### Informations système collectées

- TARGET ID (basé sur l'IP)
- MACHINE (nom de l'ordinateur)
- USER (nom d'utilisateur)
- IP ADDRESS
- SYSTEM (version Windows, architecture)
- PROCESSOR
- MEMORY
- DISK SPACE

---

## Sécurité et limitations

### Limitations

- Windows uniquement
- Dépendance cURL
- Peut être détecté par les antivirus
- Requiert des droits suffisants

### Détection

Ce programme peut être détecté par:
- Antivirus (signature heuristique)
- EDR (Endpoint Detection and Response)
- Analyse comportementale

---

## Disclaimer

Ce projet est destiné exclusivement à des fins éducatives et académiques dans le cadre de formations en cybersécurité.

**Conditions d'utilisation:**

- Utilisation uniquement sur des systèmes dont vous êtes propriétaire ou avez l'autorisation explicite
- Environnement de test contrôlé (machines virtuelles, laboratoires)
- Recherche et compréhension des mécanismes de sécurité

**Interdit:**

- Utilisation sur des systèmes tiers sans consentement
- Collecte de données sensibles sans autorisation
- Usage malveillant ou illégal

L'auteur décline toute responsabilité en cas d'utilisation non conforme à ces conditions.

---

## Screenshots

### Nouvelle session démarrée
![Nouvelle session](https://github.com/Mateobro33/discord-keylogger/blob/b9cb58d63db688bc510de2eee0364a737c053bd5/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170542.png)

### Connexion établie
![Connexion](https://github.com/Mateobro33/discord-keylogger/blob/9afc8252f84fd6ee886053e2d32fc0dae3bad97f/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170430.png)

### Nouveaux logs
![Logs](https://github.com/Mateobro33/discord-keylogger/blob/0efb1fcc9b54fe5590105f5864dd9c5eaea831f0/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170629.png)

### Désactivation
![Désactivation](https://github.com/Mateobro33/discord-keylogger/blob/09289232dccd446953855bb1ac4b2d527f8b6a99/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170659.png)

---

## Resources

- [Windows API Documentation](https://docs.microsoft.com/en-us/windows/win32/api/)
- [Low-Level Keyboard Hooks](https://docs.microsoft.com/en-us/windows/win32/inputdev/low-level-keyboard-hooks)
- [Discord Webhooks Guide](https://discord.com/developers/docs/resources/webhook)
- [OWASP Keylogger Analysis](https://owasp.org/www-community/attacks/Keylogger)

---

## Author

Projet réalisé dans le cadre du cursus de l'École 42  
Module: Cybersécurité et Analyse de Malwares

Version: 4.1

---

## License

Ce projet est fourni à des fins éducatives uniquement.
