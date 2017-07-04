#include <ArduinoJson.h>
#define MQTTCLIENT_QOS2 1
#define MAX_INCOMING_Q0S2_MESSAGES 10 
#include <MQTTClient.h>
#include <SD.h>
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <SD.h> // Make sure you have this if you're using a disk based storage back-end!
#include <IonDB.h>
#include <IPStack.h>
#include <Countdown.h>

//Network settings
IPAddress ip(192,168,0,14);
IPAddress server(192,168,0,13);
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

const char* topic = "arduino1";

//MQTT client setup
EthernetClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

unsigned long lastMillis = 0;

Dictionary < int, ion_value_t > *schema = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), 20, 3);
Dictionary < int, ion_value_t > *records = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), 20, 3);

void connect() {
  Serial.println("Connecting...");
  int port = 1883;
  char hostname[] = "192.168.0.13";
  int statusId = ipstack.connect(hostname, port);
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.clientID.cstring = (char*) "Arduino1";
  statusId = client.connect(data);
  
 client.subscribe(topic, MQTT::QOS2, messageReceived);
  char* msg = "here";
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = (void*) msg;
  message.payloadlen = strlen(msg)+1;
  client.publish("one", message);
}

int createTable(String tableName, String fieldString) {
  Serial.println("createTable called!");
  ion_value_t ionTableName = (ion_value_t) &tableName;
  ion_value_t ionFieldString = (ion_value_t) &fieldString;
  schema->insert(stringToInt("name"), ionTableName);
  schema->insert(stringToInt("fields"), ionFieldString);
  String fields = (char*) schema->get(stringToInt("fields"));
  Serial.println("fields " + fields);
}

int stringToInt(char* str) {
    int result = 0;
    for(int i = (strlen(str)-1); i >= 0; i--) {
        int charValue = (str[i] - 96);
        result = result + (charValue);
    }
    return result;
}

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);  
  connect();
}

void loop() {
  if (!client.isConnected())
    connect();
  client.yield(1000);
}

void messageReceived(MQTT::MessageData& md) {
  MQTT::Message &message = md.message;
  String payload = (char *) message.payload;
  Serial.println("message received");
  Serial.println(payload);
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(payload);
  const char* operationType = root["operation_type"];
  Serial.println((String) operationType);
  if((String) operationType == "CREATE") {
    const char* tableName = root["query_data"]["table"];
    const char* fieldString = root["query_data"]["fields"];
    createTable(tableName, fieldString);
    Cursor < int, void* > *my_cursor = schema->allRecords();
    while (my_cursor->next()) {
      Serial.println("itereating");
      char* value = (char*) my_cursor->getValue();
      Serial.println((String) value);
    }
  }
}
