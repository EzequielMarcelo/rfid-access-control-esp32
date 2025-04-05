#ifndef BLEClientLibrary_H_
#define BLEClientLibrary_H_

#include "Arduino.h"
#include "BLEDevice.h"

#define SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb"
#define BUTTON_UUID "0000ffe1-0000-1000-8000-00805f9b34fb"

class BLEClientLibrary: public BLEAdvertisedDeviceCallbacks, public BLEClientCallbacks
{
    private:
        BLEClient *_client;
        BLEScan *_scan;
        BLEAdvertisedDevice *_selectedDevice = nullptr;
        static BLERemoteCharacteristic *buttonCharacteristic;
        static void (*_notificationCallback)(void);

        // Callbacks
        void onResult(BLEAdvertisedDevice advertisedDevice);
        void onConnect(BLEClient *client);
        void onDisconnect(BLEClient *client);
        static void buttonNotification(BLERemoteCharacteristic *characteristic, uint8_t *data,size_t length, bool flag);

    public:
        void begin();
        void StartScan();
        void Connect(BLEAdvertisedDevice *device); 
        void Disconnect();  
        bool isConnected();
        void SetNotificationCallBack(void(*notificationCallback)(void));
        BLEAdvertisedDevice *GetFoundDevice();    
};

#endif
