#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

const byte DIGITAL_OUTPUT_PIN = D1; //pin do którego podpięty jest LED/przekaźnik
const byte DIGITAL_INPUT_PIN = D2;  //pin do którego podpięty jest przycisk
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(DIGITAL_OUTPUT_PIN, OUTPUT); 
  pinMode(DIGITAL_INPUT_PIN, INPUT_PULLUP);    
  digitalWrite(DIGITAL_OUTPUT_PIN, LOW); 
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERV, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print((char)*payload);
  Serial.println();

  if ((char)*payload == '1'){
    digitalWrite(DIGITAL_OUTPUT_PIN, HIGH); 
    Serial.print("LOW");
  } else if ((char)*payload == '0'){
    digitalWrite(DIGITAL_OUTPUT_PIN, LOW); 
    Serial.print("HIGH");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(SUB_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  static int btn_las_state;
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if ((digitalRead(DIGITAL_INPUT_PIN) != 1) && btn_las_state == 0) {
    Serial.print("Publish message");
    char *msg = "on";
    client.publish(PUB_TOPIC, msg);
    btn_las_state = 1;
  } else if((digitalRead(DIGITAL_INPUT_PIN) == 1) && btn_las_state == 1){
    Serial.print("Publish message");
    char *msg = "off";
    client.publish(PUB_TOPIC, msg);
    btn_las_state = 0;
  }
  delay(10);
}
