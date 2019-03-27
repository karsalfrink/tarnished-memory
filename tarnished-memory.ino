#include <SimpleMap.h>
#include <TM1637.h>

#define BLE_SCAN_TYPE        0x00   // Passive scanning
#define BLE_SCAN_INTERVAL    0x0060 // 60 ms
#define BLE_SCAN_WINDOW      0x0030 // 30 ms

#define CLK 4
#define DIO 5

TM1637 tm1637(CLK,DIO);

int deviceCount = 0;
int window = 60; // Seconds

SimpleMap<String, String> *deviceMap = new SimpleMap<String, String>([](String &a, String &b) -> int {
  if (a == b) return 0;      // a and b are equal
  else if (a > b) return 1;  // a is bigger than b
  else return -1;            // a is smaller than b
});

void reportCallback(advertisementReport_t *report) {
  String peerAddrStr;
  
  for (int index = 0; index < 6; index++) {
    peerAddrStr.concat(String(report->peerAddr[index], HEX));
  }
  
  String timestamp = String(Time.now());
  
  if (deviceMap->has(peerAddrStr)) {
    // Existing device
    deviceMap->put(peerAddrStr, timestamp); // Update the timestamp
  } else {
    // New device
    deviceMap->put(peerAddrStr, timestamp); // Add the device
    updateDeviceCount();
    Particle.publish("deviceCount", String(deviceCount), PRIVATE);
    dispNum(deviceCount);
  }

  cleanUpMap(window); // Remove any outdated devices
}

void cleanUpMap(int timeframe) {
    int now = Time.now();
    int expirationTime = now - timeframe;

    for (int i = 0; i < deviceMap->size(); i++) {
      int timestamp = atoi(deviceMap->getData(i));

      if (timestamp < expirationTime) {
        // Remove device
        deviceMap->remove(i);
        updateDeviceCount();
      } else {
        // Keep device, i.e. do nothing
      }
    }
}

void updateDeviceCount() {
  deviceCount = deviceMap->size();
}

void dispNum(unsigned int num) {
  int8_t TimeDisp[] = {0x01,0x02,0x03,0x04};

  if (num > 9999) num = 9999;

  TimeDisp[0] = num / 1000;
  TimeDisp[1] = num % 1000 / 100;
  TimeDisp[2] = num % 100 / 10;
  TimeDisp[3] = num % 10;

  tm1637.display(TimeDisp);
}

void setup() {  
  ble.init();
  ble.onScanReportCallback(reportCallback);
  ble.setScanParams(BLE_SCAN_TYPE, BLE_SCAN_INTERVAL, BLE_SCAN_WINDOW);
  ble.startScanning();

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL); // BRIGHT_TYPICAL = 2, BRIGHT_DARKEST = 0, BRIGHTEST = 7
  dispNum(random(9999));
  
  Serial.begin(9600);
}

void loop() {
}
