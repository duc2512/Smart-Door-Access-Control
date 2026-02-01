#include "InputManager.h"

InputManager input;

void InputManager::begin() {
    pinMode(KEYPAD_SCL_PIN, OUTPUT);
    pinMode(KEYPAD_SDO_PIN, INPUT);
    digitalWrite(KEYPAD_SCL_PIN, HIGH);
    
    lastKeyPressTime = 0;
    lastKey = 0;
    keyIsHeld = false;
}

char InputManager::readKey() {
    uint16_t data = 0;

    for (int i = 0; i < 16; i++) {
        digitalWrite(KEYPAD_SCL_PIN, LOW);
        delayMicroseconds(2);
        if (digitalRead(KEYPAD_SDO_PIN) == LOW) {
            data |= (1 << i);
        }
        digitalWrite(KEYPAD_SCL_PIN, HIGH);
        delayMicroseconds(2);
    }

    int count = 0, index = -1;
    for (int i = 0; i < 16; i++) {
        if (bitRead(data, i)) {
            count++;
            index = i;
        }
    }

    if (count == 1) {
        char currentKey = keyMap[index];
        unsigned long currentTime = millis();

        if (!keyIsHeld && (currentTime - lastKeyPressTime > DEBOUNCE_DELAY)) {
            keyIsHeld = true;
            lastKey = currentKey;
            lastKeyPressTime = currentTime;
            return currentKey;
        }
    } else if (count == 0 && keyIsHeld) {
        keyIsHeld = false;
    }

    return 0; 
}
