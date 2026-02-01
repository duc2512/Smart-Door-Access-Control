#include "DoorController.h"
#include "DisplayManager.h" // For updating display during lock

DoorController door;

void DoorController::begin() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    
    pinMode(LED_FAILED_PIN, OUTPUT);
    digitalWrite(LED_FAILED_PIN, LOW);
    
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    
    lastFailedTime = 0;
    failedAttempts = 0;
    isLocked = false;
}

void DoorController::openDoor() {
    digitalWrite(RELAY_PIN, HIGH);
    // Note: The original code blocked with delay(5000), 
    // but in a real system we might want non-blocking.
    // Sticking to original logic for now for simplicity.
    delay(5000);
    digitalWrite(RELAY_PIN, LOW);
}

void DoorController::signalSuccess() {
    // Usually openDoor handles the success indicator (Relay LED)
    // But we can add specific sounds here if needed
}

void DoorController::signalFailure() {
    digitalWrite(LED_FAILED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Keep LED on for a bit
    delay(2000); 
    digitalWrite(LED_FAILED_PIN, LOW);
}

void DoorController::signalAlarm() {
    for(int i=0; i<3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
    }
}

void DoorController::registerFailedAttempt() {
    failedAttempts++;
    signalFailure();
    if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
        isLocked = true;
        lastFailedTime = millis();
    }
}

void DoorController::resetFailedAttempts() {
    failedAttempts = 0;
    isLocked = false; // Reset lock if success
}

bool DoorController::isSystemLocked() {
    if (isLocked) {
        if (millis() - lastFailedTime < LOCK_DURATION) {
            return true;
        } else {
            isLocked = false;
            failedAttempts = 0;
            return false;
        }
    }
    return false;
}

void DoorController::handleLockLogic() {
    if (isLocked) {
        unsigned long remaining = LOCK_DURATION - (millis() - lastFailedTime);
        if (remaining > 0) {
             // We can update LCD here via DisplayManager if we want constant updates
             static unsigned long lastUpdate = 0;
             if (millis() - lastUpdate > 1000) {
                 int seconds = remaining / 1000;
                 display.showMessage("HE THONG DANG KHOA", 
                                     "Thoi gian con lai:", 
                                     String(seconds/60) + " phut " + String(seconds%60) + " giay");
                 digitalWrite(BUZZER_PIN, HIGH); // Annoying buzzer during lock? (From original code)
                 delay(100);
                 digitalWrite(BUZZER_PIN, LOW); 
                 lastUpdate = millis();
             }
        } else {
            isLocked = false;
            failedAttempts = 0;
            display.showMainMenu();
        }
    }
}

int DoorController::getFailedAttempts() {
    return failedAttempts;
}
