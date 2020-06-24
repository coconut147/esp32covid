
#include "Arduino.h"
//#include "heltec.h"
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEScan *scanner;
std::map<std::string, unsigned long> seenNotifiers;
bool NewId = false;

// Scan update time, 5 seems to be a good value.
#define SCAN_TIME_SECONDS 1
#define SCAN_TIME_LONG_SECONDS 1

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
}


void forgetOldNotifiers() {
  for (auto const &notifier : seenNotifiers) {
    if (notifier.second + (FORGET_AFTER_SECONDS * 1000) < millis()) {
      Serial.printf("-   %s \r\n", notifier.first.c_str());
      seenNotifiers.erase(notifier.first);
    }
  }
}

int progress = 0;
char ProgressChar = '-';

void loop() {


  // Scan.
  scanner->start(SCAN_TIME_SECONDS, false);

  // Cleanup.
  scanner->clearResults();
  forgetOldNotifiers();

  if(NewId)
  {
     //Heltec.display->setFont(ArialMT_Plain_24);
     //Heltec.display->drawString(80, 0, "New!");
     //Heltec.display->display(); 
     delay(1000);
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
        ProgressChar = '-';
        // Scan a longer period
        scanner->start(SCAN_TIME_LONG_SECONDS, false);
        progress = 0;
        break;
    }

    
    //Heltec.display->drawString(0, 0, "Found " + String(seenNotifiers.size())+ " Apps (" + ProgressChar + ")");
    int line = 12;
    for (auto const &notifier : seenNotifiers) 
    {
      //Heltec.display->drawString(0,line, notifier.first.c_str());
      line += 10;
    }
    
  
    // write the buffer to the display
    //Heltec.display->display();

}
