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

Dictionary<int, int> *tables = new SkipList<int, int>(-1, key_type_numeric_signed, sizeof(int), sizeof(int), 7);

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

int createTable(String tableName) {
  Dictionary<int, int> *my_dictionary = new SkipList<int, int>(-1, key_type_numeric_signed, sizeof(int), sizeof(int), 7);
  //Data type to initialize Key and Value type of dictionary being created
  int type = 0;
  int insertStatus = insertTable(dictionary.instance->id, tableName);
  Serial.println("Created table " + tableName);
}

void insertTable(int id, String tableName) {
  return tables->insert(id, stringToInt(tableName));
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

  //Data type to initialize Key and Value type of dictionary being created
  int type = 1;
  tables = master_table->initializeDictionary(key_type_numeric_signed, type, type, sizeof(int), sizeof(int), 14, dictionary_type_skip_list_t);
  
  connect();
}

void loop() {
  if (!client.isConnected())
    connect();
  client.yield(1000);
}

void messageReceived(String topic, String payload) {
  Serial.println("message received");
  createTable("team")
}
