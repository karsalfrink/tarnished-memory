#include <SimpleMap.h>

#define BLE_SCAN_TYPE        0x00   // Passive scanning
#define BLE_SCAN_INTERVAL    0x0060 // 60 ms
#define BLE_SCAN_WINDOW      0x0030 // 30 ms

int deviceCount = 0;

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
  
  if (deviceMap->has(peerAddrStr)) {
    // Skip adding a device
  } else {
    String timestamp = String(Time.now());
    deviceMap->put(peerAddrStr, timestamp);

    deviceCount = deviceMap->size();
    
    Particle.publish("deviceCount", String(deviceCount));
  }
}

void setup() {  
  ble.init();
  ble.onScanReportCallback(reportCallback);
  ble.setScanParams(BLE_SCAN_TYPE, BLE_SCAN_INTERVAL, BLE_SCAN_WINDOW);
  ble.startScanning();

  Serial.begin(9600);
}

void loop() {
}
