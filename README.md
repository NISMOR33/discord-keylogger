# 🔐 Keylogger Pédagogique (Windows)

> **Projet académique - École 42**  
> Module de cybersécurité - Étude des hooks système et surveillance de frappes clavier

---

## 📋 Table des matières

- [Description du projet](#description-du-projet)
- [Fonctionnalités](#fonctionnalités)
- [Architecture technique](#architecture-technique)
- [Configuration](#configuration)
- [Installation et compilation](#installation-et-compilation)
- [Utilisation](#utilisation)
- [Structure des données](#structure-des-données)
- [Sécurité et limitations](#sécurité-et-limitations)
- [Disclaimer légal](#disclaimer-légal)

---

## 🎯 Description du projet

Ce projet est un **keylogger pédagogique** développé dans un cadre académique pour comprendre les mécanismes de surveillance système et les techniques de hooking sous Windows. Il permet de capturer les frappes clavier et d'exfiltrer les données vers des webhooks Discord pour analyse.

**Objectifs pédagogiques :**
- Comprendre le fonctionnement des hooks clavier Windows (`WH_KEYBOARD_LL`)
- Étudier les techniques d'injection de code et de surveillance
- Analyser les méthodes d'exfiltration de données
- Apprendre les contre-mesures de sécurité

---

## ⚡ Fonctionnalités

### Capture des frappes
- **Hook clavier global** : Installation d'un hook bas niveau (`WH_KEYBOARD_LL`)
- **Détection de fenêtre active** : Enregistrement du titre de la fenêtre courante
- **Gestion des touches spéciales** : Mapping des touches (Shift, Ctrl, Alt, etc.)
- **Formatage configurable** : Sortie lisible, codes décimaux ou hexadécimaux

### Exfiltration des données
- **Envoi périodique** : Transmission des logs toutes les 60 secondes (configurable)
- **Webhooks Discord** : Utilisation de l'API Discord pour la réception des données
- **Informations système** : Collecte et envoi des métadonnées système au démarrage
- **Gestion de session** : Messages de connexion/déconnexion

### Fonctionnalités avancées
- **Mode furtif** : Option pour masquer la console
- **Gestion du démarrage** : Attente si le système est en phase de boot
- **Nettoyage automatique** : Suppression des fichiers temporaires
- **Gestion des erreurs** : Handler pour interruption forcée (Ctrl+C)

---

## 🏗️ Architecture technique

### Stack technique
- **Langage** : C++ (Windows API)
- **Plateforme** : Windows x64
- **IDE** : Visual Studio (Configuration Release)
- **Librairies** :
  - `ws2_32` - Winsock pour les communications réseau
  - `iphlpapi` - API IP Helper
  - `psapi` - Process Status API
  - `shell32` - Shell API
  - `ole32` - OLE Library

### Flux de données

```
┌─────────────────┐
│  Hook Clavier   │
│  (WH_KEYBOARD_LL)│
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Traitement     │
│  - Mapping keys │
│  - Window title │
│  - Timestamp    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Buffer local   │
│  (Memory)       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Fichier log    │
│  (logs/)        │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Webhook Discord│
│  (cURL)         │
└─────────────────┘
```

### Structure du code

- **Hook Management** : Installation/libération du hook clavier
- **Key Processing** : Mapping et formatage des touches
- **System Info** : Collecte des informations système
- **Network Communication** : Envoi via webhooks Discord
- **File Management** : Gestion des logs et fichiers temporaires

---

## ⚙️ Configuration

### Paramètres modifiables

Modifiez ces constantes en haut du fichier source (`KEYLOGGER.cpp`) :

```cpp
// Visibilité de la console
#define visible        // Console visible
// #define invisible   // Décommentez pour masquer la console

// Comportement au démarrage
#define bootwait       // Attendre si le système démarre

// Format de sortie
#define FORMAT 0       // 0 = lisible; 10 = décimal; 16 = hexadécimal

// Options de capture
#define mouseignore    // Ignorer les clics souris

// Intervalle d'envoi (secondes)
#define SEND_INTERVAL_SECONDS 60
```

### Configuration des webhooks

**⚠️ IMPORTANT : Ne jamais publier vos webhooks en clair dans un dépôt public.**

Remplacez les URLs par vos propres webhooks Discord :

```cpp
#define WEBHOOK_INFO "https://discord.com/api/webhooks/VOTRE_WEBHOOK_INFO"
#define WEBHOOK_CONNEXION "https://discord.com/api/webhooks/VOTRE_WEBHOOK_CONNEXION"
#define WEBHOOK_LOGS "https://discord.com/api/webhooks/VOTRE_WEBHOOK_LOGS"
#define WEBHOOK_DESACTIVATION "https://discord.com/api/webhooks/VOTRE_WEBHOOK_DESACTIVATION"
```

**Création d'un webhook Discord :**
1. Allez dans les paramètres de votre serveur
2. Sélectionnez "Intégrations" → "Webhooks"
3. Créez un nouveau webhook
4. Copiez l'URL et collez-la dans le code

---

## 📦 Installation et compilation

### Pré-requis

- **Système** : Windows 10/11 (x64)
- **IDE** : Visual Studio 2019 ou supérieur
- **SDK** : Windows SDK
- **Outil externe** : cURL (doit être dans le PATH)

### Étapes de compilation

1. **Ouvrir le projet dans Visual Studio**
   ```bash
   # Ou créez un nouveau projet console C++
   # Ajoutez KEYLOGGER.cpp aux fichiers source
   ```

2. **Configurer le projet**
   - Configuration : Release
   - Plateforme : x64
   - Sous-système : Windows (`/SUBSYSTEM:windows`)

3. **Lier les librairies**
   Les librairies sont déjà liées via `#pragma comment` :
   ```cpp
   #pragma comment(lib, "ws2_32.lib")
   #pragma comment(lib, "iphlpapi.lib")
   #pragma comment(lib, "psapi.lib")
   #pragma comment(lib, "shell32.lib")
   #pragma comment(lib, "ole32.lib")
   ```

4. **Compiler**
   - Build → Build Solution (Ctrl+Shift+B)
   - L'exécutable sera généré dans `x64/Release/`

### Vérification de cURL

Assurez-vous que cURL est installé et accessible :
```bash
curl --version
```

Si cURL n'est pas installé, téléchargez-le depuis [curl.se](https://curl.se/windows/) et ajoutez-le au PATH.

---

## 🚀 Utilisation

### Lancement normal

```bash
# Exécution directe
KEYLOGGER.exe
```

### Lancement en mode furtif

Décommentez `#define invisible` dans le code et recompilez.

### Arrêt du programme

- **Normal** : Fermeture propre avec envoi des logs finaux
- **Forcé (Ctrl+C)** : Envoi d'un message de désactivation
- **Fermeture fenêtre** : Nettoyage automatique des fichiers temporaires

### Structure des fichiers générés

```
KEYLOGGER.exe
├── logs/
│   ├── log_TARGET_xxx_xxx.txt      # Logs de frappes
│   └── log_TARGET_xxx_FINAL_xxx.txt # Logs finaux
└── temp_*.json                      # Fichiers temporaires (auto-supprimés)
```

---

## 📊 Structure des données

### Format des logs

Les logs sont structurés avec les informations suivantes :

```
[Window: Nom de la fenêtre - at 2025-12-11T17:05:42] 
texte tapé ici...

[Window: Autre fenêtre - at 2025-12-11T17:06:15]
autre texte...
```

### Informations système collectées

Au démarrage, le programme envoie :
- **TARGET ID** : Identifiant unique basé sur l'IP
- **MACHINE** : Nom de l'ordinateur
- **USER** : Nom d'utilisateur Windows
- **IP ADDRESS** : Adresse IP locale
- **SYSTEM** : Version de Windows et architecture
- **PROCESSOR** : Type de processeur
- **MEMORY** : RAM totale et disponible
- **DISK SPACE** : Espace disque

### Messages Discord

Le programme utilise des **embeds Discord** avec :
- Titre et description
- Champs structurés pour les données
- Horodatage ISO 8601
- Footer avec information de protocole

---

## 🔒 Sécurité et limitations

### Limitations techniques

- **Windows uniquement** : Ne fonctionne pas sur Linux/macOS
- **Dépendance cURL** : Nécessite cURL installé
- **Antivirus** : Peut être détecté comme malware
- **Permissions admin** : Requiert des droits suffisants

### Mesures de sécurité

- **Fichiers temporaires** : Auto-suppression à la fermeture
- **Pas de persistance** : Ne s'installe pas au démarrage (sauf si configuré)
- **Transmission chiffrée** : Utilisation de HTTPS pour les webhooks

### Détection

Ce type de programme peut être détecté par :
- Antivirus (signature heuristique)
- EDR (Endpoint Detection and Response)
- Analyse comportementale (hook clavier)

---

## ⚖️ Disclaimer légal

> **⚠️ AVERTISSEMENT IMPORTANT**

Ce projet est destiné **exclusivement** à des fins **éducatives et académiques** dans le cadre de formations en cybersécurité.

**Conditions d'utilisation :**
- ✅ Utilisation uniquement sur des systèmes dont vous êtes propriétaire ou avez l'autorisation explicite
- ✅ Environnement de test contrôlé (machines virtuelles, laboratoires)
- ✅ Recherche et compréhension des mécanismes de sécurité
- ❌ **INTERDIT** : Utilisation sur des systèmes tiers sans consentement
- ❌ **INTERDIT** : Collecte de données sensibles sans autorisation
- ❌ **INTERDIT** : Usage malveillant ou illégal

**Responsabilité :**
L'auteur décline toute responsabilité en cas d'utilisation non conforme à ces conditions. L'utilisateur est seul responsable des conséquences de son utilisation de ce logiciel.

**Cadre légal :**
L'utilisation non autorisée de keyloggers est illégale dans la plupart des juridictions et peut être poursuivie en vertu des lois sur :
- La cybersécurité
- La protection des données personnelles
- L'accès non autorisé aux systèmes informatiques

---

## 📸 Captures d'exécution

### Nouvelle session démarrée
![Nouvelle session démarrée](https://github.com/Mateobro33/discord-keylogger/blob/b9cb58d63db688bc510de2eee0364a737c053bd5/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170542.png)

### Connexion établie
![Connexion Établie](https://github.com/Mateobro33/discord-keylogger/blob/9afc8252f84fd6ee886053e2d32fc0dae3bad97f/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170430.png)

### Nouveaux logs disponibles
![Nouveaux Logs Disponibles](https://github.com/Mateobro33/discord-keylogger/blob/0efb1fcc9b54fe5590105f5864dd9c5eaea831f0/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170629.png)

### Keylogger désactivé
![Keylogger désactivé](https://github.com/Mateobro33/discord-keylogger/blob/09289232dccd446953855bb1ac4b2d527f8b6a99/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170659.png)

---

## 📚 Ressources pédagogiques

**Pour approfondir vos connaissances :**
- [Windows API Documentation](https://docs.microsoft.com/en-us/windows/win32/api/)
- [Low-Level Keyboard Hooks](https://docs.microsoft.com/en-us/windows/win32/inputdev/low-level-keyboard-hooks)
- [Discord Webhooks Guide](https://discord.com/developers/docs/resources/webhook)
- [OWASP Keylogger Analysis](https://owasp.org/www-community/attacks/Keylogger)

**Contre-mesures :**
- Utilisation de claviers virtuels
- Analyse comportementale (EDR)
- Monitoring des hooks système
- Authentification forte (MFA)

---

## 👨‍💻 Auteur

**Projet réalisé dans le cadre du cursus de l'École 42**  
Module : Cybersécurité et Analyse de Malwares

**Date** : Décembre 2025  
**Version** : 4.1

---

## 📝 Licence

Ce projet est fourni à des fins éducatives uniquement.  
Toute reproduction ou utilisation commerciale est interdite sans autorisation explicite.

---

**⚡ Projet académique - Usage pédagogique uniquement ⚡**

