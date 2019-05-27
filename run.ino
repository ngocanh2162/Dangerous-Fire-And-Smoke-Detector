#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <MQ2.h>

// Wifi
#define ssid "linksys"//"ICSE_Child"//"HPCC_IOT"
#define password ""//"icse_child@123"//"hpcc_iot"

// MQTT Broker
#define mqtt_server "192.168.1.233" // "192.168.1.40" 
#define mqtt_topic_pub "demo"   
#define mqtt_topic_sub "demo"
#define mqtt_user "prj1"   
#define mqtt_pwd "password"
//
const uint16_t mqtt_port =  1883; 

#define DHTPIN D1 // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
#define PIN_MQ2 A0
#define red D8
#define green D3

DHT dht(DHTPIN, DHTTYPE);
MQ2 mq2_sensor = MQ2(PIN_MQ2);

//
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50]; 
int value = 0;

void setup() {
  Serial.begin(115200);
  //setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  dht.begin();
  mq2_sensor.begin();
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  //
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived []");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      client.publish(mqtt_topic_pub, "ESP_reconnected");
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float gas = mq2_sensor.readLPG();
  float smoke = mq2_sensor.readSmoke();
  if (isnan(h) || isnan(t)|| isnan(gas)|| isnan(smoke)) {
   Serial.println("Failed to read from sensor!");
   return;
  }
  
  // Sau mỗi 2s sẽ thực hiện publish lên MQTT broker
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    static char temperatureTemp[7];
    dtostrf(t, 6, 2, temperatureTemp);
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);
    static char gasTemp[7];
    dtostrf(gas, 6, 2, gasTemp);
    static char smokeTemp[7];
    dtostrf(smoke, 6, 2, smokeTemp);
    // Publishes Temperature and Humidity values
    client.publish("sensor/temp", temperatureTemp);
    client.publish("sensor/hum", humidityTemp);
    client.publish("sensor/gas", gasTemp);
    client.publish("sensor/smoke", smokeTemp);
    //
    Serial.print("hum: ");         Serial.print(h);
    Serial.print(" %\t temp: ");   Serial.print(t);  
    Serial.print(" *C\t gas: ");   Serial.print(gas);
    Serial.print("\t smoke: ");    Serial.print(smoke);
    Serial.print("\n ");
  }
  //
  if (t <= 40 && gas <= 300 && smoke <= 3000){
        digitalWrite(green, LOW); 
        digitalWrite(red, HIGH);
  }
  else {
        digitalWrite(red, LOW); 
        digitalWrite(green, HIGH);
  }
}
