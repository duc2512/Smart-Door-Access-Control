#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "Config.h"

// Define ssid and password in cpp file
extern const char* ssid;
extern const char* password;

class NetworkManager {
public:
    void begin();
    void run();
    
    void logEvent(String event, String message);
    void updateStatus(String status);
    void updateLastCard(String uid);

private:
   // Internal helpers if needed
};

extern NetworkManager network;

#endif
