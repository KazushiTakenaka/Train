#ifndef ESP32BT_H
#define ESP32BT_H
#include <Arduino.h>
#include <string>
// Bluetooth用設定
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
// BluetoothSerial SerialBT_;

class EspBTSetUpClass{
public:
    EspBTSetUpClass() {
        Serial.println("test");
    }

    void getMacAddress();
    void mainUnitSetUp(uint8_t address[6]);
    void receptionSetUp();
};

#endif