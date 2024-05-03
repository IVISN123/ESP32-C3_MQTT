#include "WiFi.h"
#include "PubSubClient.h"


/////////////////// SETTINGS /////////////////////////////

// Wi-Fi
const char* ssid = "pressF";
const char* password = "010720291";

// MQTT
const char* mqtt_server = "m5.wqtt.ru";
const int mqtt_port = 8374;
const char* mqtt_user = "u_XDXG92";
const char* mqtt_password = "LjBEE4CRiZ8K";

// RELAY
const String relay_topic = "/topic/Relay-2";
const int RELAY = 0;
const int LED = 8;
/////////////////////////////////////////////////////////



WiFiClient espClient;
PubSubClient client(espClient);

bool relay_on = false;


void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
     delay(250); 
    Serial.print(".");  
  }

  for (int f = 0; f < 7; f++) {
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("Подключение к Wi-FI: Выполнено");
  Serial.println("IP адрес: ");
  Serial.println(WiFi.localIP());
}

void updateStatePins(void){
    if(relay_on){
      digitalWrite(RELAY, HIGH);
      digitalWrite(LED, LOW);
    }else{
      digitalWrite(RELAY, LOW);
      digitalWrite(LED, HIGH);
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    
  String data_pay;
  for (int i = 0; i < length; i++) {
    data_pay += String((char)payload[i]);
  }

    Serial.println(data_pay);
    
  if( String(topic) == relay_topic ){
        if(data_pay == "ON" || data_pay == "1") relay_on = true;
        if(data_pay == "OFF" || data_pay == "0") relay_on = false;
    }

    updateStatePins();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Подключение к MQTT...");
    String clientId = "ESP8266-" + WiFi.macAddress();
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password) ) {
      Serial.println("Подключено");
      
      client.subscribe( (relay_topic + "/#").c_str() );

    } else {
      Serial.print("ОШибка, КОД=");
      Serial.print(client.state());
      Serial.println(" Повторение через 5 секунд...");
      digitalWrite(LED, HIGH);
      delay(2500);
      digitalWrite(LED, LOW);
      delay(2500);
    }
  }
}

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(RELAY, LOW);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();

  }
  client.loop();
}