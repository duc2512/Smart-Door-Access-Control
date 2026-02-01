#include "FingerprintManager.h"
#include "DisplayManager.h"
#include "StorageManager.h"
#include "InputManager.h" // Needed for detecting abort (*) during enroll

// Need to forward declare InputManager if we use it here, 
// OR just use a simple check if we want to avoid circular dependency.
// But we haven't created InputManager yet. 
// However, the original code used `readKeyChar()` which is needed for aborting enrollment.
// I will include InputManager.h, and I will have to implement it next. 
// For now, I will assume `input.readKeyChar()` exists or similar.
// To satisfy compiler before InputManager is ready, I'll rely on `InputManager.h` existing later.

// Define global serial and sensor
HardwareSerial fingerprintSerial(2);
Adafruit_Fingerprint sensor = Adafruit_Fingerprint(&fingerprintSerial);

FingerprintManager fingerprint;

void FingerprintManager::begin() {
    fingerprintSerial.begin(FINGERPRINT_BAUD, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
    
    if (sensor.verifyPassword()) {
        Serial.println("Tìm thấy cảm biến vân tay!");
    } else {
        Serial.println("Không tìm thấy cảm biến vân tay!");
        display.showMessage("LOI CAM BIEN", "VAN TAY!");
        delay(2000);
    }
}

bool FingerprintManager::isSensorActive() {
    return sensor.verifyPassword();
}

int FingerprintManager::checkFingerprint() {
    uint8_t p = sensor.getImage();
    if (p == FINGERPRINT_NOFINGER) return -2;
    if (p != FINGERPRINT_OK) return -3;

    p = sensor.image2Tz();
    if (p != FINGERPRINT_OK) return -4;

    p = sensor.fingerFastSearch();
    if (p == FINGERPRINT_OK) return sensor.fingerID;
    if (p == FINGERPRINT_NOTFOUND) return -1;
    
    return -5;
}

// Helper to interact with InputManager - we need to make sure InputManager is available
// I will use a forward declaration or include. Since I haven't made InputManager yet, 
// I will depend on it being there. 
// Actually, `checkKey` would be better.
#include "InputManager.h" 

bool FingerprintManager::waitForFingerPlacement(uint8_t slot, unsigned long timeout) {
    unsigned long start = millis();
    while (millis() - start < timeout) {
        int result = sensor.getImage();

        if (result == FINGERPRINT_OK) {
            if (sensor.image2Tz(slot) == FINGERPRINT_OK) {
                return true;
            } else {
                // error conversion
            }
        } else if (result == FINGERPRINT_NOFINGER) {
            // waiting
        }

        // Check for cancel
        if (input.readKey() == '*') return false;
        
        delay(100);
    }
    
    display.showMessage("HET THOI GIAN");
    delay(1500);
    return false;
}

bool FingerprintManager::enrollFingerprint(uint8_t id) {
    if (id == 0) {
        display.showMessage("DA DAY BO NHO!", "XOA BOT VAN TAY");
        delay(2000);
        return false;
    }

    display.showMessage("DANG KY ID: " + String(id), "DAT NGON TAY LAN 1");
    if (!waitForFingerPlacement(1, 10000)) return false;

    display.showMessage("BO NGON TAY RA");
    unsigned long start = millis();
    while (sensor.getImage() != FINGERPRINT_NOFINGER && millis() - start < 10000) {
        if (input.readKey() == '*') return false;
        delay(100);
    }

    display.showMessage("DAT LAI LAN 2");
    if (!waitForFingerPlacement(2, 10000)) return false;

    if (sensor.createModel() != FINGERPRINT_OK) {
        display.showMessage("MAU KHONG KHOP!");
        delay(2000);
        return false;
    }

    if (sensor.storeModel(id) != FINGERPRINT_OK) {
        display.showMessage("LOI LUU MAU!");
        delay(2000);
        return false;
    }

    storage.markFingerprintID(id, true);
    display.showMessage("DANG KY THANH CONG", "ID: " + String(id));
    delay(2000);
    return true;
}

bool FingerprintManager::deleteFingerprint(uint8_t id) {
    if (id == 0 || id > MAX_FINGERPRINTS) return false;
    
    if (sensor.deleteModel(id) == FINGERPRINT_OK) {
        storage.markFingerprintID(id, false);
        return true;
    }
    return false;
}
