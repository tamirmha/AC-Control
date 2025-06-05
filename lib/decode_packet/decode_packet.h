#ifndef _decode_packet_h
#define _decode_packet_h
#include "global_var.h"

/**
 * @brief Function to decode incoming data from any sender (uart / server / whatever)
 * @param packet the packet that needs to be decoded
 * @param sender sender of the packet
 * @return void
 */
void decode_packet(String packet)
{
    // Serial.println(packet);

    int spaceIndex = packet.indexOf(' ');
    String cmd = spaceIndex == -1 ? packet : packet.substring(0, spaceIndex);
    String value = spaceIndex == -1 ? "" : packet.substring(spaceIndex + 1);
    cmd.toLowerCase();
    IRAirwellAc* ac = irRemote.getAc(); // Get the IRAirwellAc object
    // For AC
    if (cmd == "temp") {
        int temp = value.toInt();
        if (temp >= 16 && temp <= 30) 
        {
            ac->setTemp(temp);
            Serial.println("Temperature set to: " + String(temp));
        }
    }
    else if (cmd == "mode") {
        value.toLowerCase();
        if (value == "cool") ac->setMode(kAirwellCool);
        else if (value == "heat") ac->setMode(kAirwellHeat);
        else if (value == "dry") ac->setMode(kAirwellDry);
        else if (value == "fan") ac->setMode(kAirwellFan);
        Serial.println("Mode set to: " + value);
    }
    else if (cmd == "fan") {
        value.toLowerCase();
        if (value == "auto") ac->setFan(kAirwellFanAuto);
        else if (value == "high") ac->setFan(kAirwellFanHigh);
        else if (value == "med") ac->setFan(kAirwellFanMedium);
        else if (value == "low") ac->setFan(kAirwellFanLow);
        Serial.println("Fan set to: " + value);
    }
    else if (cmd == "power") {
        value.toLowerCase();
        ac->setPowerToggle(value == "on" || value == "1");
        Serial.println("Power set to: " + value);
    }
    else if (cmd == "send") {
        ac->send(10);
        Serial.println("Current settings:");
        Serial.println(ac->toString());
        Serial.println("Command sent!");
        uint64_t state = ac->getRaw();
        Serial.printf("Code: 0x%016llX\n", state);
    }
    else if (cmd == "status")   Serial.println(ac->toString()); // Show current settings of the AC
    // For dumpers
    else if (cmd == "n") irRemote.turn_on();
    else if (cmd == "f") irRemote.turn_off();
    // General
    else if (cmd == "d") deepSleepManager.enterDeepSleep();
    else if (cmd == "help") {
        Serial.println("Available commands for AC:");
        Serial.println("temp [16-30] - Set temperature");
        Serial.println("mode [cool/heat/dry/fan] - Set mode");
        Serial.println("fan [auto/high/med/low] - Set fan speed");
        Serial.println("power [on/off] - Set power state");
        Serial.println("send - Send current settings");
        Serial.println("status - Show current settings");
        Serial.println("Available commands for Dumper:");
        Serial.println("n - Turn on Dumper");
        Serial.println("f - Turn off Dumper");
        Serial.println("General commands:");
        Serial.println("d - Enter deep sleep mode");
        Serial.println("help - Show this help message");
    }
    else    Serial.printf("Unknown command: %s\n", packet.c_str());  // Return what it get    
}

void serial_listener()
{
  if(Serial.available())
  {
    String st = Serial.readStringUntil('\n');
    st.trim();
    decode_packet(st);
  }
}
#endif