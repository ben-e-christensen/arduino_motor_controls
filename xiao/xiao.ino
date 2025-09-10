#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice dev) override {
    if (!dev.haveManufacturerData()) return;

    std::string mfg = dev.getManufacturerData();
    if (mfg.size() < 6) return;

    const uint8_t* d = (const uint8_t*)mfg.data();
    uint16_t company = d[0] | (uint16_t(d[1]) << 8);
    if (company != 0xFFFF) return;

    uint32_t counter = uint32_t(d[2]) |
                       (uint32_t(d[3]) << 8) |
                       (uint32_t(d[4]) << 16) |
                       (uint32_t(d[5]) << 24);

    Serial.printf("[RX] From: %s  RSSI: %d dBm  Counter: %lu\n",
                  dev.getAddress().toString().c_str(),
                  dev.getRSSI(),
                  (unsigned long)counter);
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("BLE Scanner (ESP32 BLE Arduino)");

  BLEDevice::init("XIAO-RX");
  BLEScan* scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scan->setActiveScan(true);   // request scan response (ok even if none)
  scan->setInterval(45);       // ~28 ms
  scan->setWindow(45);         // continuous
  scan->start(0, false);       // 0 = forever, don't auto-restart
}

void loop() {
  // Keep scan running
  if (!BLEDevice::getScan()->isScanning()) {
    BLEDevice::getScan()->start(0, false);
  }
}
