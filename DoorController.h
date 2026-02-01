#ifndef DOOR_CONTROLLER_H
#define DOOR_CONTROLLER_H

#include <Arduino.h>
#include "Config.h"

class DoorController {
public:
    void begin();
    
    void openDoor();
    void signalSuccess();
    void signalFailure();
    void signalAlarm(); // For continuous alarm or specific pattern
    
    // System Lock logic
    bool isSystemLocked();
    void handleLockLogic(); // Called in loop
    void registerFailedAttempt();
    void resetFailedAttempts();
    int getFailedAttempts();

private:
    unsigned long lastFailedTime;
    int failedAttempts;
    bool isLocked;
    const int MAX_FAILED_ATTEMPTS = 5;
    const unsigned long LOCK_DURATION = 300000; // 5 minutes
};

extern DoorController door;

#endif
