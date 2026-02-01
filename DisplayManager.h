#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Config.h"

class DisplayManager {
public:
    DisplayManager();
    void begin();
    
    void clear();
    void printLine(int row, String text);
    void showMessage(String line1, String line2 = "", String line3 = "", String line4 = "");
    
    // Preset screens
    void showWelcome();
    void showConnectingWiFi();
    void showWiFiResult(bool success);
    void showMainMenu();
    void showAdminMenu();
    void showInputPassword(String maskedPwd);
    void showAccessGranted(String id = "");
    void showAccessDenied();
    void showFingerprintMenu();
    void showRFIDMenu();

private:
    LiquidCrystal_I2C lcd;
};

extern DisplayManager display;

#endif
