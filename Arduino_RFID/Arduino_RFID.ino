#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SS_PIN    10
#define RST_PIN   9
#define SERVO_PIN A3

#define I2C_ADDR 0x27 


#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;

byte authorizedUID1[4] = {23, 39, 181, 103};
byte authorizedUID2[4] = {108, 199, 30, 73}; 
byte authorizedUID3[4] = {23,55,244,102};  
int angle = 0;

void setup() {
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(SERVO_PIN);
  servo.write(angle); 
}

void loop() {
  lcd.clear();
  lcd.print("Tap Your Card");

  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) { 
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (isAuthorized(rfid.uid.uidByte)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Granted");
        delay(1000);

        if (angle == 90) {
          angle = 0;
        } else {
          angle = 90;
        }

        servo.write(angle);
        
        

        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], DEC);
        }
        Serial.println();

        delay(4000);
        lcd.clear();


        if (angle == 0) {
          angle = 90;
        }
        else {
        angle =0;
        }

        servo.write(angle);
        lcd.setCursor(0, 0);
        lcd.print("Door Locked");
        delay(2000);
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Access Denied");
        delay(3000);
      }

      rfid.PICC_HaltA(); 
      rfid.PCD_StopCrypto1();
    }
  }
}

bool isAuthorized(byte uid[]) {
  if (checkUID(uid, authorizedUID1) || checkUID(uid, authorizedUID2) || checkUID(uid, authorizedUID3)) {
    return true;
  }
  return false;
}

bool checkUID(byte uid[], byte authorizedUID[]) {
  for (int i = 0; i < 4; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;
    }
  }
  return true;
}