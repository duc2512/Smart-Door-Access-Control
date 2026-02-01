#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Config.h"

class StorageManager {
public:
    void begin();
    
    // Password
    String getPassword();
    void setPassword(String newPwd);
    
    // RFID
    bool isCardStored(byte *card);
    void storeCard(byte *card);
    bool removeCard(byte *card);
    // Helper to list cards - returns a string or prints to Serial? 
    // For now, let's just expose a way to check if an address has a card
    bool getCardAt(int index, byte *buffer);

    // Fingerprint
    void initFingerprintMemory();
    uint8_t getNextAvailableFingerprintID();
    void markFingerprintID(uint8_t id, bool used);
    bool isFingerprintIDUsed(uint8_t id);

private:
    const int CARD_EEPROM_SIZE = MAX_CARDS * CARD_SIZE; 
    // Helper to calculate address
    int getCardAddr(int index);
};

extern StorageManager storage;

#endif
