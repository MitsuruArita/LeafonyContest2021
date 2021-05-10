#ifndef _COCOA_COUNTER_H_
#define _COCOA_COUNTER_H_

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>

//cocoaのサービスuuid
#define COCOA_ID    "fd6f"

static uint8_t cocoaNum;

class CocoaCounter {
  public:
    void init();
    void Scan(uint8_t scanTime);
    void Clear();
    uint8_t getCocoaNum(void);
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //サービスuuidをもっているなら
      if (advertisedDevice.haveServiceUUID())
        if (advertisedDevice.getServiceUUID().toString().find(COCOA_ID) == 4) {
          cocoaNum++;
          Serial.printf("service: %s\n", advertisedDevice.getServiceUUID().toString().c_str());
        }
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

#endif
