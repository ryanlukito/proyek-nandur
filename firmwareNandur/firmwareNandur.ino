#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

const int DHT_PIN = 21;  
// const char *ssid = "GOWINDA";
// const char *password = "06111967";

const char *mqtt_broker = "broker.emqx.io";
const char *topic = "project-nandur-sub/1";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

DHT sensor_dht(DHT_PIN,DHT22);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
float temp = 0;
float hum = 0;

StaticJsonDocument<100> doc;

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void setup() {
  WiFi.mode(WIFI_STA);
  // put your setup code here, to run once:
  Serial.begin(115200);
  sensor_dht.begin();
  WiFiManager wm;
  wm.resetSettings();

  bool res;

  res = wm.autoConnect("AutoConnectAP", "password");

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected!");
  }
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.println("Connecting to WiFi");
  // }
  Serial.println("Connected to the Wi-Fi Network");
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public MQTT Broker connected");
    } else {
      Serial.print("failed with state");
      Serial.print(client.state());
      delay(200);
    }
  }

  // client.publish(topic, "Hi, I'm ESP32 ^^");
  // client.subscribe(topic);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) { //perintah publish data
    lastMsg = now;
    
    float temp = sensor_dht.readTemperature();
    float hum = sensor_dht.readHumidity();
    
    doc["Temperature"] = temp;
    doc["Humidity"] = hum;
 
    char jsonString[100];
    serializeJson(doc, jsonString);
    client.publish(topic, jsonString);

    Serial.println("Data Published: ");
    Serial.println(jsonString);

    delay(2000);
  }

}
