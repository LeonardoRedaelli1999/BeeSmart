// Looks for 1-wire devices and prints their addresses (serial number)
#include <OneWire.h>

OneWire  ds(2);  // Connect your 1-wire device to pin 2

void setup(void) {
  Serial.begin(9600);
  discoverOneWireDevices();
}

void discoverOneWireDevices(void) {
  uint8_t i;
  uint8_t present = 0;
  uint8_t data[12];
  uint8_t addr[8];
  
  Serial.print("Looking for 1-Wire devices...\n\r");
  while(ds.search(addr)) {
    Serial.print("\n\rFound \'1-Wire\' device with address:\n\r");
    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!\n");
        return;
    }
  }
  Serial.print("\n\r\n\rThat's it.\r\n");
  ds.reset_search();
  return;
}

void loop(void) {
}
