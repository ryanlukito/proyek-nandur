#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char *mqtt_broker = "broker.emqx.io";
const char *topic = "project-nandur-sub/1";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

StaticJsonDocument<100> doc;

const int airValue = 790; // template internet
const int waterValue = 390; //template internet
const int sensorPin = 34;
float smv = 0;
float smp = 0;
bool res;

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
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  WiFiManager wm;
  wm.resetSettings();
  res = wm.autoConnect("AutoConnectAP", "password");

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected!");
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

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

}

void loop() {
  client.loop();
  // put your main code here, to run repeatedly:
  smv = analogRead(sensorPin);
  Serial.println(smv);
  smp = map(smv, airValue, waterValue, 0, 100);
  unsigned long now = millis();
  char jsonString[100];

  if (now - lastMsg > 2000) {
    lastMsg = now;

    if(smp > 100) {
      Serial.println("100 %");

      doc["moisture"] = smv;
      serializeJson(doc, jsonString);
      client.publish(topic, jsonString);
      Serial.println("Data Published: ");
      Serial.println(jsonString);
      
      display.setCursor(45,0);  //oled display
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.println("Soil");
      display.setCursor(20,15);  
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.println("Moisture");
      
      display.setCursor(30,40);  //oled display
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.println("100 %");
      display.display();
      
      delay(250);
      display.clearDisplay();
    } else if (smp < 0) {
      Serial.println("0 %");

      doc["moisture"] = smv;
      serializeJson(doc, jsonString);
      client.publish(topic, jsonString);
      Serial.println("Data Published: ");
      Serial.println(jsonString);
    
      display.setCursor(45,0);  //oled display
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.println("Soil");
      display.setCursor(20,15);  
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.println("Moisture");
      
      display.setCursor(30,40);  //oled display
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.println("0 %");
      display.display();
    
      delay(250);
      display.clearDisplay();
    } else if (smp >=0 && smp <= 100) {
      Serial.print(smp);
      Serial.println("%");

      doc["moisture"] = smv;
      serializeJson(doc, jsonString);
      client.publish(topic, jsonString);
      Serial.println("Data Published: ");
      Serial.println(jsonString);
      
      display.setCursor(45,0);  //oled display
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.println("Soil");
      display.setCursor(20,15);  
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.println("Moisture");
      
      display.setCursor(30,40);  //oled display
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.println(smp);
      display.setCursor(70,40);
      display.setTextSize(3);
      display.println(" %");
      display.display();
    
      delay(250);
      display.clearDisplay();
    }
  }
}
