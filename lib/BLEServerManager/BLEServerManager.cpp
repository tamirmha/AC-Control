#include "BLEServerManager.h"
#include <utility>


// ** Callback class for handling BLE writes **
CharacteristicCallbacks::CharacteristicCallbacks(BLEServerManager* mgr, int idx, const char* uuid) 
    : manager(mgr), deviceIndex(idx), characteristicUUID(uuid) {}

void CharacteristicCallbacks::onWrite(BLECharacteristic* characteristic) {  
    std::string value = characteristic->getValue();
    if (!value.empty()) {
        Serial.printf("Received update for UUID %s: %s\n", characteristicUUID.c_str(), value.c_str());
        
        int characteristicIndex = -1;
        if (characteristicUUID == VENT_SPEED_UUID)  characteristicIndex = 0;
        else if (characteristicUUID == MODE_UUID)   characteristicIndex = 1;
        else if (characteristicUUID == STATE_UUID)  characteristicIndex = 2;
        else if (characteristicUUID == TEMP_UUID )  characteristicIndex = 3;
        manager->onWriteMsg = value;
        manager->onWriteCharInd = characteristicIndex;
        manager->onWrite = true;
    }
}

void BLEServerManager::begin(std::string ble_server_name) {
    Serial.println("Initializing BLE...");
    BLEDevice::init(std::move(ble_server_name));
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(this); // Set callback to track connected clients

    // AC Service
    BLEService* Service = pServer->createService(SERVICE_UUID);
    pVentSpeed = createCharacteristic(Service, VENT_SPEED_UUID, "AUTO");
    pMode = createCharacteristic(Service, MODE_UUID, "COOL");  // 0 = Cool, 1 = Heat
    pState = createCharacteristic(Service, STATE_UUID, "OFF");
    pVoltage = createCharacteristic(Service, VOLTAGE_UUID, "0", true);
    pTemp = createCharacteristic(Service, TEMP_UUID, "23");
    Service->start();

    pVentSpeed->setCallbacks(&VentSpeedCallback);
    pMode->setCallbacks(&ModeCallback);
    pState->setCallbacks(&StateCallback);
    pTemp->setCallbacks(&TempCallback);
    pVoltage->setCallbacks(&VoltageCallback);

    // Start advertising the services
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    BLEDevice::startAdvertising();
    Serial.println("BLE Server started!");

    // Enable Watchdog Timer
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(nullptr);
}

bool BLEServerManager::loop()
{
    esp_task_wdt_reset();  // Feed the watchdog timer
    if (onWrite)
    {
        updateDeviceState(onWriteCharInd, onWriteMsg.c_str());
        //Print all devices state
        printDevicesState();
        updateCharacteristic(pVoltage, savedState.voltage);
        onWrite = false;
        return true;
    }
    return false;
}

void BLEServerManager::onConnect(BLEServer* pServer)  {
    Serial.printf("Client Connected. Total Clients: %d\n", pServer->getConnectedCount() +1);
    updateCharacteristic(pVoltage, savedState.voltage);
    BLEDevice::startAdvertising();
}

void BLEServerManager::onDisconnect(BLEServer* pServer)  {
    Serial.printf("Client Disconnected. Total Clients: %d\n", pServer->getConnectedCount()-1);
    Serial.println();
    delay(1000);  // Short delay before restarting advertising
    BLEDevice::startAdvertising();
}

void BLEServerManager::printDevicesState()
{
    Serial.printf("Vent Speed: %d, ", savedState.fanSpeed);
    Serial.printf("Mode: %d, ", savedState.mode);
    Serial.printf("State: %d, ", savedState.powerState);
    Serial.printf("Temperature: %d, ", savedState.temperature);
    Serial.printf("Voltage: %f\n", savedState.voltage);
}

void BLEServerManager::updateDeviceState(int index, const String& value) {
    Serial.printf("Updating device state: %d, %s\n", index, value.c_str());
    switch (index) {
        case 0:
            if (value == "p_auto")      savedState.fanSpeed = AUTO;
            else if (value == "po_low") savedState.fanSpeed = P_LOW;
            else if (value == "medium") savedState.fanSpeed = MEDIUM;
            else if (value == "p_high") savedState.fanSpeed = P_HIGH;
            break;
        case 1:
            if (value == "cool")        savedState.mode = COOL;
            else if (value == "heat")   savedState.mode = HEAT;
            else if (value == "fan")    savedState.mode = FAN;
            break;
        case 2:
            if (value == "off")     savedState.powerState = OFF;
            else if (value == "on") savedState.powerState = ON;
            break;
        case 3:
            savedState.temperature = value.toInt();
            break;
        default:
            Serial.println("Invalid index");
            break;
    }
}
