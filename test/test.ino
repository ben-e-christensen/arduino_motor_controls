/*
  ESP32-C3 BLE Server
  This script creates a BLE server with a specific service and characteristic.
  It waits for a client to connect and write data to the characteristic,
  then prints the received data to the Serial Monitor.
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// These UUIDs (Universally Unique IDentifiers) are like addresses for your
// service and data characteristic. The client needs these to know where to
// send data. You can generate your own here: https://www.uuidgenerator.net/
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// This class handles events for the characteristic, like when a client writes data.
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // FIX: Changed std::string to String to match the return type from getValue()
      // in some versions of the ESP32 library.
      String value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("Received Value: ");
        Serial.println(value);
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Server...");

  // Initialize the BLE device and give it a name
  BLEDevice::init("XIAO_ESP32C3_Server");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  
  // Create the BLE Service using the UUID
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic for the service
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  // Set the callback handler for the characteristic
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising the server so clients can find it
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  
  Serial.println("Server started. Waiting for a client to connect...");
}

void loop() {
  // The main work is done by the callbacks, so the loop can be empty
  // or used for other tasks.
  delay(2000);
}
