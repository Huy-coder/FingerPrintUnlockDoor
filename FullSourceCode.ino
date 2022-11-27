/**************Created by Group 1***************
 * Tran Ngo Phuong Huy
 * Nguyen Gia Huy
 * Vu Tien Thuc
 * Bui Huu Nam
************************************************/

#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif

int imgIsConverted(int p);
int checkMatchFingerPrint(int p);
int storageFingerPrint(int p);
int searchExistFingerPrint(int p);
int getFingerprintIDez();
void informationFingerPrintSensor();
void getConnectionStatus();
void switchOption();
void printOption();
void controlSolenoidLock();
void imgIsTaken(int p);
uint8_t readNumber(void);
uint8_t getFingerprintEnroll();
uint8_t getFingerprintID();


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
const int RELAY_PIN = A5;
uint8_t id;
int idUser;

void setup()
{
    Serial.begin(9600);
    while (!Serial);  // For Yun/Leo/Micro/Zero/...
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
    finger.begin(57600);

    getConnectionStatus();
    informationFingerPrintSensor();
}


void loop()                     // run over and over again
{
    switchOption();
}


void getConnectionStatus()
{
    if (finger.verifyPassword()) 
    {
        Serial.println("Found fingerprint sensor!");
    } 
    else 
    {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) {
            delay(1);
        }
    }
}


void informationFingerPrintSensor()
{
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}


uint8_t readNumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


void printOption()
{
    Serial.println("Please input the number you want to do");
    Serial.println("Press '1' to delete all database");
    Serial.println("Press '2' to create new fingerprint");
    Serial.println("Press '3' to validate the finger and unlock the door");
    Serial.println("Press '4' to unlock the door");
}


int chooseNumber()
{
  int number = 0;
  printOption();
  while (1) {
    if (Serial.available() && (Serial.read() == '1')) {
      number = 1;
      break;
    }
    if (Serial.available() && (Serial.read() == '2'))
    {
      number = 2;
      break;
    }
    if (Serial.available() && (Serial.read() == '3'))
    {
      number = 3;
      break;
    }
    if (Serial.available() && (Serial.read() == '4'))
    {
      number = 4;
      break;
    }
  }
  return number;
}


void controlSolenoidLock()
{
    Serial.println("Open the door");
    digitalWrite(RELAY_PIN, HIGH);
    delay(5000);
    Serial.println("Close the door");
    digitalWrite(RELAY_PIN, LOW);
}


void switchOption()
{
    int number = chooseNumber();
    switch(number)
    {
    case 1:
        finger.emptyDatabase();
        Serial.println("Now database is empty :)");
        break;

    case 2:
        Serial.println("Ready to enroll a fingerprint!");
        Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
        id = readNumber();
        if (id == 0) 
        {
            Serial.println("Please Try Again");
            return;
        }
        Serial.print("Enrolling ID #");
        Serial.println(id);
        while (!  getFingerprintEnroll() );
        break;
      
    case 3:
        Serial.println("Waiting for valid finger…");
        while(1)
        {
        int idUser = getFingerprintIDez();
        if(idUser >= 1 && idUser <= 127)
            break;
        }
        delay(50);
        break;
      
    case 4:
        controlSolenoidLock();
        break;

    default:
        break;
    }
}


void imgIsTaken(int p)
{
    switch (p) 
    {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
    }
}


int imgIsCoverted(int p)
{
    switch (p) 
    {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            return 1;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }
}


int checkMatchFingerPrint(int p)
{
    if (p == FINGERPRINT_OK) 
    {
        Serial.println("Prints matched!");
        return 1;
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
        Serial.println("Communication error");
        return p;
    } 
    else if (p == FINGERPRINT_ENROLLMISMATCH) 
    {
        Serial.println("Fingerprints did not match");
        return p;
    } 
    else 
    {
        Serial.println("Unknown error");
        return p;
    }
}


int storageFingerPrint(int p)
{
    if (p == FINGERPRINT_OK) 
    {
        Serial.println("Stored!");
        return 1;
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
        Serial.println("Communication error");
        return p;
    } 
    else if (p == FINGERPRINT_BADLOCATION) 
    {
        Serial.println("Could not store in that location");
        return p;
    } 
    else if (p == FINGERPRINT_FLASHERR) 
    {
        Serial.println("Error writing to flash");
        return p;
    } 
    else 
    {
        Serial.println("Unknown error");
        return p;
    }
}


int searchExistFingerPrint(int p)
{
    if (p == FINGERPRINT_OK) 
    {
        Serial.println("Found a print match!");
        return 1;
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
        Serial.println("Communication error");
        return p;
    } 
    else if (p == FINGERPRINT_NOTFOUND) 
    {
        Serial.println("Did not find a match");
        return p;
    } 
    else 
    {
        Serial.println("Unknown error");
        return p;
    }
}


uint8_t getFingerprintEnroll() {

    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #");
    Serial.println(id);
    while (p != FINGERPRINT_OK) 
    {
        p = finger.getImage();
        imgIsTaken(p);
    }

    // OK success!

    p = finger.image2Tz(1);
    imgIsCoverted(p);

    Serial.println("Remove finger");
    p = 0;
    while (p != FINGERPRINT_NOFINGER) 
    {
        p = finger.getImage();
    }
    Serial.print("ID "); Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    while (p != FINGERPRINT_OK) 
    {
        p = finger.getImage();
        imgIsTaken(p);
    }

    // OK success!

    p = finger.image2Tz(2);
    imgIsCoverted(p);

    // OK converted!
    Serial.print("Creating model for #");
    Serial.println(id);

    p = finger.createModel();
    checkMatchFingerPrint(p);

    Serial.print("ID ");
    Serial.println(id);
    p = finger.storeModel(id);
    storageFingerPrint(p);
    return true;
}


uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    imgIsTaken(p);


    // OK success!
    p = finger.image2Tz();
    imgIsCoverted(p);


    // OK converted!
    p = finger.fingerFastSearch();
    searchExistFingerPrint(p);


    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
}


int getFingerprintIDez() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)  
        return -1;
    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)  
        return -1;
    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)  
        return -1;
    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    controlSolenoidLock();
    return finger.fingerID;
}
