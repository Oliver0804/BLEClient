/*
基於藍牙裝置進行數據解析
*/

#include "BLEDevice.h"

#define BATT_SERVICE_UUID   "180F"
#define BATT_LEVEL_UUID     "2A19"

#define CONFIG_IMU_SERVICE_UUID "FF00"
#define CONFIG_IMU_AG     "FF10"  // 0x00 => Start 6-axis acc,gyro,0xFF=>off

#define NOTIFY_IMU_SERVICE_UUID     "1600"  // print imu data
#define NOTIFY_IMU_UUID     "1601"  // print imu data

#define LBS_SERVICE_UUID    "FFC0"
#define NOTIFY_BTN_UUID     "FFC3" //print button state

BLEAdvertData foundDevice;
BLEAdvertData targetDevice;

BLEClient* client;

//batt Servic
BLERemoteService* battService;
BLERemoteCharacteristic* battChar;

//lbs Servic
BLERemoteService* lbsService;
BLERemoteCharacteristic* lbsChar;

//imu setting Servic
BLERemoteService* imuSettingService;
BLERemoteCharacteristic* imuSettingChar;
//BLECharacteristic imuSetChar(CONFIG_IMU_AG);

//imu Servic
BLERemoteService* imuService;
BLERemoteCharacteristic* imuChar;
bool notifyState = false;
int8_t connID;


void scanCB(T_LE_CB_DATA* p_data) {
  foundDevice.parseScanInfo(p_data);
  if (foundDevice.hasName()) {
    if (foundDevice.getName() == String("Lapita_3280")) {//device name
      Serial.print("Found Ameba BLE Device at address ");
      Serial.println(foundDevice.getAddr().str());
      targetDevice = foundDevice;
    }
  }
  uint8_t serviceCount = foundDevice.getServiceCount();
  if (serviceCount > 0) {
    BLEUUID* serviceList = foundDevice.getServiceList();
    for (uint8_t i = 0; i < serviceCount; i++) {
      if (serviceList[i] == BLEUUID(BATT_SERVICE_UUID)) {
        Serial.print("Found Battery Service at address ");
        Serial.println(foundDevice.getAddr().str());

      }
      if (serviceList[i] == BLEUUID(LBS_SERVICE_UUID)) {
        Serial.print("Found LBS Service at address ");
        Serial.println(foundDevice.getAddr().str());

      }
    }
  }
}

void notification_battCB (BLERemoteCharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.print("Notification received for chr UUID: ");
  Serial.print(chr->getUUID().str());
  if (len == 1) {
    Serial.print(" Battery level: ");
    Serial.println(data[0]);
  }
}

void notification_lbsCB (BLERemoteCharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.print("Notification received for chr UUID: ");
  Serial.print(chr->getUUID().str());
  if (len == 1) {
    Serial.print("Notification lbs: ");
    Serial.println(data[0]);
  }
}

//Acc x(2byte),y(2byte),z(2byte), gyro x(2byte), y(2byte), z(2byte),RTC(4byte) => total 16byte.
void notification_imuCB (BLERemoteCharacteristic* chr, uint8_t* data, uint16_t len) {
  //Serial.println(len); // Print the length of the data
  //Serial.print("imu: ");
  //Serial.print(chr->getUUID().str()); // Print the UUID of the characteristic
  
  // Replace the if condition with a for loop to handle any length of data
  for (int i = 0; i < len; i++) {
    //Serial.print("imu: ");
    Serial.print(data[i]); // Print each byte of the data
    if (i < len - 1) {
      Serial.print(", "); // Print a comma after each byte except the last one
    }
  }
  Serial.println(); // Print a newline after all data bytes have been printed
}
void setup() {
  Serial.begin(115200);

  BLE.init();
  BLE.configScan()->setScanMode(GAP_SCAN_MODE_ACTIVE);
  BLE.configScan()->setScanInterval(500);   // Start a scan every 500ms
  BLE.configScan()->setScanWindow(250);     // Each scan lasts for 250ms
  BLE.setScanCallback(scanCB);
  BLE.beginCentral(1);

  BLE.configScan()->startScan(2000);    // Scan for 2 seconds, then stop
  BLE.configConnection()->connect(targetDevice, 2000);
  delay(2000);
  connID = BLE.configConnection()->getConnId(targetDevice);

  if (!BLE.connected(connID)) {
    Serial.println("BLE not connected");
  } else {
    BLE.configClient();
    client = BLE.addClient(connID);
    client->discoverServices();
    Serial.print("Discovering services of connected device");
    do {
      Serial.print(".");
      delay(1000);
    } while (!(client->discoveryDone()));
    Serial.println();

    //imu setting
    imuSettingService = client->getService(CONFIG_IMU_SERVICE_UUID);
    if (imuSettingService != nullptr) {
      imuSettingChar = imuSettingService->getCharacteristic(CONFIG_IMU_AG);
      if (imuSettingChar != nullptr) {
        //write 0x00 turn on imu        imuSetChar.writeData8(0);
        Serial.print("imuSetChar1:");
        Serial.println(imuSettingChar->readData8());
        imuSettingChar->writeData8(0);
        Serial.print("imuSetChar2:");
        Serial.println(imuSettingChar->readData8());
      }
    }

    //batt notify
    battService = client->getService(BATT_SERVICE_UUID);
    if (battService != nullptr) {
      battChar = battService->getCharacteristic(BATT_LEVEL_UUID);
      if (battChar != nullptr) {
        Serial.println("Battery level characteristic found");
        battChar->setNotifyCallback(notification_battCB);
      }
    }
    Serial.println("batt Notifications Enabled");
    battChar->enableNotifyIndicate();

    //lbs notify
    lbsService = client->getService(LBS_SERVICE_UUID);
    if (lbsService != nullptr) {
      lbsChar = lbsService->getCharacteristic(NOTIFY_BTN_UUID);
      if (lbsChar != nullptr) {
        Serial.println("lbs characteristic found");
        lbsChar->setNotifyCallback(notification_lbsCB);
      }
    }
    Serial.println("lbs Notifications Enabled");
    lbsChar->enableNotifyIndicate();

    //imu notify
    imuService = client->getService(NOTIFY_IMU_SERVICE_UUID);
    if (imuService != nullptr) {
      imuChar = imuService->getCharacteristic(NOTIFY_IMU_UUID);
      if (imuChar != nullptr) {
        Serial.println("imu characteristic found");
        imuChar->setNotifyCallback(notification_imuCB);
      }
    }
    Serial.println("imu Notifications Enabled");
    imuChar->enableNotifyIndicate();
  }
}
int x =0;//測試用
void loop() {
  if (BLE.connected(connID)) {
    Serial.print("Battery level read: ");
    Serial.println(lbsChar->readData8());

    delay(1000);
    if(x>4)x=0;//四種模式循環,0x00的時候是六軸資訊
  
    Serial.print("write imu set: ");
    imuSettingChar->writeData8(x++);
    Serial.print("read imu set: ");
    Serial.println(imuSettingChar->readData8());
    
  } else {
    Serial.println("BLE not connected");
    delay(5000);
  }
}
