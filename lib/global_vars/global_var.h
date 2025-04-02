#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H
#pragma once
#include "deep_sleep.h"
#include "Arduino.h" 
#include "ir.h"

const char* allowedMACs[] = {
        "9C:9E:6E:C1:0C:5E",  // AC
        "9C:9E:6E:C1:09:E2",
        "64:E8:33:8C:04:A6",
        "64:E8:33:8A:7C:BE"
};

const char* BLE_SERVER_NAMES[] = {
        "AC",
        "PARENTS_ROOM_DUMPER",  
        "WORKING_ROOM_DUMPER",
        "SAFE_ROOM_DUMPER"
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

#endif