#include "decode_packet.h"
#include "global_var.h"

#define SERIAL_PRINT false

unsigned long previousMillis = 0; // Store the last time the loop ran
const long interval = 2000; // Interval at which to run the loop (3000 milliseconds)


void setup() {
    #if (SERIAL_PRINT == true)
        Serial.begin(115200);
        while (!Serial); 
        Serial.println("Start");
        delay(1000);
    #endif
    get_ble_mac();
    bleManager.begin(BLE_SERVER_NAMES[savedState.id]);
    irRemote.begin();            // Initialize IR Remote Control 

    // Initialize ADC
    analogReadResolution(12); // Set ADC resolution (ESP32 default is 12-bit)
    analogSetAttenuation(ADC_11db); // Set range to ~0-3.3V   
}

/**
 * @brief Handles incoming commands and controls devices based on the saved state.
 *
 * This function checks the `savedState` structure to determine which device the command is for.
 * - If the device ID is 0, it logs that a new message for the AC has been received.
 * - If the device ID is greater than 0 (indicating a non-AC device, e.g., a dumper):
 *   - If the power state is ON, it logs that the dumper is on and sends a turn-on command via IR.
 *   - If the power state is OFF, it logs that the dumper is off and sends a turn-off command via IR.
 */
void handle_command()
{
    if (savedState.id == 0)
    {
            IRAirwellAc* ac = irRemote.getAc(); // Get the IRAirwellAc object
            
            ac->setPowerToggle(savedState.powerState != savedState.prevPowerState); // Set power state
            ac->setMode(savedState.mode); // Set mode
            ac->setFan(savedState.fanSpeed); // Set fan speed
            ac->setTemp(savedState.temperature); // Set temperature
            ac->send(10); // Send the command to the AC    
    #if (SERIAL_PRINT == true)
            Serial.println("Current settings:");
            Serial.println(ac->toString());
            uint64_t state = ac->getRaw();
            Serial.printf("Code: 0x%016llX\n", state);
    #endif
    } 
    else if (savedState.id > 0) // Check if the device is not the AC
    {
        Serial.printf("Dumper is %s", savedState.powerState == ON ? "On" : "Off");
        if (savedState.powerState == ON) irRemote.turn_on(); // Check if the Dumber command is ON
        else irRemote.turn_off();
    }
}

void loop() {
    bool new_msg = false;
    previousMillis = millis();
    while (millis() - previousMillis < interval) 
    {
        new_msg = bleManager.loop();
        if (new_msg)  handle_command(); // Handle new message from BLE
        // Check for received IR signals
        #if (SERIAL_PRINT == true)
            serial_listener();
        #endif
            adc_loop();
    }
    deepSleepManager.enterDeepSleep();
}
