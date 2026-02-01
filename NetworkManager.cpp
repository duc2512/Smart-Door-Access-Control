#include "NetworkManager.h"
#include "DisplayManager.h"
#include "DoorController.h"

NetworkManager network;

const char* ssid = "PTIT.HCM_CanBo";     
const char* password = ""; 

void NetworkManager::begin() {
    display.showConnectingWiFi();
    
    // We can use the original logic
    WiFi.begin(ssid, password);
    
    int dots = 0;
    while (WiFi.status() != WL_CONNECTED && dots < 20) {
        delay(500);
        display.printLine(1, "Ket noi WiFi" + String("....................").substring(0, dots)); // Simple animation
        dots++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        display.showWiFiResult(true);
        Blynk.config(BLYNK_AUTH_TOKEN);
        Blynk.connect(); // Try to connect to Blynk
    } else {
        display.showWiFiResult(false);
    }
}

void NetworkManager::run() {
    Blynk.run();
}

void NetworkManager::logEvent(String event, String message) {
    if (WiFi.status() == WL_CONNECTED) {
        Blynk.logEvent(event, message);
    }
}

void NetworkManager::updateStatus(String status) {
    if (WiFi.status() == WL_CONNECTED) {
        Blynk.virtualWrite(V1, status);
    }
}

void NetworkManager::updateLastCard(String uid) {
    if (WiFi.status() == WL_CONNECTED) {
        Blynk.virtualWrite(V0, uid);
    }
}

// ==== Blynk Callbacks ====
BLYNK_WRITE(V5) {
    int value = param.asInt();
    if (value == 1) {
        Serial.println("🔓 Mở cửa từ xa qua Blynk");
        network.updateStatus("🔓 Mở cửa từ xa");
        network.logEvent("bao_dong", "Cửa đã mở từ xa");
        
        display.showAccessGranted("MO TU XA");
        door.openDoor();
        
        // Return to main menu if not locked
        if (!door.isSystemLocked()) {
             display.showMainMenu();
        }
    }
}
