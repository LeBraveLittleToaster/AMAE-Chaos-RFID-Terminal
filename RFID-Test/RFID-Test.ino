#include <FastLED.h>
#include <SPI.h>
#include <MFRC522.h>


#define DATA_PIN 4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 3
int BRIGHTNESS = 96;

#define SS_PIN 10
#define RST_PIN 9

CRGB leds[NUM_LEDS];

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

int code[] = {02, 176, 04, 52}; //This is the stored UID (Unlock Card)
int codeRead = 0;
String uidString;

void setup() {
  delay(1000);
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  showLedAccept(0);
  Serial.println(F("Initialized..."));
}

void loop() {
  if (  rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    readRFID();

  }else{
    showLedAccept(0);
  }
  delay(100);
}

void showLedAccept(int code) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (code == 0) {
      leds[i] = CRGB::Black;
    } else if(code == 1) {
      leds[i] = CRGB::Green;
    }else{
      leds[i] = CRGB::Red;
    }
  }
  FastLED.show();
}

void readRFID() {
  rfid.PICC_ReadCardSerial();

  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {

    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  Serial.println("Scanned PICC's UID:");
  printDec(rfid.uid.uidByte, rfid.uid.size);
  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);

  int i = 0;
  boolean match = true;
  while (i < rfid.uid.size) {
    if (!(int(rfid.uid.uidByte[i]) == int(code[i]))) {
      match = false;
    }
    i++;
  }
  if (match) {
    Serial.println("\n*** Unlock ***");
    showLedAccept(1);
  } else {
    Serial.println("\nUnknown Card");
    showLedAccept(-1);
  }
  Serial.println("============================");

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
