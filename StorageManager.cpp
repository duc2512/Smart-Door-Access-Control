#include "StorageManager.h"

StorageManager storage;

void StorageManager::begin() {
    EEPROM.begin(512);
    initFingerprintMemory();
}

// ==== Password ====
String StorageManager::getPassword() {
    int addr = PASSWORD_ADDR;
    int pwdLength = EEPROM.read(addr);
    String pwd = "";

    if (pwdLength > 0 && pwdLength < 20) {
        addr++;
        for (int i = 0; i < pwdLength; i++) {
            pwd += (char)EEPROM.read(addr + i);
        }
    } else {
        pwd = "1234"; // Default
        setPassword(pwd);
    }
    return pwd;
}

void StorageManager::setPassword(String newPwd) {
    int addr = PASSWORD_ADDR;
    EEPROM.write(addr, newPwd.length());
    addr++;
    for (int i = 0; i < newPwd.length(); i++) {
        EEPROM.write(addr + i, newPwd[i]);
    }
    EEPROM.commit();
}

// ==== RFID ====
int StorageManager::getCardAddr(int index) {
    return EEPROM_CARD_START_ADDR + index * CARD_SIZE;
}

bool StorageManager::isCardStored(byte *card) {
    for (int i = 0; i < MAX_CARDS; i++) {
        int addr = getCardAddr(i);
        bool match = true;
        for (int j = 0; j < CARD_SIZE; j++) {
            if (EEPROM.read(addr + j) != card[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

void StorageManager::storeCard(byte *card) {
    for (int i = 0; i < MAX_CARDS; i++) {
        int addr = getCardAddr(i);
        if (EEPROM.read(addr) == 0xFF) {
            for (int j = 0; j < CARD_SIZE; j++) {
                EEPROM.write(addr + j, card[j]);
            }
            EEPROM.commit();
            break;
        }
    }
}

bool StorageManager::removeCard(byte *card) {
    for (int i = 0; i < MAX_CARDS; i++) {
        int addr = getCardAddr(i);
        bool match = true;
        for (int j = 0; j < CARD_SIZE; j++) {
            if (EEPROM.read(addr + j) != card[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            for (int j = 0; j < CARD_SIZE; j++) {
                EEPROM.write(addr + j, 0xFF);
            }
            EEPROM.commit();
            return true;
        }
    }
    return false;
}

bool StorageManager::getCardAt(int index, byte *buffer) {
    int addr = getCardAddr(index);
    if (EEPROM.read(addr) == 0xFF) return false;
    
    for(int j=0; j<CARD_SIZE; j++) {
        buffer[j] = EEPROM.read(addr + j);
    }
    return true;
}

// ==== Fingerprint ====
void StorageManager::initFingerprintMemory() {
    uint8_t checkValue = EEPROM.read(FINGERPRINT_START_ADDR);
    if (checkValue != 0xAA) {
        for (int i = 0; i <= MAX_FINGERPRINTS; i++) {
            EEPROM.write(FINGERPRINT_START_ADDR + i, 0);
        }
        EEPROM.write(FINGERPRINT_START_ADDR, 0xAA);
        EEPROM.commit();
    }
}

uint8_t StorageManager::getNextAvailableFingerprintID() {
    for (int i = 1; i <= MAX_FINGERPRINTS; i++) {
        if (EEPROM.read(FINGERPRINT_START_ADDR + i) != 1) {
            return i;
        }
    }
    return 0;
}

void StorageManager::markFingerprintID(uint8_t id, bool used) {
    EEPROM.write(FINGERPRINT_START_ADDR + id, used ? 1 : 0);
    EEPROM.commit();
}

bool StorageManager::isFingerprintIDUsed(uint8_t id) {
     return (EEPROM.read(FINGERPRINT_START_ADDR + id) == 1);
}
