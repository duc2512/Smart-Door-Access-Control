#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Config.h"

class RFIDManager {
public:
    void begin();
    
    // Checks if a new card is present and reads its serial.
    // Returns true if a card was successfully read.
    // Result is copied to cardBuffer (must be at least CARD_SIZE bytes).
    bool checkForCard(byte *cardBuffer);
    
    String uidToString(byte *card);
    
    void halt(); // Stop crypto after reading

private:
   // MFRC522 object managed internally
};

extern RFIDManager rfid;

#endif
