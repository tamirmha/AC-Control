#include <Arduino.h>
//#include "decode_packet.h"
#include "BLEServerManager.h"
#include "global_var.h"

#define SERIAL_PRINT true

// Store the structure in RTC memory
RTC_DATA_ATTR DeviceState savedState;

unsigned long previousMillis = 0; // Store the last time the loop ran
const long interval = 1000; // Interval at which to run the loop (500 milliseconds)


BLEServerManager bleManager;

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
   if(savedState.voltage < 3.3)
   {
       Serial.println("Voltage is less than 3.3V, entering deep sleep...");
       delay(100); // Ensure message is sent before sleeping
       deepSleepManager.enterDeepSleep();
   }
}


int getMACIndex(const String& macAddress) {

    const int numAllowedMACs = sizeof(allowedMACs) / sizeof(allowedMACs[0]);

    for (int i = 0; i < numAllowedMACs; i++) {
        if (macAddress.equalsIgnoreCase(allowedMACs[i])) {
            return i; // Return the index if found
        }
    }
    return -1; // Return -1 if not found
}


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
//    #if (SERIAL_PRINT == true)
//        serial_listener();
//    #endif
    adc_loop();

    previousMillis = millis();
    while (millis() - previousMillis < interval) 
    {
      delay(10);
    }
     deepSleepManager.enterDeepSleep();
}


// #include <IRremote.hpp>
// #include "electraAcRemoteEncoder.h" // Ensure this header defines the necessary encoding functions

// const uint16_t kIrLedPin = 4; // IR LED connected to GPIO 4

// IRsend irsend(kIrLedPin);

// void setup() {
//     Serial.begin(115200);
//     Serial.println("Start");
//     IrSender.begin(kIrLedPin, false, 0);  // Initialize IR sending on GPIO 4
//     IrSender.enableIROut(38); // Set IR frequency to 38 kHz
//     Serial.println("IR Sender Initialized");
// }

// void loop() {
//   // Define the desired AC state
//   struct airCon acState;  // Declare acState
//   Serial.println("airCon");

//   int fanSpeed = FAN_MASK;   // Adjust as needed
//   int mode = COOL;           // Set mode (COOL, HEAT, etc.)
//   int temperature = 24;      // Set temperature (15-30°C)
//   int powerState = ON;       // ON or OFF
//   int swingState = SWING_OFF; // SWING_ON, SWING_OFF, SWING_SINGLE
//   Serial.println("getCodes");
//   // Generate Manchester-encoded timings
//   int* irSignalInt = getCodes(&acState, fanSpeed, mode, temperature, powerState, swingState);
//   Serial.println("TIMINGS_LENGTH");
//   // Convert int* to uint16_t*
//   uint16_t irSignal[TIMINGS_LENGTH];
//   for (int i = 0; i < TIMINGS_LENGTH; i++) {
//       irSignal[i] = static_cast<uint16_t>(irSignalInt[i]);
//   }
//   Serial.println("sendRaw");
//   // Send raw IR signal
//   irsend.sendRaw(irSignal, TIMINGS_LENGTH, 38);
//   Serial.println("IR Command Sent");

//   delay(5000);  // Wait 5 seconds before next send
// }
