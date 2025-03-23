#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define MOISTURE_PIN 34  // Analog pin for soil moisture sensor

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// Custom BLE Service & Characteristic UUIDs
#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "87654321-4321-6789-4321-fedcba987654"

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

void setup() {
    Serial.begin(115200);
    
    // Initialize BLE
    BLEDevice::init("ESP32 Soil Sensor");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    // Create BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // Create BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
    pCharacteristic->addDescriptor(new BLE2902());
    
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();
    
    Serial.println("BLE Soil Moisture Monitor Started!");
}

void loop() {
    int moisture = analogRead(MOISTURE_PIN);
    int moisturePercentage = map(moisture, 4095, 1200, 0, 100);  // Convert to percentage
    
    Serial.print("Soil Moisture: ");
    Serial.print(moisturePercentage);
    Serial.println("%");

    if (deviceConnected) {
        String moistureData = "Moisture:" + String(moisturePercentage) + "%";
        pCharacteristic->setValue(moistureData.c_str());
        pCharacteristic->notify();
    }

    delay(2000);  // Update every 2 seconds
}
