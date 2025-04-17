#include <Arduino.h>
#include "decode_packet.h"
#include "global_var.h"

#define SERIAL_PRINT true

unsigned long previousMillis = 0; // Store the last time the loop ran
const long interval = 1000; // Interval at which to run the loop (1000 milliseconds)


void setup() {
    #if (SERIAL_PRINT == true)
        Serial.begin(115200);
        while (!Serial); 
        Serial.println("Start");
        delay(1000);
    #endif
    get_ble_mac();
    bleManager.begin(BLE_SERVER_NAMES[savedState.id]);

    // Initialize IR remote control
    irRemote.begin();
    // Initialize ADC
    analogReadResolution(12); // Set ADC resolution (ESP32 default is 12-bit)
    analogSetAttenuation(ADC_11db); // Set range to ~0-3.3V   
}

void loop() {
    bool new_msg =  bleManager.loop();
    if (new_msg) 
    {
        if (savedState.id > 0) // Check if the device is not the AC
        {
            if (savedState.powerState == ON) // Check if the Dumber command is ON
            {
                Serial.println("Dumnber is On");
                irRemote.turn_on();

            }
            else
            {
                Serial.println("Dumnber is Off");
                irRemote.turn_off();
            }
        }
    }
    // Check for received IR signals
   #if (SERIAL_PRINT == true)
       serial_listener();
   #endif
    adc_loop();

    previousMillis = millis();
    while (millis() - previousMillis < interval) 
    {
      delay(10);
    }
    // deepSleepManager.enterDeepSleep();
}
