#include <BleKeyboard.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Declare variables
#define DEVICE_NAME "RFID Scanner"
#define READ_TIMEOUT_MS 3000
#define SS_PIN  5  // ESP32 pin GIOP5 
#define RST_PIN 27 // ESP32 pin GIOP27 

// Declare scanner
// MFRC522 mfrc522(0x28, RST);

MFRC522 rfid(SS_PIN, RST_PIN);

// Declare keyboard with Bluetooth device name
BleKeyboard bleKeyboard("TSS RFID Scanner");

// Forward function declaration
String getRFIDString(MFRC522::Uid);

void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
}

int lastRead = 0;
void loop() {

    bool cardRead = rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial();
    bool timeoutOver = (lastRead + READ_TIMEOUT_MS) < millis();
    
    if (bleKeyboard.isConnected() && cardRead && timeoutOver ) {
        String result = getRFIDString(rfid.uid);        
        Serial.println("RFID: " + result);
        bleKeyboard.println(result);
        lastRead = millis();
    }
    
    delay(50);
}

String getRFIDString(MFRC522::Uid uid) {
    String result = "";
    for (byte i = 0; i < uid.size; i++) {
        if (i != 0) {
            result += " ";
        }
        if (uid.uidByte[i] < 0x10) {
            result += "0";
        }
        result += String(rfid.uid.uidByte[i], HEX);
    }
    result.toUpperCase();
    return result;
}