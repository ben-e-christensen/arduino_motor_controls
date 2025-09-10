/*
  ESP32-C3 BLE Client
  This script scans for the BLE server, connects to it, and writes
  a message with an incrementing counter to its characteristic every two seconds.
*/
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// The UUIDs for the remote service and characteristic we want to connect to.
// **These MUST match the UUIDs on the server!**
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

// Flags to manage connection state
static boolean doConnect = false;
static boolean connected = false;
int counter = 0; // Counter initialized to 0

// Pointers to the remote device and its characteristic
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

// Callback for when the client connects or disconnects from the server
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    //
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("Disconnected from server.");
  }
};

// Function to connect to the server
bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient* pClient  = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remote BLE Server.
    pClient->connect(myDevice);
    Serial.println(" - Connected to server");

    // Obtain a reference to the service on the remote server
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found service");

    // Obtain a reference to the characteristic in the service
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found characteristic");
    connected = true;
    return true;
}

// Callback for when a BLE device is found during a scan
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // We have found a device, see if it is the one we want
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true; // Set flag to attempt connection in the main loop
      Serial.println("Found our device! Now connecting...");
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Client...");
  BLEDevice::init("");

  // Start scanning for the server
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); 
  pBLEScan->start(5, false); // Scan for 5 seconds
}

void loop() {
  // If we found the server and are ready to connect, run the connection logic
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("Successfully connected to the BLE Server.");
    } else {
      Serial.println("Failed to connect to the server.");
    }
    doConnect = false;
  }

  // If connected, write a new value to the characteristic every 2 seconds
  if (connected) {
    String message = "Hello from client! #" + String(counter);
    pRemoteCharacteristic->writeValue(message.c_str(), message.length());
    Serial.print("Sent: ");
    Serial.println(message);
    counter++;
  } else {
    // If not connected, start scanning again
    Serial.println("Not connected. Scanning...");
    BLEDevice::getScan()->start(5, false);
  }
  
  delay(2000); 
}

