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
MFRC522::StatusCode status;

int code[] = {02, 176, 04, 52}; //This is the stored UID (Unlock Card)
int codeRead = 0;
String uidString;

union memory {
  float fl;
  byte asBytes[4];
};
union memory conversion;

void setup() {
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
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
    if (readRFID()) {
      float value = 139482.23f;
      Serial.print("Writing value");
      Serial.println(value);
      writeWalletAmountToCard(value);
      delay(100);
      Serial.println("Reading value");
      readWalletAmountOnCard();
    }
  } else {
    showLedAccept(0);
  }
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  delay(100);
}

void showLedAccept(int code) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (code == 0) {
      leds[i] = CRGB::Black;
    } else if (code == 1) {
      leds[i] = CRGB::Green;
    } else {
      leds[i] = CRGB::Red;
    }
  }
  FastLED.show();
}

boolean readRFID() {

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
  return match;
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

float readWalletAmountOnCard() {
  byte bufferRead[18];

  byte block = 1;
  byte len = 18;

  //------------------------------------------- GET FIRST NAME
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  status = rfid.MIFARE_Read(block, bufferRead, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  for (int i = 0; i < len; i++) Serial.print(bufferRead[i], HEX);
  Serial.println("");

  for (int i = 0; i < 4; i++) conversion.asBytes[i] = bufferRead[i];

  Serial.println(conversion.fl);
  return 0.0f;
}

void writeWalletAmountToCard(float amount) {
  byte buffer[34];
  long l = *(long*) &amount;
  buffer[0] = l & 0x00FF;
  buffer[1] = (l >> 8) & 0x00FF;
  buffer[2] = (l >> 16) & 0x00FF;
  buffer[3] = l >> 24;
  byte block = 1;
  //Serial.println(F("Authenticating using key A..."));
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  // Write block
  status = rfid.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
}
