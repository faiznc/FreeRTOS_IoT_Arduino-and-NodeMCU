# FreeRTOS_IoT_Arduino-and-NodeMCU
Implementation of FreeRTOS with IOT, using Arduino + transmitter and Nodemcu + Receiver with Wifi connection to publish MQTT message

Using 2 Separate Microcontrollers

1. Arduino Micro (Uno,mini,etc compatible) 
   With 433Mhz Transmitter Module and a Gas sensor as data source
   
   Read analog voltage of pin A0 in arduino and transmit data using transmitter. Coordinated by FreeRTOS. 
   <br></br>
   
2. NodeMCU / ESP8266 
   With 433Mhz Receiver
   
   Receive data from transmitter, and react based on incoming data. In this case we publish MQTT message.
