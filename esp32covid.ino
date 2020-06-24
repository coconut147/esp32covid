
#include "Arduino.h"
#include <M5StickC.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEScan *scanner;
std::map<std::string, unsigned long> seenNotifiers;
bool NewId = false;

TFT_eSprite tftSprite = TFT_eSprite(&M5.Lcd); 

// Scan update time, 5 seems to be a good value.
#define SCAN_TIME_SECONDS 1
#define SCAN_TIME_LONG_SECONDS 5

// When to forget old senders.
#define FORGET_AFTER_SECONDS 5



void onNewNotifierFound() {
  Serial.println("Found a new ID!");
  NewId = true;
}


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called when a BLE device is discovered.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    if (!advertisedDevice.haveServiceUUID()
        || !advertisedDevice.getServiceUUID().equals(BLEUUID((uint16_t) 0xfd6f))
      ) {
      return;
    }

    if (!seenNotifiers.count(advertisedDevice.getAddress().toString())) {
      // New notifier found.
      onNewNotifierFound();
    }
    else {
      // We've already seen this one.
      
    }
    Serial.printf("... %s \r\n", advertisedDevice.getAddress().toString().c_str());
    
    // Remember, with time.
    seenNotifiers[advertisedDevice.getAddress().toString()] = millis();
  }
};


void setup() {
  //Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  //Heltec.display->flipScreenVertically();
  //Heltec.display->setFont(ArialMT_Plain_10);
  //Heltec.display->clear();
  //Heltec.display->setFont(ArialMT_Plain_24);
  //Heltec.display->drawString(0, 0, "esp32Covid");
  //Heltec.display->display();


  
  // Initialize scanner.
  BLEDevice::init("ESP");
  scanner = BLEDevice::getScan();
  scanner->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scanner->setActiveScan(true); // Active scan uses more power, but gets results faster.
  scanner->setInterval(100);
  scanner->setWindow(99);

  // Setup Serial
  Serial.begin(115200);
  Serial.println("esp32Covid");

    // Setup M5Sstick
    M5.begin();
    M5.Lcd.setRotation(3);
    tftSprite.createSprite(160, 80);
    tftSprite.setRotation(3);
    M5.Axp.EnableCoulombcounter();
    tftSprite.fillSprite(BLACK);
    tftSprite.setCursor(0, 0, 2);
    tftSprite.printf("esp32covid");
    tftSprite.pushSprite(0, 0);
    delay(2000);
}


void forgetOldNotifiers() {
  for (auto const &notifier : seenNotifiers) {
    if (notifier.second + (FORGET_AFTER_SECONDS * 1000) < millis()) {
      Serial.printf("-   %s \r\n", notifier.first.c_str());
      seenNotifiers.erase(notifier.first);
    }
  }
}

int progress = 1;
char ProgressChar = '-';

void loop() {


  // Scan.
  scanner->start(SCAN_TIME_SECONDS, false);

  // Cleanup.
  scanner->clearResults();
  forgetOldNotifiers();

  if(NewId)
  {
      M5.Lcd.fillScreen(RED);
      delay(100);
      M5.Lcd.fillScreen(BLACK);
      delay(100);
      M5.Lcd.fillScreen(GREEN);
      delay(100);
      M5.Lcd.fillScreen(BLACK);
      delay(100);
      M5.Lcd.fillScreen(BLUE);
      delay(100);
      M5.Lcd.fillScreen(BLACK);   
      NewId = false;
  }

  
    Serial.printf("Count: %d \r\n", seenNotifiers.size());
  
  
    // clear the display
    //Heltec.display->clear();
    // draw the current demo method
  
    //Heltec.display->setFont(ArialMT_Plain_10);

    switch(progress++)
    {
      case 0:
        // Scan a longer period
        scanner->start(SCAN_TIME_LONG_SECONDS, false);
        ProgressChar = '-';
        break;
      case 1:
        ProgressChar = '\\';
        break;
      case 2:
        ProgressChar = '|';
        break;
      case 3:
        ProgressChar = '/';
        break;
      case 4:
        ProgressChar = '-';
        break;
      case 5:
        ProgressChar = '\\';
        break;
      case 6:
        ProgressChar = '|';
        break;
      case 7:
        ProgressChar = '/';
        break;
      case 8:
        ProgressChar = '*';
        progress = 0;
        break;
    }

    
    tftSprite.fillSprite(BLACK);
    tftSprite.setCursor(0, 0, 1);
    tftSprite.setCursor(0, 0, 1);
    tftSprite.printf("Found %d Apps [%c]\r\n", seenNotifiers.size(), ProgressChar);
    
    //int line = 12;
    for (auto const &notifier : seenNotifiers) 
    {
      tftSprite.printf("%s\r\n",notifier.first.c_str());
    }

    tftSprite.printf("Bat:\r\n  V: %.3fv  I: %.3fma\r\n", M5.Axp.GetBatVoltage(), M5.Axp.GetBatCurrent()); 
    tftSprite.printf("Bat power %.3fmw\r\n", M5.Axp.GetBatPower());
    tftSprite.printf("AXP Temp: %.1fC \r\n", M5.Axp.GetTempInAXP192());
    
    // 0x01 long press(1s), 0x02 press
    if(M5.Axp.GetBtnPress() == 0x02) 
    {
        esp_restart();
    }


    // write the buffer to the display
     tftSprite.pushSprite(0, 0);

         delay(1000);
  

}
