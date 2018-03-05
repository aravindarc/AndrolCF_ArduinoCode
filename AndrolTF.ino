#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#define MQTTsubQos 1


WiFiClient nodeClient;
PubSubClient client(nodeClient);
const int ON = 0;
const int OFF = 1;

long lastMsg;


const char* ssid = "aravind";
const char* password = "87654321";

const char* mqtt_server = "m14.cloudmqtt.com";
const char* inTopic = "inTopic";
const int mqtt_port = 19276;
const char* mqttUser = "nulbdnqz";
const char* mqttPass = "74_3fxYI5EXx";

String command;
int ledpin[10] = { D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, };

void setup() {
  for(int i=0; i<10; i++) {
    pinMode(ledpin[i], OUTPUT);
    digitalWrite(ledpin[i], OFF);
  }
    
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived: {");
  Serial.print(topic);
  Serial.print("] ");
  char temp[4];
  for(int i=0; i<length; i++) {
    temp[i] = (char)payload[i];
  }
  String message = String(temp);

  Serial.println(message);

  toggle(message);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqttUser, mqttPass)) {
      Serial.println("connected");
      
      client.subscribe("outTopic", 1);
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
  delay(300);
}

void toggle(String message) {
  int index = message.indexOf(' ');

  Serial.println("index of ' ': " + String(index));

  char device[2];

  
  
  char stat;

  
  
  for(int i=0; i<index; i++)
    device[i] = message.charAt(i);
  stat = message.charAt(index + 1);

  Serial.println("device char arr: " + String(device[0]));

  Serial.println("status char arr: " + String(stat));
  int dev = String(device).toInt();
  int sta = String(stat).toInt();

  Serial.println("device int string: " + String(dev));

  Serial.println("status int string: " + String(sta));

  if(sta == 1) 
    digitalWrite(ledpin[dev], ON);
  else
    digitalWrite(ledpin[dev], OFF); 

  char char_message_array[message.length()+1];

  message.toCharArray(char_message_array, message.length()+1);

  if(client.publish(inTopic, char_message_array, true))
    Serial.println(message);
  else {
    reconnect();
    client.publish(inTopic, char_message_array, true);
    Serial.println(message);
  }
}

