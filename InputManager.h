#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include "Config.h"

class InputManager {
public:
    void begin();
    char readKey();

private:
    const char keyMap[16] = {
      '1','2','3','A',
      '4','5','6','B',
      '7','8','9','C',
      '*','0','#','#'
    };
    unsigned long lastKeyPressTime;
    char lastKey;
    bool keyIsHeld;
};

extern InputManager input;

#endif
