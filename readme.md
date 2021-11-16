The aim of this project is to delvelop a device with ARDUINO able to measure different quantities to keep control of a beehive and to transmit the data 
with LoRa module to another arduino (the idea is to transmit them to home and analyse them to know the status of the beehive).
We measured the following parameters of the beehive:

TEMPERATURE:
T1=CLUSTER of bees
T2=T2=LEFT or EST STOCKS
T3=RIGHT or OVEST STOCKS
T4=CLUSTER of bees (safety)
T5=OUTSIDE

HUMIDITY:
DHT0=humidity inside the beehive

PIR:
Flux oh bees entering and exiting the beehive to have an idea of the activity

Lo-Ra:
transmits the data to a second arduino

ESP32:
Receive with LoRa the data from the beehives and uploads them with wifi

ThingSpeak:
Analysis of the data with Matlab code

DATA ANALYSIS:
Code to analyse data with Matlab
