#include "CocoaCounter.h"

BLEScan *pBLEScan;

void CocoaCounter::init()
{
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
  cocoaNum = 0;
}

void CocoaCounter::Scan(uint8_t scanTime)
{
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
}

uint8_t CocoaCounter::getCocoaNum()
{
  return cocoaNum;
}

void CocoaCounter::Clear()
{
  cocoaNum = 0;
  pBLEScan->clearResults();
}
