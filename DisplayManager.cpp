#include "DisplayManager.h"

DisplayManager display;

DisplayManager::DisplayManager() : lcd(LCD_ADDR, LCD_COLS, LCD_ROWS) {}

void DisplayManager::begin() {
    lcd.init();
    lcd.backlight();
    showWelcome();
}

void DisplayManager::clear() {
    lcd.clear();
}

void DisplayManager::printLine(int row, String text) {
    lcd.setCursor(0, row);
    lcd.print(text);
}

void DisplayManager::showMessage(String line1, String line2, String line3, String line4) {
    lcd.clear();
    if (line1 != "") { lcd.setCursor(0, 0); lcd.print(line1); }
    if (line2 != "") { lcd.setCursor(0, 1); lcd.print(line2); }
    if (line3 != "") { lcd.setCursor(0, 2); lcd.print(line3); }
    if (line4 != "") { lcd.setCursor(0, 3); lcd.print(line4); }
}

void DisplayManager::showWelcome() {
    showMessage("Khoi dong...", "Vui long cho...");
}

void DisplayManager::showConnectingWiFi() {
    lcd.setCursor(0, 1);
    lcd.print("Ket noi WiFi");
}

void DisplayManager::showWiFiResult(bool success) {
    lcd.setCursor(0, 2);
    if (success) lcd.print("WiFi: OK");
    else lcd.print("WiFi: THAT BAI");
    delay(1000);
}

void DisplayManager::showMainMenu() {
    showMessage("HE THONG SMART DOOR", "", "RFID|PIN|VAN TAY", "QUAN LY: PHIM *");
}

void DisplayManager::showAdminMenu() {
    showMessage("1.Doi Mat Khau", "2.Quan Ly RFID", "3.Quan Ly Van Tay", "*.Thoat");
}

void DisplayManager::showInputPassword(String maskedPwd) {
    lcd.setCursor(0, 0);
    lcd.print("NHAP MAT KHAU:");
    lcd.setCursor(0, 1);
    lcd.print("MK: " + maskedPwd);
}

void DisplayManager::showAccessGranted(String id) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(id != "" ? "ID: " + id : "MAT KHAU DUNG");
    lcd.setCursor(0, 1);
    lcd.print("MO CUA TRONG 5s");
}

void DisplayManager::showAccessDenied() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KHONG HOP LE!");
    lcd.setCursor(0, 1);
    lcd.print("VUI LONG THU LAI");
}

void DisplayManager::showFingerprintMenu() {
    showMessage("A.Dang ky van tay", "B.Xoa van tay", "C.Xem danh sach", "*.Thoat");
}

void DisplayManager::showRFIDMenu() {
    showMessage("QUAN LY RFID:", "A.Them The  C.Xem", "B.Xoa The", "*.Thoat");
}
