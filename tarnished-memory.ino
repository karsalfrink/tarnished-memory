#include <QList.h>

#define BLE_SCAN_TYPE        0x00   // Passive scanning
#define BLE_SCAN_INTERVAL    0x0060 // 60 ms
#define BLE_SCAN_WINDOW      0x0030 // 30 ms

QList<String> devices;
int deviceCount = 0;

void reportCallback(advertisementReport_t *report) {
  String peerAddrStr;
  
  for (int index = 0; index < 6; index++) {
    peerAddrStr.concat(String(report->peerAddr[index], HEX));
  }
  
  if (devices.indexOf(peerAddrStr) > 0) {
    // Skip adding a device
  } else {
    devices.push_front(peerAddrStr);
    deviceCount = devices.length();
  }
  Particle.publish("deviceCount", String(deviceCount));
}

void setup() {  
  ble.init();
  ble.onScanReportCallback(reportCallback);
  ble.setScanParams(BLE_SCAN_TYPE, BLE_SCAN_INTERVAL, BLE_SCAN_WINDOW);
  ble.startScanning();
}

void loop() {
}
