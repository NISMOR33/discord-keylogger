# Keylogger Pédagogique

Projet académique - École 42 - Module de cybersécurité

---

## Overview

Ce projet est un keylogger développé dans un cadre académique pour comprendre les mécanismes de surveillance système et les techniques de hooking sous Windows. Il capture les frappes clavier et exfiltre les données vers des webhooks Discord via des embeds formatés.

### Objectifs pédagogiques

- Comprendre le fonctionnement des hooks clavier Windows (`WH_KEYBOARD_LL`)
- Étudier les techniques d'injection de code et de surveillance
- Analyser les méthodes d'exfiltration de données via API web
- Apprendre les contre-mesures de sécurité

---

## Features

### Capture des frappes

- Hook clavier global bas niveau (`WH_KEYBOARD_LL`)
- Détection de fenêtre active avec titre et horodatage
- Mapping des touches spéciales (Shift, Ctrl, Alt, Win, etc.)
- Formatage configurable (lisible, décimal, hexadécimal)
- Gestion de la casse (Caps Lock, Shift)
- Rotation des fichiers logs par heure

### Exfiltration des données

- Envoi périodique via cURL (configurable, défaut: 15 secondes)
- Exécution de cURL en mode caché (fenêtre masquée)
- 4 webhooks Discord distincts (infos, connexion, logs, désactivation)
- Embeds Discord avec images, avatars et champs structurés
- Buffer mémoire pour accumulation des frappes
- Envoi de fichier log complet en pièce jointe

### Collecte d'informations système

Au démarrage, envoi des métadonnées:
- TARGET ID (généré depuis l'IP locale)
- MACHINE (nom de l'ordinateur)
- USER (nom d'utilisateur Windows)
- IP ADDRESS (adresse IP locale)
- SYSTEM (version Windows: 7/8/10+, architecture 32/64-bit)
- PROCESSOR (AMD64, ARM, IA64, x86)
- MEMORY (RAM totale et disponible en GB)
- DISK SPACE (espace disque C: total et libre)

### Gestion de session

- Message de connexion au démarrage (hook installé)
- Message de désactivation à la fermeture
- Handler pour interruption forcée (Ctrl+C, fermeture fenêtre)
- Envoi des logs finaux avant terminaison
- Nettoyage automatique des fichiers temporaires

### Fonctionnalités avancées

- Console masquée automatiquement au démarrage
- Sous-système Windows (pas de console visible)
- Attente si système en phase de boot (bootwait)
- Gestion des fichiers temporaires avec tracking
- Échappement JSON pour les caractères spéciaux
- Horodatage ISO 8601 pour tous les événements

---

## Architecture

### Stack technique

- **Langage**: C++ (Windows API)
- **Plateforme**: Windows x64
- **IDE**: Visual Studio (Release)
- **Sous-système**: Windows (`/SUBSYSTEM:windows /ENTRY:mainCRTStartup`)
- **Librairies**: ws2_32, iphlpapi, psapi, shell32, ole32
- **Dépendance externe**: cURL (exécution via CreateProcess)

### Flux de données

```
Hook Clavier → Mapping touches → Buffer mémoire → Fichier log (par heure)
                                                        ↓
                                              cURL caché → Webhook Discord
                                                        ↓
                                              Embed + fichier pièce jointe
```

### Structure du code

- `executeCurlHidden()`: Exécution cachée de cURL
- `HookCallback()`: Callback pour interception des touches
- `Save()`: Traitement et formatage des frappes
- `sendInfoMessage()`: Envoi infos système au démarrage
- `sendConnexionMessage()`: Notification de connexion
- `sendLogMessage()`: Envoi des logs (embed + fichier)
- `sendExitMessage()`: Notification de désactivation
- `consoleHandler()`: Gestion interruption forcée
- `cleanupTempFiles()`: Suppression fichiers temporaires

---

## Configuration

### Paramètres modifiables

Modifiez ces constantes en haut du fichier source (`KEYLOGGER.cpp`, lignes 69-73):

```cpp
#define visible        // Console visible (commenté par défaut)
// #define invisible   // Console masquée (activé dans main)
#define bootwait       // Attendre si le système démarre
#define FORMAT 0       // 0 = lisible; 10 = décimal; 16 = hexadécimal
#define mouseignore    // Ignorer les clics souris
#define SEND_INTERVAL_SECONDS 15  // Intervalle d'envoi en secondes
```

### Webhooks Discord

Remplacez les URLs par vos propres webhooks (lignes 63-66):

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

- Windows 7/8/10/11 (x64)
- Visual Studio 2019 ou supérieur
- Windows SDK
- cURL (doit être accessible dans le PATH système)

### Compilation

1. Créez un projet console C++ dans Visual Studio
2. Ajoutez `KEYLOGGER.cpp` aux fichiers source
3. Configurez: Release / x64
4. Les librairies sont déjà liées via `#pragma comment` (lignes 56-60)
5. Build → Build Solution (Ctrl+Shift+B)
6. L'exécutable sera généré dans `x64/Release/`

### Vérification de cURL

```bash
curl --version
```

Si cURL n'est pas installé, téléchargez-le depuis [curl.se](https://curl.se/windows/) et ajoutez-le au PATH.

---

## Usage

### Lancement

```bash
KEYLOGGER.exe
```

Le programme:
1. Masque immédiatement la console
2. Crée le dossier `logs/` si nécessaire
3. Envoie les informations système
4. Envoie la notification de connexion
5. Installe le hook clavier
6. Commence la surveillance

### Comportement

- **Surveillance active**: Capture toutes les frappes clavier
- **Buffer**: Accumule les frappes en mémoire
- **Envoi périodique**: Transmet les logs toutes les 15 secondes (configurable)
- **Rotation logs**: Nouveau fichier log chaque heure
- **Arrêt propre**: atexit handler pour fermeture normale

### Arrêt

- **Normal**: Fermeture avec envoi logs finaux + notification désactivation
- **Forcé (Ctrl+C)**: Handler détecte, envoie notification, nettoie
- **Fermeture fenêtre**: Même processus que Ctrl+C

### Structure des fichiers générés

```
KEYLOGGER.exe
├── logs/
│   ├── 2025-12-11__17-05-42.log           # Log horodaté
│   ├── log_TARGET_192_168_1_1_xxx.txt     # Logs transmis
│   └── log_TARGET_192_168_1_1_FINAL_xxx.txt # Logs finaux
└── temp_*.json                             # Fichiers temporaires (auto-supprimés)
    ├── temp_info.json
    ├── temp_connexion.json
    ├── temp_log_embed.json
    └── temp_exit.json
```

---

## Structure des données

### Format des logs

Les logs incluent le contexte de chaque frappe:

```
[Window: Nom de la fenêtre - at 2025-12-11T17:05:42] 
texte tapé ici avec les touches spéciales [SHIFT][ENTER]

[Window: Autre fenêtre - at 2025-12-11T17:06:15]
autre texte...
```

### Embeds Discord

Le programme utilise des embeds riches avec:
- Titre et description
- Image et avatar URL
- Champs structurés (inline ou block)
- Horodatage ISO 8601
- Footer avec information de protocole
- Couleur personnalisée (blanc pour actif, bleu pour désactivation)

### Messages Discord

1. **WEBHOOK_INFO**: Informations système complètes
2. **WEBHOOK_CONNEXION**: Notification hook installé
3. **WEBHOOK_LOGS**: Embed + fichier log en pièce jointe
4. **WEBHOOK_DESACTIVATION**: Notification terminaison + cleanup

---

## Sécurité et limitations

### Limitations techniques

- Windows uniquement (API Windows spécifiques)
- Dépendance cURL (doit être installé)
- Peut être détecté par les antivirus (hook clavier)
- Requiert des droits suffisants pour installation hook
- IP locale uniquement (pas d'IP publique)

### Détection

Ce programme peut être détecté par:
- Antivirus (signature heuristique pour hook clavier)
- EDR (Endpoint Detection and Response)
- Analyse comportementale (CreateProcess caché)
- Monitoring des hooks système (WH_KEYBOARD_LL)

### Mesures implémentées

- Fichiers temporaires auto-supprimés
- Pas de persistance (ne s'installe pas au démarrage)
- Transmission HTTPS via cURL
- Console masquée par défaut
- Nettoyage traces à la fermeture

---

## Disclaimer

Ce projet est destiné exclusivement à des fins éducatives et académiques dans le cadre de formations en cybersécurité à l'École 42.

**Conditions d'utilisation:**

- Utilisation uniquement sur des systèmes dont vous êtes propriétaire ou avez l'autorisation explicite
- Environnement de test contrôlé (machines virtuelles, laboratoires)
- Recherche et compréhension des mécanismes de sécurité
- Respect de la législation en vigueur

**Interdit:**

- Utilisation sur des systèmes tiers sans consentement
- Collecte de données sensibles sans autorisation
- Usage malveillant ou illégal
- Distribution à des fins non éducatives

L'auteur décline toute responsabilité en cas d'utilisation non conforme à ces conditions. L'utilisation non autorisée de keyloggers est illégale dans la plupart des juridictions.

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
- [SetWindowsHookEx Documentation](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa)
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
