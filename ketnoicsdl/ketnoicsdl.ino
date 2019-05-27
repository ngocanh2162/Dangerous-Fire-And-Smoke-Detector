#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <MQ135.h>
#include <ArduinoJson.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

#define ssid "FPT-Telecom1"//"HPCC_IOT"
#define password "passla6086162"//"hpcc_iot"

const PROGMEM char* mqtt_server = "192.168.1.35";
const PROGMEM uint16_t mqtt_port = 1883;
const PROGMEM char* mqtt_user = "prj1";
const PROGMEM char* mqtt_pwd = "password";
const PROGMEM char* MQTT_SENSOR_TOPIC = "sensor";

#define DHTPIN D1
#define DHTTYPE DHT11
#define PIN_MQ135 A0
MQ135 mq135_sensor = MQ135(PIN_MQ135);
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void publishData(float p_temperature, float p_humidity, float p_ppm) {
  // create a JSON object
  // doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
    StaticJsonDocument<512> jsonBuffer;
    DeserializationError error = deserializeJson(jsonBuffer, file);
    if (error) return;
    JsonObject &root = jsonBuffer.as<JsonObject>();
  // INFO: the data must be converted into a string; a problem occurs when using floats...
  root["temperature"] = (String)p_temperature;
  root["humidity"] = (String)p_humidity;
  root["ppm"] = (String)p_ppm;
  root.prettyPrintTo(Serial);
  Serial.println("");
  /*
     {
        "temperature": "23.20" ,
        "humidity": "43.70"
     }
  */
  char data[200];
  root.printTo(data, root.measureLength() + 1);
  client.publish(MQTT_SENSOR_TOPIC, data, true);
  yield();
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  /Serial.begin(115200);
  //setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  dht.begin();
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
  // init the MQTT connection
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float ppm = mq135_sensor.getPPM();
  if (isnan(h) || isnan(t)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
    return;
  } else {
    //Serial.println(h);
    //Serial.println(t);
    publishData(t, h, ppm);
  }

  Serial.println("INFO: Closing the MQTT connection");
  client.disconnect();

  Serial.println("INFO: Closing the Wifi connection");
  WiFi.disconnect();

  ESP.deepSleep(SLEEPING_TIME_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  delay(500); // wait for deep sleep to happen
}
