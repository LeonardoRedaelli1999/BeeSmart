// Libraries for DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
// Libraries for DHT11/22
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
// Libraries for comunicating via LoRa
#include <SPI.h>
#include <LoRa.h>

// Initialize the commands for SLEEP to save energy (from course slides: Enerlib.h)

// Sensor type for the humidity sensor
#define DHTTYPE DHT22   // DHT 22
// Humidity sensor DHT INSIDE pin 3 the beehive
#define DHT_PIN 3
// Data wire bus for DS18B20 in digital pin 4
#define ONE_WIRE_BUS 4
#define PIR_PIN1 5 // Fiocco NERO, RIGHT, tempo di attivazione ~1.6s e bloccato ~3s
#define PIR_PIN2 6 // Fiocco BIANCO, LEFT, tempo di attivazione ~1.2s e bloccato ~4.1s
#define LEDpin 8
//LoRa Ra-02 pin connections: GND->GND, 3.3V->3.3V, RST->D9, DIO0->D2, NSS->D10, MOSI->D11, MISO->D12, SCK->D13

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library for DS18B20
DallasTemperature sensors(&oneWire);


// Number of TEMPERATURE sensors PREDEFINED
const int numDS18B20 = 5;
float MaxCount=1200;
int PIRread1 = 0;
int PIRread2 = 0;
int T[numDS18B20];

// BRUTTO Addresses of DS18B20 sensors retrived with the Sensor_Identifier_DS18B20.ino program. This procedure can be AUTOMATIZED for a large scale production 
DeviceAddress address_T1 = {0x28, 0x96, 0x25, 0x46, 0x5F, 0x20, 0x01, 0x65}; // T1=CLUSTER of bees
DeviceAddress address_T2 = {0x28, 0x3E, 0xA8, 0x93, 0x63, 0x20, 0x01, 0x5A}; // T2=LEFT or EST STOCKS
DeviceAddress address_T3 = {0x28, 0xE2, 0x8A, 0x7A, 0x62, 0x20, 0x01, 0xE9}; // T3=RIGHT or OVEST STOCKS
//DeviceAddress address_T4 = {0x28, 0x65, 0x7B, 0x5C, 0x5F, 0x20, 0x01, 0x5A}; // T4=CLUSTER of bees (safety)
DeviceAddress address_T5 = {0x28, 0x49, 0x7C, 0x51, 0x5F, 0x20, 0x01, 0x5D}; // T5=OUTSIDE

int humidity;

// Initialize the DHT sensors INSIDE the beehive
DHT_Unified dht(DHT_PIN, DHTTYPE);


void setup(void)
{
  // Start up the library for DS18B20 and for DHT
  sensors.begin();
  dht.begin();
  pinMode(PIR_PIN1, INPUT);
  pinMode(PIR_PIN2, INPUT);
  Serial.begin(9600);
  
  // Check visually for the power light (spia di accensione)
  pinMode(LEDpin, OUTPUT);
  delay(3000);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    digitalWrite(LEDpin, HIGH);
    while (1);
  }
  // BRUTTO we have to implement the check for connection to receiver
  
  // BRUTTO We saves datas on a MicroSD so we could send them via LoRa only once a day (Solar Panel, so when light is max)
  // However if the battery ended or for every inconvenient crash we should send all the datas that probably wasn't able to send
}

void loop(void)
{ 
  // BRUTTO Not optimized from a memory point of view, since sensors_event_t is a ~40byte data and we only need a 4byte float/int
  sensors_event_t event;
  
  // PIR sensor "number" of detections=detect and "number" of nothing=tool used to have the same time (a discapito della battery)
  int count=0;
  int nothing=0;
  int detect1=0;
  int detect2=0;
  
  
  // Send command to all DS18B20 for temperature conversion
  sensors.requestTemperatures();
  
  // Get temperature from a specified sensor (position of sensors is physically written on the sensor), in order to keep track for the physical position of sensors
  T[0] = sensors.getTempC(address_T1)*100;
  delay(50);
  T[1] = sensors.getTempC(address_T2)*100;
  delay(50);
  T[2] = sensors.getTempC(address_T3)*100;
  delay(50);
  //T[3] = sensors.getTempC(address_T4)*100;
  //delay(50);
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    T[3]=-12700;
  } else {
    T[3]=event.temperature*100;
  }
  T[4] = sensors.getTempC(address_T5)*100;
  delay(50);
  
  // Display the temperature values in order to test/debug
  for (int i = 0;  i < numDS18B20;  i++) {
    // BRUTTO Serial.print always try to print even though Arduino is not connected to a Serial Monitor?????????????????
    Serial.print("Sensor T");
    Serial.print(i+1);
    Serial.print(" : ");
    Serial.print(T[i]/100.0);
    Serial.print("C ");
  }
  
  Serial.println("");
  
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    // BRUTTO Serial.print(F()) using F() we are moving constant strings to the program memory instead of the ram
    Serial.println(F("Error reading humidity IN!")); // Debug
    humidity=-12600;
  } else {
    humidity=event.relative_humidity*100;
    Serial.print(F("Humidity in: ")); // Debug
    Serial.print(humidity/100.0);
    Serial.println(F("%"));
  }
  
  // BRUTTO it could be used as in the examples of PIR, which implies a better accuracy by interrupting the cycle using an external CLOCK
  // BRUTTO The external CLOCK could also be used to WAKE UP arduino from sleep
  // Get percentual time of activation of PIR
  // In order for this to work we had to estimate the time on which the PIR was IN ALLARM (~we set it to 1.6s manually) and the time on which the alarm was BLOCKED (~3s from datasheet)
  // We used the program PIR_alarm_stop_time.ino and estimated the time through a mean of time shown at the Serial Monitor
  while (count<MaxCount){
    PIRread1 = digitalRead(PIR_PIN1);  // read PIR1 value
    PIRread2 = digitalRead(PIR_PIN2);  // read PIR2 value
    if (PIRread1 == HIGH && PIRread2 == HIGH) { 
        delay(100);
        detect1=detect1+1;
        detect2=detect2+1;
      } else if (PIRread1 == HIGH && PIRread2 == LOW) {
        delay(100);
        detect1=detect1+1;
        nothing=nothing+1;
      } else if (PIRread1 == LOW && PIRread2 == HIGH) {
        delay(100);
        detect2=detect2+1;
        nothing=nothing+1;
      } else {
        delay(100);
        nothing=nothing+1; // These operations are useless, they are done in order to stop for the same amount of time in each else
        nothing=nothing+1;
      }
    count=count+1;
  }

    Serial.println(detect1/MaxCount*333.3);
    Serial.println(detect2/MaxCount*449.0);

  
  // Send packets of data, 1 packet each measure
  // In order to check for possible errors during transmission, the identifing letters differs for more than 2 bits in their ASCII representation 
  // Resulting in A, F, K, P, c, h, m, r. Each packet has 2 identifiers: the first representing the beehive, while the second one representing the sensor.
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("A");
  LoRa.print(T[0]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("F");
  LoRa.print(T[1]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("K");
  LoRa.print(T[2]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("P");
  LoRa.print(T[3]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("c");
  LoRa.print(T[4]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("h");
  LoRa.print(humidity);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("m");
  LoRa.print(detect1/MaxCount*333.3);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("A");
  LoRa.print("r");
  LoRa.print(detect2/MaxCount*449.0);
  LoRa.endPacket();
  
  delay(60000); // BRUTTO sleep
}
