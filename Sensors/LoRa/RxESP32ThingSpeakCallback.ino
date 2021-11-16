#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

// LoRa Ra-02 pins
#define ss 15 // SPI NSS GPIO15 
#define rst 4 // RESET GPIO 4
#define dio0 0 // DIO0 GPIO0
#define LEDpin 2
// MISO GPIO19
// MOSI GPIO23
// SPI-CLK GPIO18

// In order to check for possible errors during transmission, the identifing letters differs for more than 2 bits in their ASCII representation
const char BEEHIVES_ID[10]={'A','F','K','P','U','c','h','m','r','w'};
const int num_beehives=10;
const char SENSORS_ID[8]={'A','F','K','P','c','h','m','r'};
const int num_sensors=8;
// This can be expanded just using strings or doubling the packet header representing the beehive (in this case we will arrive up to 100 beehives)


// Thingspeak
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber[10] = {SECRET_CH_ID1,SECRET_CH_ID2,SECRET_CH_ID3,SECRET_CH_ID4,SECRET_CH_ID5,SECRET_CH_ID6,SECRET_CH_ID7,SECRET_CH_ID8,SECRET_CH_ID9,SECRET_CH_ID10};
// BRUTTO Configuration file from which we can add several channels ....
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;



void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver Callback and Transmitter to Thingspeak");

  pinMode(LEDpin,OUTPUT);
  
  digitalWrite(LEDpin, HIGH);
  // Initialize LoRa
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  // WIFI_STA means than ESP32 connect to the WiFi and not vice versa (WIFI_AP)
  WiFi.mode(WIFI_STA);
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  digitalWrite(LEDpin,LOW);
  
  delay(3000);
  
  digitalWrite(LEDpin,HIGH);
  // Connect to WiFi
  if(WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    } 
    Serial.println("\nConnected.");
  }
    digitalWrite(LEDpin,LOW);

  // Uncomment the next line to disable the default AGC (Automatic Gain Control) and set LNA gain, values between 1 - 6 are supported
  // LoRa.setGain(6);
  
  // Register the receive callback, look at library LoRa.cpp for LoRa.onReceive (works with attachInterrupt)
  // And wants the pointer to a callback
  LoRa.onReceive(onReceive);

  // Put the radio into receive mode
  LoRa.receive();
}



void loop() {
}


// It's optimal, in order to avoid data loss due to packets conficts, to send packets made this way:
// - As small as possible (few bytes)
// - Low frequency (once every 10 minutes is better than every 5 minutes)


void onReceive(int packetSize) {
  Serial.print("Received packet '");
  
  char packet[20];
  bool beehive_found=false;
  bool sensor_found=false;
  unsigned long BeehiveChannelNumber;

  // read packet FIFO
  for (int i = 0; i < packetSize; i++) {
    packet[i]=(char)LoRa.read();
    Serial.print(packet[i]); // Debug
  }
  packet[packetSize]='\0';
  Serial.println("");
  Serial.println(packet);

  // RSSI=Received Signal Strenght Indicator=received signal power in milliwatts and is measured in dBm
  // -30dBm strong signal, -120dBm weak signal
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());

  if(WiFi.status() != WL_CONNECTED){
    // BRUTTO mettere LEDpin HIGH e vedere se funziona ancora correttamente
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    } 
    Serial.println("\nConnected.");
  }


  // Beehive check
  for (int i = 0; i < num_beehives; i++) {
    if (packet[0] == BEEHIVES_ID[i]) {
      BeehiveChannelNumber=myChannelNumber[i];
      beehive_found=true;
    } 
  }
  if (beehive_found == false) {
      Serial.print("Packet header: beehive unknown! "); // Debug
      Serial.println(packet[0]);
      return;
  }

  // BRUTTO if you want to modify and make a larger packet, you have to put to zero or not consider the wrong field (remove all the "return")

  // Sensor check
  for (int i = 0; i < num_sensors; i++) {
    if (packet[1] == SENSORS_ID[i]) {
      // Number check
      if (!((packet[2] >= 48 && packet[2] <= 57) || packet[2] == 43 || packet[2] == 45)) {
        return;
      }
      for (int j = 3; j < packetSize; j++) {
        if (!(packet[j] >= 48 && packet[j] <= 57)) {
          return;
        }
      }
      ThingSpeak.setField(i, (float)(atoi(packet+2)/100.0));
      sensor_found=true;
    }
  }
  if (sensor_found == false) {
      Serial.print("Packet header: sensor unknown! "); // Debug
      Serial.println(packet[1]);
      return;
  }

/*
  if(number1 > number2){
    myStatus = String("field1 is greater than field2"); 
  }
  else if(number1 < number2){
    myStatus = String("field1 is less than field2");
  }
  else{
    myStatus = String("field1 equals field2");
  }
  
  ThingSpeak.setStatus(myStatus);
*/

  // Write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(BeehiveChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}
