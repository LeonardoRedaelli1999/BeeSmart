#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include "secretstest.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

// LoRa Ra-02 pins
#define ss 15 // SPI NSS GPIO15 
#define rst 4 // RESET GPIO4
#define dio0 2 // DIO0 GPIO0
#define LEDpin 25
// MISO GPIO19
// MOSI GPIO23
// SPI-CLK GPIO18

// In order to check for possible errors during transmission, the identifing letters differs for more than 2 bits in their ASCII representation
const char BEEHIVES_ID[10]={'A','F','K','P','U','c','h','m','r','w'};
const int num_beehives=10;
//const char SENSORS_ID[8]={'A','F','K','P','c','h','m','r'};
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
  
  // Initialize LoRa
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);/*{
      digitalWrite(LEDpin, HIGH);
      delay(500);
      digitalWrite(LEDpin, LOW);
      delay(500);
    }*/
  }
  // WIFI_STA means than ESP32 connect to the WiFi and not vice versa (WIFI_AP)
  WiFi.mode(WIFI_STA);
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  
  digitalWrite(LEDpin, HIGH);
  delay(5000);
  digitalWrite(LEDpin, LOW);
    
  // Connect to WiFi
  if(WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      digitalWrite(LEDpin, LOW);
      delay(2500);
      digitalWrite(LEDpin, HIGH);
      delay(2500);
    } 
    Serial.println("\nConnected.");
  }
  digitalWrite(LEDpin, LOW);
}

void loop() {
  char packet[26];
  bool beehive_found=false;
  bool sensor_found=false;
  unsigned long BeehiveChannelNumber;
  short int dati;
  
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet '");

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

    // A$+A11A... problema nel trovare i caratteri identificativi (si potrebbero cambiare in soli identificativi dell'arnia e.g. A11A11A11A11A11A11A11A11) e nel distinguerli dal numero che ci arriva (si potrebbero contare i bit siccome nota la distanza tra due misure contigue)
    // ENCODING https://lora-developers.semtech.com/documentation/tech-papers-and-guides/the-book/data-packet-transmissions/ 

    // PacketSize Check
    if (packetSize != 24) {
      return;
    }
    
    // Beehive check
    for (int j = 0; j < packetSize; j = j+3) {
      for (int i = 0; i < num_beehives; i++) {
        if (((packet[j] == BEEHIVES_ID[i]) && (packet[j+3] == BEEHIVES_ID[i])) || ((packet[j] == BEEHIVES_ID[i]) && (packet[j+6] == BEEHIVES_ID[i]))) {
          BeehiveChannelNumber=myChannelNumber[i];
          beehive_found=true;
          break;
        }
      }
    }
    if (beehive_found == false) {
        Serial.print("Packet header: beehive unknown! "); // Debug
        Serial.print(packet[0]);
        Serial.print(packet[3]);
        Serial.print(packet[6]);
        Serial.print(packet[9]);
        return;
    }

    for (int i = 1; i < packetsize; i = i+3) {
      dati=packet[i]<<8; // dati=[0000.0000][0000.0000] -> dati=[packet[i]][0000.0000]
      dati=dati+packet[i+1]; // dati=[packet[i]][packet[i+1]]
      ThingSpeak.setField(1+i/3, (float)(dati/100.0));
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

    // Write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(BeehiveChannelNumber, myWriteAPIKey);
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}
