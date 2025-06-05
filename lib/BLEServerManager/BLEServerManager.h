#ifndef BLE_SERVER_MANAGER_H
#define BLE_SERVER_MANAGER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <esp_task_wdt.h> // Include Watchdog Timer

#define SERVICE_UUID  "5678abcd-0000-1000-8000-00805f9b34fb"
#define VENT_SPEED_UUID "5678abcd-0001-1000-8000-00805f9b34fb"
#define MODE_UUID "5678abcd-0002-1000-8000-00805f9b34fb"
#define STATE_UUID "5678abcd-0003-1000-8000-00805f9b34fb"
#define TEMP_UUID "5678abcd-0004-1000-8000-00805f9b34fb"
#define VOLTAGE_UUID "5678abcd-0005-1000-8000-00805f9b34fb"

#define WDT_TIMEOUT 10  // 10 seconds timeout


/**
 * @brief Enum for vent speed states.
 */
enum VentSpeed {P_LOW, MEDIUM, P_HIGH, AUTO};

/**
 * @brief Enum for AC mode states.
 */
enum Mode {COOL, HEAT, FAN};

/**
 * @brief Enum for state (on/off).
 */
enum State {OFF, ON};

/**
 * @brief Structure to store the state of the device.
 */
struct DeviceState {
    int id=-1;
    float voltage=0.0;
    State powerState=OFF;
    State prevPowerState=OFF;
    Mode mode=COOL;
    VentSpeed fanSpeed=AUTO;
    int temperature=0;
};

// Store the structure in RTC memory
extern RTC_DATA_ATTR DeviceState savedState;

class BLEServerManager;
// ** Callback class for handling BLE writes **
class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    private:
        BLEServerManager* manager;
        int deviceIndex;
        String characteristicUUID;
    public:
        CharacteristicCallbacks() : manager(nullptr), deviceIndex(-1), characteristicUUID("") {}
        CharacteristicCallbacks(BLEServerManager* mgr, int idx, const char* uuid);

        void onWrite(BLECharacteristic* characteristic) override ;  
    };


class BLEServerManager:public BLEServerCallbacks {

public:
    // DeviceState devicesState;
    bool onWrite = false;
    std::string onWriteMsg;
    int onWriteCharInd = -1;
    BLEServerManager() :
        VentSpeedCallback(this, 0, VENT_SPEED_UUID),
        ModeCallback(this, 0, MODE_UUID),
        StateCallback(this, 0, STATE_UUID),
        TempCallback(this, 0, TEMP_UUID),
        VoltageCallback(this, 0, VOLTAGE_UUID)
        {}

    /**
     * @brief Initialize the BLE server and services.
     */
    void begin(std::string ble_server_name);
    bool loop();
    void onConnect(BLEServer* pServer) override ;
    void onDisconnect(BLEServer* pServer) override ;
    static void printDevicesState();
    static void updateDeviceState(int index, const String& value) ;

    private:
    BLECharacteristic* pVentSpeed;
    BLECharacteristic* pMode;
    BLECharacteristic* pState;
    BLECharacteristic* pTemp;
    BLECharacteristic* pVoltage;
    CharacteristicCallbacks VentSpeedCallback;
    CharacteristicCallbacks ModeCallback;
    CharacteristicCallbacks StateCallback;
    CharacteristicCallbacks TempCallback;
    CharacteristicCallbacks VoltageCallback;
    /**
     * @brief Create a BLE characteristic.
     * @param service The BLE service to which the characteristic belongs.
     * @param uuid The UUID of the characteristic.
     * @param defaultValue The default value of the characteristic.
     * @return A pointer to the created BLE characteristic.
     */
    static BLECharacteristic* createCharacteristic(BLEService* service, const char* uuid, const char* defaultValue, bool notify=false) {
        BLECharacteristic* characteristic;
        if (notify)
             characteristic = service->createCharacteristic(uuid,BLECharacteristic::PROPERTY_READ
                                                       | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY );
        else
            characteristic = service->createCharacteristic(uuid,
                                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
        characteristic->setValue(defaultValue);
        // 🔥 Add CCCD descriptor for notifications
        characteristic->addDescriptor(new BLE2902());  
        return characteristic;
    }
    static void updateCharacteristic(BLECharacteristic* characteristic, float voltage) {
        if (characteristic) {
            String strValue = String(voltage, 2);
            characteristic->setValue(strValue.c_str());
            characteristic->notify();
            Serial.println("Updated BLE Voltage Characteristic: " + strValue);
        }
    }

};


#endif // BLE_SERVER_MANAGER_H