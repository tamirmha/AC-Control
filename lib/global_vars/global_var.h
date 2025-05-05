#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H
#pragma once
#include "deep_sleep.h"
#include "Arduino.h" 
#include "ir.h"
#include "BLEServerManager.h"

// Store the structure in RTC memory
RTC_DATA_ATTR DeviceState savedState;

BLEServerManager bleManager;

const char* allowedMACs[] = {
        "9C:9E:6E:C1:0C:5E",  // AC
        "9C:9E:6E:C1:09:E2",
        "64:E8:33:8C:04:A6",
        "64:E8:33:8A:7C:BE",
        "DC:06:75:E9:6F:92",
        "DC:06:75:E9:3C:02"
};

const char* BLE_SERVER_NAMES[] = {
        "AC",
        "PARENTS_ROOM_DUMPER",  
        "WORKING_ROOM_DUMPER",
        "SAFE_ROOM_DUMPER",
        "Test1",
        "Test2"
};

// IR Remote Control
#define IR_RECEIVER_PIN 2
#define IR_LED_PIN  4
IRRemoteControl irRemote(IR_RECEIVER_PIN, IR_LED_PIN, IRRemoteControl::TRANSMITTER_ONLY);

// Voltage Sensor
#define ANALOG_PIN 1
#define VOLTAGE_DIVIDER_RATIO 2.0 // Voltage divider ratio (R1 = 0.1M, R2 = 0.1M)
float getVoltage() {
    int adcValue = analogReadMilliVolts(ANALOG_PIN);
    return adcValue * 0.001 * VOLTAGE_DIVIDER_RATIO ;
}

// Deep Sleep Manager
#define WAKEUP_TIME_SECONDS 1
ESP32C3DeepSleep deepSleepManager(WAKEUP_TIME_SECONDS);

// Main .cpp functions
/**
 * @brief Get the index of the MAC address in the allowedMACs array.
 * @param macAddress The MAC address to search for.
 * @return The index of the MAC address in the allowedMACs array, or -1 if not found.
 */
int getMACIndex(const String& macAddress)
    {

        const int numAllowedMACs = sizeof(allowedMACs) / sizeof(allowedMACs[0]);
        for (int i = 0; i < numAllowedMACs; i++) {
            if (macAddress.equalsIgnoreCase(allowedMACs[i]))    return i; // Return the index if found
            
        }
        return -1; // Return -1 if not found
    }
/** 
 * @brief Get the BLE MAC address and check if it is in the allowed list.
 * @return void
 */
void get_ble_mac()
{
    // Get BLE MAC Address
    esp_bd_addr_t mac;
    esp_read_mac(mac, ESP_MAC_BT);

    // Convert MAC to String
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    Serial.print("BLE MAC Address: ");
    Serial.println(macStr);

    // Get the index of the MAC in the list
    int index = getMACIndex(String(macStr));

    if (index != -1) {
        Serial.print("MAC Address found at index: ");
        Serial.println(index);
    } else
        Serial.println("MAC Address NOT found in the list.");

    savedState.id = index;
}
/** 
 * @brief print the current state to the Serial monitor and check if the voltage is less than 3.3V.
 * @return void
 */
void adc_loop()
{
    savedState.voltage = getVoltage();
    
    // Print the current state;
   Serial.print(savedState.id);
   Serial.print(" ");
   Serial.print(savedState.voltage);
   Serial.print(" ");
   Serial.print(savedState.powerState);
   Serial.print(" ");
   Serial.print(savedState.mode);
   Serial.print(" ");
   Serial.print(savedState.fanSpeed);      
   Serial.print(" ");
   Serial.println(savedState.temperature);  

//    if voltage is less than 3.3V, then deep sleep
   if(savedState.voltage < 3.3 )
   {
       Serial.println("Voltage is less than 3.3V, entering deep sleep...");
       delay(100); // Ensure message is sent before sleeping
//       deepSleepManager.enterDeepSleep();
   }
}

#endif