#include "BLEClientLibrary.h"

BLERemoteCharacteristic *BLEClientLibrary::buttonCharacteristic = nullptr;
void (*BLEClientLibrary::_notificationCallback)() = NULL;

void BLEClientLibrary::begin()
{
    BLEDevice::init("");
    _scan = BLEDevice::getScan();
    _scan->setAdvertisedDeviceCallbacks(this);
    _scan->setInterval(1349);
    _scan->setWindow(449);
    _scan->setActiveScan(true);

    
}
void BLEClientLibrary::StartScan()
{
    BLEScanResults foundDevices = _scan->start(1);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    _scan->clearResults();
}
void BLEClientLibrary::onResult(BLEAdvertisedDevice device)
{
    if(device.haveServiceUUID() && device.isAdvertisingService(BLEUUID(SERVICE_UUID)))
    {
        _scan->stop();
        Serial.println(device.getAddress().toString().c_str());
        _selectedDevice = new BLEAdvertisedDevice(device);
    }
}
void BLEClientLibrary::onConnect(BLEClient *client)
{
    Serial.println("Sucessful connection");
}
void BLEClientLibrary::onDisconnect(BLEClient *client)
{
    Serial.println("Disconnected");
    _selectedDevice = nullptr;
}
void BLEClientLibrary::buttonNotification(BLERemoteCharacteristic *characteristic, uint8_t *data, size_t length, bool flag)
{
    if (characteristic == buttonCharacteristic && *data == 1)
    {
        Serial.println("ButtonClick"); 
        if(_notificationCallback != NULL)
            _notificationCallback();                       
    }         
}
void BLEClientLibrary::Connect(BLEAdvertisedDevice *device)
{
    _client = BLEDevice::createClient();
    _client->setClientCallbacks(this);

    if (_client->connect(device))
    {      
        BLERemoteService *remoteService = _client->getService(SERVICE_UUID);

        if (remoteService == nullptr)
        {
            Serial.println("Unrecognized service");
            Disconnect();            
        } 
        
        buttonCharacteristic = remoteService->getCharacteristic(BLEUUID(BUTTON_UUID));
        buttonCharacteristic->registerForNotify(buttonNotification);       
    }
    else
    {
        Serial.println("Failed to connect");
        _selectedDevice = nullptr;
    }
}
void BLEClientLibrary::Disconnect() 
{
    _client->disconnect();
}
bool BLEClientLibrary::isConnected()
{
    if(_client)
        return _client->isConnected();
    
    return false;
}
BLEAdvertisedDevice *BLEClientLibrary::GetFoundDevice()
{
    return _selectedDevice;
}
void BLEClientLibrary::SetNotificationCallBack(void(*notificationCallback)(void))
{
    _notificationCallback = notificationCallback;
}
