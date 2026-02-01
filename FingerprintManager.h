#ifndef FINGERPRINT_MANAGER_H
#define FINGERPRINT_MANAGER_H

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include "Config.h"

class FingerprintManager {
public:
    void begin();
    
    // Returns ID if found, -1 if not found, -2 if no finger, other negative for errors
    int checkFingerprint();
    
    bool enrollFingerprint(uint8_t id);
    bool deleteFingerprint(uint8_t id);
    
    bool isSensorActive();

private:
   // We will manage the serial and sensor object internally or via pointer
   // to avoid global declaration issues in header if possible, but 
   // Adafruit library expects a Stream. 
   // We can declare the global object in cpp.
   
   bool waitForFingerPlacement(uint8_t slot, unsigned long timeout);
};

extern FingerprintManager fingerprint;

#endif
