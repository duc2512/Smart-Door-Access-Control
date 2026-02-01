#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>

#include "Config.h"
#include "SystemState.h"

// Modules
#include "StorageManager.h"
#include "DisplayManager.h"
#include "DoorController.h"
#include "NetworkManager.h"
#include "FingerprintManager.h"
#include "RFIDManager.h"
#include "InputManager.h"

// Global State
SystemMode currentMode = MODE_SCAN_CARD;
String inputPassword = "";
String adminPasswordInput = "";
bool isEnteringAdmin = false;

// Password Change State
int changeStage = 0; // 0: None, 1: Old, 2: New, 3: Confirm
String newPassword = "";

unsigned long lastFingerprintCheck = 0;
const unsigned long checkInterval = 100; 

// Prototypes
void handleKeyInput(char key);
void handleAdminInput(char key);
void handlePasswordMode(char key);
void handleAdminMenu(char key);
void handleChangePassword(char key);
void handleRFIDMenu(char key);
void handleFingerprintMenu(char key);

void setup() {
    Serial.begin(9600);
    
    // Initialize HW
    Wire.begin();
    
    storage.begin();
    display.begin();
    door.begin();
    
    // Network might be time consuming
    network.begin();
    
    fingerprint.begin();
    rfid.begin();
    input.begin();
    
    display.showMainMenu();
}

void loop() {
    network.run();
    
    // Check Lock
    if (door.isSystemLocked()) {
        door.handleLockLogic();
        return; 
    }
    
    unsigned long currentMillis = millis();
    
    // Check Fingerprint (Periodic)
    if (currentMillis - lastFingerprintCheck >= checkInterval) {
        lastFingerprintCheck = currentMillis;
        if ((currentMode == MODE_SCAN_CARD || currentMode == MODE_ENTER_PASSWORD)) {
            int fid = fingerprint.checkFingerprint();
            if (fid > 0) {
                Serial.println("Xac thuc ID Van tay: " + String(fid));
                display.showAccessGranted("VAN TAY ID " + String(fid));
                network.logEvent("bao_dong", "Hệ thống xác thực vân tay hợp lệ");
                door.openDoor();
                door.resetFailedAttempts();
                delay(2000);
                display.showMainMenu();
            } else if (fid == -1) {
                // Not Match
                display.showAccessDenied();
                network.logEvent("bao_dong", "Cảnh báo vân tay không hợp lệ");
                door.registerFailedAttempt();
                delay(2000);
                display.showMainMenu();
            }
        }
    }
    
    // Check RFID
    byte cardBuffer[CARD_SIZE];
    if ((currentMode == MODE_SCAN_CARD || currentMode == MODE_ENTER_PASSWORD) && rfid.checkForCard(cardBuffer)) {
        String uid = rfid.uidToString(cardBuffer);
        Serial.println("Scan Card: " + uid);
        network.updateLastCard(uid);
        
        if (storage.isCardStored(cardBuffer)) {
            display.showAccessGranted("THE HOP LE");
            network.logEvent("bao_dong", "Hệ thống xác thực thẻ hợp lệ");
            door.openDoor();
            door.resetFailedAttempts();
        } else {
            display.showAccessDenied();
            network.logEvent("bao_dong", "Cảnh báo thẻ không hợp lệ");
            door.registerFailedAttempt();
        }
        rfid.halt();
        delay(2000);
        display.showMainMenu();
    }
    
    // Check Keypad
    char key = input.readKey();
    if (key != 0) {
        Serial.print("Key: "); Serial.println(key);
        handleKeyInput(key);
    }
    
    delay(50);
}

void handleKeyInput(char key) {
    if (isEnteringAdmin) {
        handleAdminInput(key);
        return;
    }

    switch (currentMode) {
        case MODE_SCAN_CARD:
            if (key >= '0' && key <= '9') {
                currentMode = MODE_ENTER_PASSWORD;
                inputPassword = String(key);
                display.showInputPassword("*");
            } else if (key == '*') {
                isEnteringAdmin = true;
                adminPasswordInput = "";
                display.showMessage("NHAP MK ADMIN:", "");
            }
            break;
            
        case MODE_ENTER_PASSWORD:
            handlePasswordMode(key);
            break;
            
        case MODE_ADMIN_MENU:
            handleAdminMenu(key);
            break;
            
        case MODE_CHANGE_PASSWORD:
            handleChangePassword(key);
            break;
            
        case MODE_RFID_MENU:
        case MODE_ADD_CARD:
        case MODE_DEL_CARD:
        case MODE_VIEW_CARDS:
            handleRFIDMenu(key);
            break;

        case MODE_FINGERPRINT_MENU:
        case MODE_ADD_FINGERPRINT:
        case MODE_DELETE_FINGERPRINT:
        case MODE_VIEW_FINGERPRINTS:
            handleFingerprintMenu(key);
            break;
    }
}

void handleAdminInput(char key) {
    if (key >= '0' && key <= '9') {
        adminPasswordInput += key;
        String masked = "";
        for(int i=0; i<adminPasswordInput.length(); i++) masked += "*";
        display.printLine(1, masked);
    } else if (key == '#') {
        if (adminPasswordInput == storage.getPassword()) {
            isEnteringAdmin = false;
            currentMode = MODE_ADMIN_MENU;
            display.showAdminMenu();
        } else {
            display.showMessage("SAI MK ADMIN!");
            delay(2000);
            isEnteringAdmin = false;
            currentMode = MODE_SCAN_CARD;
            display.showMainMenu();
        }
    } else if (key == '*') {
        isEnteringAdmin = false;
        currentMode = MODE_SCAN_CARD;
        display.showMainMenu();
    }
}

void handlePasswordMode(char key) {
    if (key >= '0' && key <= '9') {
        inputPassword += key;
        String masked = "";
        for(int i=0; i<inputPassword.length(); i++) masked += "*";
        display.printLine(1, "MK: " + masked);
    } else if (key == '#') {
        if (inputPassword == storage.getPassword()) {
            display.showAccessGranted();
            network.logEvent("bao_dong", "Hệ thống xác thực mật khẩu đúng");
            door.openDoor();
            door.resetFailedAttempts();
            delay(2000);
            currentMode = MODE_SCAN_CARD;
            display.showMainMenu();
        } else {
            // Wrong pass
            door.registerFailedAttempt();
            if (door.isSystemLocked()) {
               network.logEvent("bao_dong", "Nhập sai mật khẩu 5 lần, hệ thống bị khóa!");
            } else {
               display.showMessage("SAI MAT KHAU!", "Con " + String(5 - door.getFailedAttempts()) + " lan");
               delay(2000);
               display.showInputPassword("");
               inputPassword = "";
            }
        }
    } else if (key == '*') {
        currentMode = MODE_SCAN_CARD;
        display.showMainMenu();
    }
}

void handleAdminMenu(char key) {
    if (key == '1') {
        currentMode = MODE_CHANGE_PASSWORD;
        changeStage = 1;
        inputPassword = "";
        display.showMessage("DOI MAT KHAU", "Nhap MK cu:");
    } else if (key == '2') {
        currentMode = MODE_RFID_MENU;
        display.showRFIDMenu();
    } else if (key == '3') {
        currentMode = MODE_FINGERPRINT_MENU;
        display.showFingerprintMenu();
    } else if (key == '*') {
        currentMode = MODE_SCAN_CARD;
        display.showMainMenu();
    }
}

void handleChangePassword(char key) {
    if (key >= '0' && key <= '9') {
        inputPassword += key;
        String masked = "";
        for(int i=0; i<inputPassword.length(); i++) masked += "*";
        display.printLine(2, masked);
    } else if (key == '#') {
        if (changeStage == 1) { // Check old pwd
            if (inputPassword == storage.getPassword()) {
                changeStage = 2;
                inputPassword = "";
                display.showMessage("DOI MAT KHAU", "Nhap MK moi:");
            } else {
                display.showMessage("MK CU SAI!");
                delay(2000);
                currentMode = MODE_ADMIN_MENU;
                display.showAdminMenu();
                changeStage = 0;
            }
        } else if (changeStage == 2) { // New pwd input
            newPassword = inputPassword;
            changeStage = 3;
            inputPassword = "";
             display.showMessage("DOI MAT KHAU", "Xac nhan MK moi:");
        } else if (changeStage == 3) { // Confirm
            if (inputPassword == newPassword) {
                storage.setPassword(newPassword);
                display.showMessage("DOI MK THANH CONG!");
                delay(2000);
            } else {
                display.showMessage("KHONG TRUNG KHOP!");
                delay(2000);
            }
            currentMode = MODE_ADMIN_MENU;
            display.showAdminMenu();
            changeStage = 0;
        }
    } else if (key == '*') {
        currentMode = MODE_ADMIN_MENU;
        display.showAdminMenu();
        changeStage = 0;
    }
}

void handleRFIDMenu(char key) {
    // If we are in sub-menus (ADD/DEL/VIEW), we handled them inline in original code or they were blocking.
    // To keep it non-blocking would be better, but the original code was blocking for waitForCard.
    // I will replicate the original semi-blocking behavior for simplicity in this refactor, 
    // or use the state machine.
    // It's cleaner to handle input based on state. 
    
    // If we are just in the MENU selection:
    if (currentMode == MODE_RFID_MENU) {
        if (key == 'A') {
            currentMode = MODE_ADD_CARD;
            display.showMessage("DAT THE DE THEM");
            // Here we need to "wait" for card. 
            // Since loop() handles card checking only in Scan Mode, we need logic here or in loop.
            // Let's implement a blocking check here to match original "waitForCard" simplicity
            // OR change design to be non-blocking. 
            // Original: waitForCard(); storeCard; ...
            
            byte card[4];
            while(!rfid.checkForCard(card)) { delay(100); } // Blocking wait
            
            if (!storage.isCardStored(card)) {
                storage.storeCard(card);
                display.printLine(1, "DA THEM THE");
            } else {
                display.printLine(1, "THE DA TON TAI");
            }
            rfid.halt();
            delay(2000);
            currentMode = MODE_RFID_MENU;
            display.showRFIDMenu();
            
        } else if (key == 'B') {
            currentMode = MODE_DEL_CARD;
            display.showMessage("DAT THE DE XOA");
            byte card[4];
            while(!rfid.checkForCard(card)) { delay(100); } 
            
            if (storage.removeCard(card)) {
                display.printLine(1, "DA XOA THE");
            } else {
                display.printLine(1, "THE CHUA DANG KY");
            }
            rfid.halt();
            delay(2000);
            currentMode = MODE_RFID_MENU;
            display.showRFIDMenu();
            
        } else if (key == 'C') {
            // VIEW CARDS
             display.showMessage("DANH SACH THE:");
             // Need to iterate.
             int row = 1;
             bool found = false;
             for(int i=0; i<MAX_CARDS; i++) {
                 byte c[4];
                 if(storage.getCardAt(i, c)) {
                     found = true;
                     String uid = rfid.uidToString(c);
                     if (row < 4) {
                        display.printLine(row, String(i+1) + ". " + uid);
                        row++;
                     }
                 }
             }
             if(!found) display.printLine(1, "Khong co the.");
             
             // Wait for key to exit
             while(input.readKey() == 0) { delay(100); }
             
             currentMode = MODE_RFID_MENU;
             display.showRFIDMenu();

        } else if (key == '*') {
            currentMode = MODE_ADMIN_MENU;
            display.showAdminMenu();
        }
    }
}

void handleFingerprintMenu(char key) {
    if (currentMode == MODE_FINGERPRINT_MENU) {
        if (key == 'A') {
            currentMode = MODE_ADD_FINGERPRINT;
            uint8_t id = storage.getNextAvailableFingerprintID();
            fingerprint.enrollFingerprint(id);
            currentMode = MODE_FINGERPRINT_MENU;
            display.showFingerprintMenu();
            
        } else if (key == 'B') {
            currentMode = MODE_DELETE_FINGERPRINT;
            display.showMessage("XOA VAN TAY", "NHAP ID:");
            String idStr = "";
            while(true) {
                char k = input.readKey();
                if (k >= '0' && k <= '9') {
                    idStr += k;
                    display.printLine(1, "NHAP ID: " + idStr);
                } else if (k == '#') {
                    int id = idStr.toInt();
                    if(fingerprint.deleteFingerprint(id)) {
                         display.showMessage("DA XOA ID " + String(id));
                    } else {
                         display.showMessage("LOI XOA ID " + String(id));
                    }
                    delay(2000);
                    break;
                } else if (k == '*') break;
                delay(50);
            }
            currentMode = MODE_FINGERPRINT_MENU;
            display.showFingerprintMenu();
            
        } else if (key == 'C') {
            // VIEW FINGERPRINTS
            display.clear();
            display.printLine(0, "VAN TAY DA DK:");
            String list = "";
            int count = 0;
             for(int i=1; i<=MAX_FINGERPRINTS; i++) {
                 if(storage.isFingerprintIDUsed(i)) {
                     list += String(i) + ",";
                     count++;
                 }
             }
             // Simple display logic, might need word wrap if too long
             if (list.length() > 0) {
                 display.printLine(1, list.substring(0, min((unsigned int)20, list.length()))); 
                 if (list.length() > 20) display.printLine(2, list.substring(20, min((unsigned int)40, list.length())));
             } else {
                 display.printLine(1, "CHUA CO VAN TAY");
             }
             display.printLine(3, "TONG: " + String(count));
             
             while(input.readKey() == 0) { delay(100); }
             currentMode = MODE_FINGERPRINT_MENU;
             display.showFingerprintMenu();
             
        } else if (key == '*') {
            currentMode = MODE_ADMIN_MENU;
            display.showAdminMenu();
        }
    }
}
