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
#define DHT_IN_PIN 3
// Data wire bus for DS18B20 in digital pin 4
#define ONE_WIRE_BUS 4 // BRUTTO TESTARE
#define PIR_PIN 5
#define LEDpin 13
//LoRa Ra-02 pin connections: GND->GND, 3.3V->3.3V, RST->D9, DIO0->D2, NSS->D10, MOSI->D11, MISO->D12, SCK->D13

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS); //
// Pass oneWire reference to DallasTemperature library for DS18B20
DallasTemperature sensors(&oneWire);


// Number of TEMPERATURE sensors PREDEFINED
const int numDS18B20 = 5;
float MaxCount=1200;
int PIRread = 0;

// BRUTTO float numbers require 2byte, we could save the measurements instantly as integer types (int=measurement*100)
int tempC[numDS18B20]; //12 significant bits

// BRUTTO Addresses of DS18B20 sensors retrived with the Sensor_Identifier_DS18B20.ino program. This procedure can be AUTOMATIZED for a large scale production 
DeviceAddress address_T1 = {0x28, 0x96, 0x25, 0x46, 0x5F, 0x20, 0x01, 0x65}; // T1=CLUSTER of bees
DeviceAddress address_T2 = {0x28, 0x3E, 0xA8, 0x93, 0x63, 0x20, 0x01, 0x5A}; // T2=LEFT or EST STOCKS
DeviceAddress address_T3 = {0x28, 0xE2, 0x8A, 0x7A, 0x62, 0x20, 0x01, 0xE9}; // T3=RIGHT or OVEST STOCKS
DeviceAddress address_T4 = {0x28, 0x65, 0x7B, 0x5C, 0x5F, 0x20, 0x01, 0x5A}; // T4=CLUSTER of bees (safety)
DeviceAddress address_T5 = {0x28, 0x49, 0x7C, 0x51, 0x5F, 0x20, 0x01, 0x5D}; // T5=OUTSIDE


// Number of HUMIDITY sensors PREDEFINED
//const int numDHT = 1;

//float temperature[numDHT]; // Temperature measured from the DHT, not needed since we used sensors T1 T2 T3 T4 T5
//float humidity[numDHT];
int humidity;

// Initialize the DHT sensors INSIDE and OUTSIDE the beehive
DHT_Unified dht_in(DHT_IN_PIN, DHTTYPE);
//DHT_Unified dht_out(DHT_OUT_PIN, DHTTYPE);


void setup(void)
{
  // Start up the library for DS18B20 and for DHT
  sensors.begin();
  dht_in.begin();
  //dht_out.begin();
  pinMode(PIR_PIN, INPUT);
  Serial.begin(9600);
  
  // Check visually for the power light (spia di accensione)
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, HIGH);
  delay(3000);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  // BRUTTO we have to implement the check for connection to receiver
  digitalWrite(LEDpin, LOW);
  
  // BRUTTO We saves datas on a MicroSD so we could send them via LoRa only once a day (Solar Panel, so when light is max)
  // However if the battery ended or for every inconvenient crash we should send all the datas that probably wasn't able to send
}

void loop(void)
{ 
  
  // PIR sensor "number" of detections=detect and "number" of nothing=tool used to have the same time (a discapito della battery)
  int count=0;
  int detect=0;
  int nothing=0;
  
  // Send command to all DS18B20 for temperature conversion
  sensors.requestTemperatures();
  
  // Get temperature from a specified sensor (position of sensors is physically written on the sensor), in order to keep track for the physical position of sensors
  tempC[0] = sensors.getTempC(address_T1)*100;
  delay(100);
  tempC[1] = sensors.getTempC(address_T2)*100;
  delay(100);
  tempC[2] = sensors.getTempC(address_T3)*100;
  delay(100);
  tempC[3] = sensors.getTempC(address_T4)*100;
  delay(100);
  tempC[4] = sensors.getTempC(address_T5)*100;
  delay(100);
  
  // Display the temperature values in order to test/debug
  for (int i = 0;  i < numDS18B20;  i++)
  {
    // BRUTTO Serial.print always try to print even though Arduino is not connected to a Serial Monitor?????????????????
    Serial.print("Sensor T");
    Serial.print(i+1);
    Serial.print(" : ");
    Serial.print(tempC[i]/100.0);
    Serial.print("C ");
  }
  
  Serial.println("");
  
  // BRUTTO Not optimized from a memory point of view, since sensors_event_t is a ~40byte data and we only need a 2byte float
  sensors_event_t event;
  
  // Get humidity and print its value
  dht_in.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    // BRUTTO Serial.print(F()) using F() we are moving constant strings to the program memory instead of the ram
    Serial.println(F("Error reading humidity IN!"));
    humidity=-126;
  }
  // Display the humidity values in order to test/debug
  else {
    humidity=event.relative_humidity*100;
    Serial.print(F("Humidity in: "));
    Serial.print(humidity/100.0);
    Serial.println(F("%"));
  }
  
  /*
  // Get humidity and print its value
  dht_in.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    // BRUTTO Serial.print(F()) using F() we are moving constant strings to the program memory instead of the ram
    Serial.println(F("Error reading humidity IN!"));
    // BRUTTO TRY FOR SOME TIME AND IF IT DOESN'T RESPOND THAN ALWAYS RETURN AN ERROR VALUE AND CALL IT NO MORE
  }
  // Display the humidity values in order to test/debug
  else {
    humidity[0]=event.relative_humidity;
    Serial.print(F("Humidity in: "));
    Serial.print(humidity[0]);
    Serial.println(F("%"));
  }
  
   // Get humidity and print its value
  dht_out.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    // BRUTTO Serial.print(F()) using F() we are moving constant strings to the program memory instead of the ram
    Serial.println(F("Error reading humidity OUT!"));
    // BRUTTO TRY FOR SOME TIME AND IF IT DOESN'T RESPOND THAN ALWAYS RETURN AN ERROR VALUE AND CALL IT NO MORE
  }
  // Display the humidity values in order to test/debug
  else {
    humidity[1]=event.relative_humidity;
    Serial.print(F("Humidity out: "));
    Serial.print(humidity[1]);
    Serial.println(F("%"));
  }
  */
  
  // BRUTTO it could be used as in the examples of PIR, which implies a better accuracy by interrupting the cycle using an external CLOCK
  // BRUTTO The external CLOCK could also be used to WAKE UP arduino from sleep
  // Get percentual time of activation of PIR
  // In order for this to work we had to estimate the time on which the PIR was IN ALLARM (~we set it to 1.6s manually) and the time on which the alarm was BLOCKED (~3s from datasheet)
  // We used the program PIR_alarm_stop_time.ino and estimated the time through a mean of time shown at the Serial Monitor
  while (count<MaxCount){
    PIRread = digitalRead(PIR_PIN);  // read PIR value
    if (PIRread == HIGH) { 
        delay(100);
        detect=detect+1;
      }
      else{
        delay(100);
        nothing=nothing+1;
      }
    count=count+1;
  }

    Serial.println(detect/MaxCount);

  
  // send packets of data, 1 packet each measure
  // BRUTTO ASCII table to make the markers different as much as possible
  LoRa.beginPacket();
  LoRa.print("a");
  LoRa.print(tempC[0]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("d");
  LoRa.print(tempC[1]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("h");
  LoRa.print(tempC[2]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("n");
  LoRa.print(tempC[3]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("p");
  LoRa.print(tempC[4]);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("w");
  LoRa.print(humidity);
  LoRa.endPacket();
  delay(100);
  
  LoRa.beginPacket();
  LoRa.print("z");
  LoRa.print(detect/MaxCount*333.3);
  LoRa.endPacket();
  
  delay(60000); // BRUTTO sleep
}
