#ifndef _deep_sleep_h
#define _deep_sleep_h

#include <esp_sleep.h>
#include <Arduino.h>


class ESP32C3DeepSleep {
public:
    // Constructor
    ESP32C3DeepSleep(uint32_t wake_seconds = 1 )
    {
        printWakeupCause();
        enableTimerWakeup(wake_seconds); // Wake up after wake_seconds seconds
    }

    // Enable wake-up by timer (in seconds)
    void enableTimerWakeup(uint32_t seconds) {
        esp_sleep_enable_timer_wakeup(seconds * 1000000ULL);
    }

    // Enter deep sleep
    void enterDeepSleep() {
        Serial.println("Entering deep sleep...");
        delay(100); // Ensure message is sent before sleeping
        esp_deep_sleep_start();
    }

    // Get wake-up cause
    esp_sleep_wakeup_cause_t getWakeupCause() {
        return esp_sleep_get_wakeup_cause();
    }

    // Print wake-up cause
    void printWakeupCause() {
        esp_sleep_wakeup_cause_t cause = getWakeupCause();
        Serial.print("Wake-up cause: ");
        switch (cause) {
            case ESP_SLEEP_WAKEUP_TIMER:
                Serial.println("Timer");
                break;
            case ESP_SLEEP_WAKEUP_EXT0:
                Serial.println("External GPIO");
                break;
            case ESP_SLEEP_WAKEUP_UNDEFINED:
            default:
                Serial.println("Power-on or Reset");
                break;
        }
    }
};


/*
void setup() {
    Serial.begin(115200);
    delay(1000);
    // Example usage
    ESP32C3DeepSleep deepSleepManager;


    deepSleepManager.enterDeepSleep();
}

void loop() {
    // This will never run after deep sleep, since setup() restarts
}
*/
#endif