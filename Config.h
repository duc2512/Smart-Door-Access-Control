#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==== Blynk Info ====
#define BLYNK_TEMPLATE_ID "TMPL69KGNKrTW"
#define BLYNK_TEMPLATE_NAME "SMART DOOR"
#define BLYNK_AUTH_TOKEN "7mnt23AXWcLs8Mr6yxY6RAIiryu9jT12"

// ==== WiFi Config ====
// These are variable in the original code, but we can define defaults here
extern const char* ssid;
extern const char* password;

// ==== Fingerprint ====
#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17
#define FINGERPRINT_BAUD 57600
#define MAX_FINGERPRINTS 20
#define FINGERPRINT_START_ADDR 300

// ==== RFID ====
#define RFID_SS_PIN 5
#define RFID_RST_PIN 27
#define MAX_CARDS 20
#define CARD_SIZE 4
#define EEPROM_CARD_START_ADDR 0

// ==== Door / OUTPUT ====
#define RELAY_PIN 4        // SUCCESS_LED_N_RELAY_PIN
#define LED_SUCCESS_PIN 4  // Same as RELAY
#define LED_FAILED_PIN 15
#define BUZZER_PIN 25

// ==== LCD ====
#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

// ==== Keypad (TTP229) ====
#define KEYPAD_SCL_PIN 32
#define KEYPAD_SDO_PIN 33
#define DEBOUNCE_DELAY 300

// ==== Password & Storage ====
#define PASSWORD_ADDR 400

#endif
