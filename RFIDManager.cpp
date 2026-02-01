#include "RFIDManager.h"

RFIDManager rfid;
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);

void RFIDManager::begin() {
    SPI.begin();
    mfrc522.PCD_Init();
}

bool RFIDManager::checkForCard(byte *cardBuffer) {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return false;
    }
    
    memcpy(cardBuffer, mfrc522.uid.uidByte, CARD_SIZE);
    return true;
}

void RFIDManager::halt() {
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

String RFIDManager::uidToString(byte *card) {
    String uid = "";
    for (int i = 0; i < CARD_SIZE; i++) {
        uid += String(card[i] < 0x10 ? "0" : "");
        uid += String(card[i], HEX);
    }
    uid.toUpperCase();
    return uid;
}
