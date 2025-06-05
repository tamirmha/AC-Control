#ifndef IR_REMOTE_CONTROL_H
#define IR_REMOTE_CONTROL_H


#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <ir_Airwell.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

// #include <IRremote.hpp>



/**
 * @brief Class to handle IR remote control functionality.
 * 
 * This class encapsulates the functionality for the IR receiver and sender,
 * as well as predefined IR signals. It supports operating as a receiver only,
 * transmitter only, or both.
 */
class IRRemoteControl {
public:

    /**
     * @brief Enumeration for the mode of operation.
     */
    enum Mode {
        RECEIVER_ONLY,
        TRANSMITTER_ONLY,
        BOTH
    };

    /**
     * @brief Constructor for the IRRemoteControl class.
     * 
     * @param receiverPin GPIO pin for the IR receiver.
     * @param senderPin GPIO pin for the IR sender.
     * @param mode Mode of operation (RECEIVER_ONLY, TRANSMITTER_ONLY, BOTH).
     */
    IRRemoteControl(uint8_t receiverPin, uint8_t senderPin, Mode mode)
        : receiverPin(receiverPin), senderPin(senderPin), mode(mode) {}

    /**
     * @brief Initializes the IR receiver and/or sender based on the mode.
     */
    void begin() {
        if (mode == RECEIVER_ONLY || mode == BOTH) {
            
            // IrReceiver.begin(receiverPin, ENABLE_LED_FEEDBACK);
            IrReceiver = new IRrecv(receiverPin);
            IrReceiver->enableIRIn();
            Serial.println("IR Receiver Ready");
        }
        if (mode == TRANSMITTER_ONLY || mode == BOTH) {
            // IrSender.begin(senderPin, false, 0);
            IrSender = new IRsend(senderPin);  // Set the GPIO to be used to sending the message.

            IrSender->enableIROut(38); // 38 kHz carrier frequency
            IrSender->begin();
            Serial.println("IR Transmitter Ready");

            ac = new IRAirwellAc(senderPin);  // Initialize the IRAirwellAc object with the sender pin
            ac->begin();  // Begin the Airwell AC protocol
        }
    }

    /**
     * @brief Sends an IR signal.
     * 
     * @param signal Pointer to the array containing the IR signal data.
     * @param length Length of the IR signal data array.
     */
    void sendIRSignal(const uint16_t* signal, size_t length) {
        if (mode == TRANSMITTER_ONLY || mode == BOTH) {
            if (signal == nullptr || length == 0) {
                Serial.println("Error: Invalid signal data");
                return;
            }
            IrSender->sendRaw(signal, length, 38); // 38kHz carrier frequency
            Serial.println("IR Command Sent!");
        } else 
            Serial.println("Error: Transmitter not enabled");
    }

    /**
     * @brief Checks for received IR signals and prints them to the Serial monitor.
     */
    void checkForReceivedSignal() {
        if (mode == RECEIVER_ONLY || mode == BOTH) {
            decode_results results;
            if (IrReceiver->decode(&results)) {
                // IrReceiver.printIRResultRawFormatted(&Serial, true);
                serialPrintUint64(results.value, HEX);
                IrReceiver->resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
                Serial.println();
            }
        } else 
            Serial.println("Error: Receiver not enabled");
    }


    void turn_on() {
        Serial.println("Sending Power On Button command...");
        for(int i = 0; i < 10; i++) {
            sendIRSignal(getPowerOnSignal(), getPowerOnSignalLength());
            delay(100);  // Wait 5 seconds before sending again
        }
    }

    void turn_off() {
        Serial.println("Sending Power Off Button command...");
        for(int i = 0; i < 10; i++) {
            sendIRSignal(getPowerOffSignal(), getPowerOffSignalLength());
            delay(100);  
        }
    }

    IRAirwellAc* getAc() { return ac; }

    // Getter functions for predefined IR signals
    const uint16_t* getTempPlusSignal() const { return temp_plus_signal; }
    size_t getTempPlusSignalLength() const { return sizeof(temp_plus_signal) / sizeof(temp_plus_signal[0]); }

    const uint16_t* getPowerOffSignal() const { return power_off_signal; }
    size_t getPowerOffSignalLength() const { return sizeof(power_off_signal) / sizeof(power_off_signal[0]); }

    const uint16_t* getPowerOnSignal() const { return power_on_signal; }
    size_t getPowerOnSignalLength() const { return sizeof(power_on_signal) / sizeof(power_on_signal[0]); }

    const uint16_t* getTempDownSignal() const { return temp_down_signal; }
    size_t getTempDownSignalLength() const { return sizeof(temp_down_signal) / sizeof(temp_down_signal[0]); }

    const uint16_t* getDumperButtonSignal() const { return dumper_button_signal; }
    size_t getDumperButtonSignalLength() const { return sizeof(dumper_button_signal) / sizeof(dumper_button_signal[0]); }

private:
    uint8_t receiverPin;
    uint8_t senderPin;
    Mode mode;
    IRrecv* IrReceiver;
    IRsend* IrSender;
    IRAirwellAc* ac;  

    // Predefined IR signals
    const uint16_t power_off_signal[104] = {
        4000, 4000,  // Header
        550, 550, 550, 550, 550, 550, 550, 550,
        550, 1300, 550, 500, 550, 1300, 550, 1300,
        550, 1300, 550, 1250, 550, 550, 550, 550,
        550, 1300, 550, 1300, 500, 550, 550, 1300,
        3950, 4000, 550, 550, 550, 550, 550, 550,
        550, 1250, 550, 550, 550, 1300, 550, 1300,
        550, 1250, 550, 1300, 550, 550, 550, 550,
        550, 1250, 600, 1250, 550, 550, 550, 1300,
        3900, 4050, 550, 550, 550, 550, 550, 550,
        550, 1250, 550, 550, 550, 1300, 550, 1300,
        550, 1250, 550, 1300, 550, 550, 550, 550,
        550, 1250, 600, 1250, 550, 550, 550, 1300,
        7900  // Footer
    };

    const uint16_t power_on_signal[104] = {
        4000, 4000,  // Header
        550, 550, 500, 600, 500, 600, 500, 600,
        500, 1350, 500, 550, 550, 1350, 500, 1300,
        500, 1350, 500, 1350, 500, 550, 550, 550,
        500, 1350, 500, 1350, 500, 1350, 500, 550,
        3950, 4000, 500, 600, 500, 600, 500, 600,
        500, 600, 500, 1350, 500, 550, 550, 1300,
        500, 1350, 500, 1350, 500, 1350, 500, 550,
        500, 600, 500, 1350, 500, 1350, 500, 1300,
        500, 600, 3950, 4000, 500, 600, 500, 600,
        500, 600, 500, 600, 500, 1350, 500, 550,
        550, 1300, 500, 1350, 500, 1300, 550, 1300,
        500, 600, 500, 600, 500, 1350, 500, 1300,
        550, 1300, 500, 600, 7850  // Footer
    };

    const uint16_t temp_plus_signal[104] = {
        4000, 4000,  // Header
        550, 550, 550, 550, 550, 550, 550, 1300,
        550, 500, 600, 1300, 500, 1300, 550, 1300,
        550, 500, 600, 500, 550, 550, 550, 550,
        550, 1300, 550, 1300, 550, 1300, 500, 1300,
        3900, 4000, 550, 550, 550, 550, 550, 550,
        550, 1250, 600, 500, 550, 1300, 550, 1300,
        550, 1250, 600, 500, 550, 550, 550, 550,
        550, 550, 550, 1300, 550, 1250, 600, 1250,
        550, 1300, 4100, 3950, 550, 550, 550, 550,
        550, 550, 550, 1250, 600, 500, 550, 1300,
        550, 1300, 550, 1250, 600, 500, 550, 550,
        550, 550, 550, 550, 550, 1300, 550, 1250,
        600, 1250, 550, 1300, 7850  // Footer
    };

    const uint16_t temp_down_signal[104] = {
        4000, 3950,  // Header
        600, 550, 550, 550, 550, 500, 550, 1300,
        550, 1300, 550, 550, 550, 1300, 500, 1350,
        500, 1300, 550, 550, 550, 550, 550, 550,
        550, 1300, 500, 1300, 550, 1300, 550, 500,
        3950, 4000, 600, 550, 500, 550, 550, 550,
        550, 1300, 550, 1300, 550, 550, 550, 1250,
        550, 1300, 550, 1300, 550, 550, 550, 550,
        550, 500, 600, 1250, 550, 1300, 550, 1250,
        600, 500, 3950, 4000, 600, 500, 550, 550,
        550, 550, 550, 1300, 550, 1300, 550, 550,
        550, 1250, 550, 1300, 550, 1300, 550, 500,
        600, 500, 600, 500, 550, 1300, 550, 1300,
        550, 1250, 600, 500, 7900  // Footer
    };

    const uint16_t dumper_button_signal[104] = {
        3950, 4000,  // Header
        550, 550, 550, 550, 550, 550, 550, 1300,
        550, 500, 550, 1350, 500, 550, 550, 550,
        550, 1300, 550, 550, 550, 500, 550, 550,
        550, 1350, 500, 1300, 550, 1300, 500, 550,
        4000, 4000, 550, 550, 550, 550, 550, 550,
        550, 1300, 550, 550, 550, 1300, 500, 550,
        550, 1300, 550, 550, 550, 550, 550, 1300,
        500, 1350, 500, 1300, 500, 600, 4000, 4000,
        500, 600, 500, 600, 500, 600, 500, 1350,
        500, 550, 550, 1300, 500, 600, 500, 600,
        500, 1350, 500, 550, 550, 550, 550, 1300,
        500, 1350, 500, 1350, 500, 550, 7900  // Footer
    };
};

#endif // IR_REMOTE_CONTROL_H