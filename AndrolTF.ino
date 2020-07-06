#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#define MQTTsubQos 1

WiFiClient nodeClient;
PubSubClient client(nodeClient);
const int ON = 0;
const int OFF = 1;

long lastMsg;


const char* ssid = "raspi-webgui";
const char* password = "ChangeMe";

const char* mqtt_server = "10.3.141.1";
const int mqtt_port = 1883;

//Change the following four as per the node number 
const char* clientID = "Node0";
const char* outTopic = "outTopic/0";
const char* inTopic = "inTopic/0";
const char* analyticsTopic = "analytics/0";

String command;
int ledpin[10] = { D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, };

long timeArray[10] = {};
long tempTimeArray[10] = {};
int seconds = 0;
int timeNow = 0;
int timeLast = 0;

String toBeSentString = "";

void setup() {
  for(int i=0; i<10; i++) {
    pinMode(ledpin[i], OUTPUT);
    digitalWrite(ledpin[i], OFF);
  }
    
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void setup_wifi() {
  delay(10);

  
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
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
  String message = "";
  for(int i=0; i<3; i++) {
    message += (char)payload[i];
  }
  
  Serial.println(message);
  toggle(message);
}

void reconnect() {
  while (!client.connected()) {
    if(WiFi.status() != WL_CONNECTED) {
      setup_wifi();
      delay(10);
      continue;
    }
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientID)) {
      Serial.println("connected");
      
      client.subscribe(outTopic, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
     
      delay(1000);
    }
  }
}

void loop() {
  delay(10);

  

  if (!client.connected()) {
    reconnect();
    delay(10);
  }
  delay(10);
  client.loop();
  delay(10);
  
  timeNow = millis()/1000;
  seconds = timeNow - timeLast;
  
  if(seconds >= 60) {
    for(int i=0; i<10; i++) 
      setTimeArray(i);
    
    printTimeArray();

    toBeSentString = timeArrayToString();

    char char_to_be_sent_array[toBeSentString.length()+1];

    toBeSentString.toCharArray(char_to_be_sent_array, toBeSentString.length()+1);
    client.publish(analyticsTopic, char_to_be_sent_array, false);
    delay(10);
    for(int i=0; i<10; i++) 
      timeArray[i] = 0;
    delay(10);
    seconds = 0;
    timeLast = millis()/1000;
  }
 
  delay(10);
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

  if(sta == 1) {
    digitalWrite(ledpin[dev], ON);
   
    tempTimeArray[dev] = millis()/1000;
  }
  else {
    digitalWrite(ledpin[dev], OFF); 
    
    setTimeArray(dev);
    printTimeArray();
  }
  char char_message_array[message.length()+1];

  message.toCharArray(char_message_array, message.length()+1);
  Serial.println(message);
  client.publish(inTopic, char_message_array, false);
}

void printTimeArray() {
  for(int i=0; i<10; i++)
      Serial.print(String(timeArray[i]) + " ");
  Serial.println("");
}

void setTimeArray(int pin) {
  if(tempTimeArray[pin] != 0 && digitalRead(ledpin[pin]) == OFF) {
    timeArray[pin] += millis()/1000 - tempTimeArray[pin];
    tempTimeArray[pin] = 0;
  }
  else if(tempTimeArray[pin] != 0) {
    timeArray[pin] += millis()/1000 - tempTimeArray[pin];
    tempTimeArray[pin] = millis()/1000;
  }
}

String timeArrayToString() {
  String tempString = "";

  for(int i=0; i<10; i++)
    tempString += String(timeArray[i]) + " ";

  return tempString;
}

