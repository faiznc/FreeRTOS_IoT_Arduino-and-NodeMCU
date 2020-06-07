/*
 ESP8266 ESP-12E (Nodemcu 1.0) Receiver code
 VCC Module -> pin VU nodemcu
 Data module -> pin D3

 Mencoba terima data dari receiver, lalu mencoba mensubmit nilai bacaannya pada topik "TugasRTOS" di server test.mosquitto.org (MQTT)
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //Library MQTT

#include <RCSwitch.h> //Library Receiver 433 Mhz

const char* ssid = "SSID";
const char* password = "PASSWORD WIFI";
const char* mqtt_server = "test.mosquitto.org"; //Server MQTT, bisa disesuaikan
const char topic[] = "TugasRTOS";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

RCSwitch mySwitch = RCSwitch();
String kode;
String statusnya;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() { //Fungsi untuk cek koneksi dan reconnect ke Wifi
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ClientTgsRTOS";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, "Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); //Mengeset MQTT Server target
  
  pinMode(D4,OUTPUT); //Pin untuk Led BuiltIn dari ESP
  mySwitch.enableReceive(0);  // Connected to GPIO0 (D3) of Nodemcu 1.0 module , Set pin untuk receiver di NodeMCU
}
void loop() {

  if (mySwitch.available()) { //Jika data masuk terdeteksi do ...
    
    int value = mySwitch.getReceivedValue(); //Ambil data yg diterima
    String datanya = String(value);
    for (int i=0 ; i<=3 ; i++){
      kode += datanya.charAt(i);
    }
    statusnya = datanya.charAt(4); //Ambil data STATUS KONDISI, data pada karakter ke-5 dari data yg diterima
    
    Serial.print("Data: ");
    Serial.print(value);
    Serial.print(", Kode: ");
    Serial.print(kode);
    Serial.print(", Status: ");
    Serial.print(datanya.charAt(4));
    Serial.print(", Kondisi: ");
    
    if(statusnya=="1"){
      Serial.println("Gas Terdeteksi!");
      digitalWrite(D4,LOW); //Nyalakan LED_Builtin ESP sebagai notif.

      if (!client.connected()) {
        reconnect();
      }
      int kodenya = kode.toInt();
      snprintf (msg, 50, "Terdeteksi pada ID: %ld", kodenya);

      long now = millis();
      if (now - lastMsg > 2000) { //Publish pesan jika jeda waktu sudah lebih dari 2 detik dari publishing pesan sebelumnya
        lastMsg = now;
        Serial.print("Publish pesan: ");
        Serial.println(msg);
        client.publish(topic, msg); //Publish pesan pada variabel "msg" dengan topik pada variabel "topic"
      }
    }
    else{
      Serial.println("Gas Tidak Terdeteksi");
      digitalWrite(D4,HIGH); //Matikan LED jika tidak ada masalah
    }
    kode = "";
    mySwitch.resetAvailable();
  }
}
