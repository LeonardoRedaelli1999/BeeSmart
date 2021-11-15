#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

// LoRa Ra-02 pins
#define ss 15
#define rst 4
#define dio0 3
#define LEDpin 2

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

char packets[20];

// Initialization of received data
int T1 = 0;
int T2 = 0;
int T3 = 0;
int T4 = 0;
int T5 = 0;
int humidity = 0;
int PIR1 = 0;
int PIR2 = 0;
String myStatus = "";

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  pinMode(LEDpin,OUTPUT);
  
  digitalWrite(LEDpin, HIGH);
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
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    } 
    Serial.println("\nConnected.");
  }
  digitalWrite(LEDpin,LOW);
  
  // BRUTTO setModemSleep() from MISCHIANTI
}

void loop() {

  // LoRa.parsePacket return the packet size in bytes or 0 if no packet was received
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    // LoRa.available returns number of bytes available for reading, every time we read the packet memorized in LoRa lose a byte
    // So LoRa.available begin being packetSize, then after we read one byte becomes packetSize-1, etc...
    while (LoRa.available()) {
      packet(i)=(char)LoRa.read();
    }
    packet(packetSize)='\0';
    
    // Print packet in order to debug
    Serial.print(packet);

    // RSSI=Received Signal Strenght Indicator=received signal power in milliwatts and is measured in dBm
    // -30dBm strong signal, -120dBm weak signal
    // Print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    
    // BRUTTO setStatus of ThingSpeak to acknowledge about the power of the connection

  
    // Reconnect to WiFi if necessary
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(SECRET_SSID);
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        Serial.print(".");
        delay(5000);
      } 
      Serial.println("\nConnected.");
    }
  
      
    // ????????????????????????????????????????????????????????????????????????????????????????????
    if (packet(0)=='a'){
      T1=atoi(packet(1),10); // Character to Integer (base 10)
      strcat(str_rf,"a"); //Add identifier character
      rfWrite(str_rf);                         //Send the string
    }
    
    
    // BRUTTO our FLOAT values
    // set the fields with the values
    ThingSpeak.setField(1, number1);
    ThingSpeak.setField(2, number2);
    ThingSpeak.setField(3, number3);
    ThingSpeak.setField(4, number4);
  
    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

  }
}
