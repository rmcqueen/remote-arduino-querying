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
IPAddress ip(192,168,2,2);
IPAddress server(10,110,9,7);

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

const char* topic = "arduino1";
int recordCount = 3; // TODO filthy hack, clean me

//MQTT client setup 
EthernetClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

unsigned long lastMillis = 0;

Dictionary < int, void* > *tables = new SkipList < int, void* > (key_type_numeric_signed, sizeof(int), sizeof(void*), 4);
Dictionary < int, void* > *maxTableSize = new SkipList < int, void* > (key_type_numeric_signed, sizeof(int), 50, 4);

int tableSize = sizeof(*maxTableSize);

void connect() {
  Serial.println("Connecting...");
  int port = 1883;
  char hostname[] = "127.0.0.1";
  int statusId = ipstack.connect(server, port);
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.clientID.cstring = (char*) "Arduino1";
  statusId = client.connect(data);
  
  client.subscribe(topic, MQTT::QOS2, messageReceived);
  sendMessageToTopic("checking in");
//  char* msg = "checking in";
//  MQTT::Message message;
//  message.qos = MQTT::QOS2;
//  message.retained = false;
//  message.dup = false;
//  message.payload = (void*) msg;
//  message.payloadlen = strlen(msg)+1;
//  client.publish("one", message);
}

int createTable(char* tableName, char* fieldString) {
  Serial.println("createTable called");

  // init dictionary
  Dictionary < int, ion_value_t > *table = new SkipList < int, ion_value_t > (key_type_numeric_signed, 32, 20, 3);
  ion_value_t ionTableName = (ion_value_t) tableName;
  ion_value_t ionFieldString = (ion_value_t) fieldString;
  // insert schema into table directory
  table->insert(1, ionTableName);
  table->insert(2, ionFieldString);
  
  // save table in memory
  Dictionary< int, ion_value_t> *tableCache = malloc(tableSize);
  memset(tableCache, 0, tableSize);
  memcpy(tableCache, table, sizeof(*table));
  tables->insert((char*) stringToInt(tableName), tableCache);
  
  Serial.println("Finished creating table");
}

Dictionary< int, ion_value_t>* getTableByName(char* tableName) {
  return ((Dictionary < int, ion_value_t>*) tables->get((char*) stringToInt(tableName)));
}

Cursor<int, void*>* getSchemaCursorByTableName(char* tableName) {
    // save table in memory
    void *tableAddress = tables->get((char*) stringToInt(tableName));
    return ((Dictionary< int, ion_value_t>*) tableAddress)->allRecords();
}

Dictionary <int, ion_value_t>* getTableByTableName(char* tableName) {
    // save table in memory
    void *tableAddress = tables->get((char*) stringToInt(tableName));
    return ((Dictionary< int, ion_value_t>*) tableAddress);
}

void describeTable(char* tableName) {
  Cursor< int, void* > *my_cursor = getSchemaCursorByTableName(tableName);
  String result = "";
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = result + (String) result + "\n";
    Serial.println((String)value);
  }
  sendMessageToTopic(result);
  // Test sending outgoing message
//  MQTT::Message message;
//  message.qos = MQTT::QOS2;
//  message.retained = false;
//  message.dup = false;
//  message.payload = (void*) &result;
//  message.payloadlen = sizeof(result)+1;
//  int statusId = client.publish("one", message);
}

void insertInto(char* tableName, char* tuple) {
  Dictionary< int, ion_value_t>* table = getTableByTableName(tableName);
  table->insert(recordCount, tuple);
  recordCount = recordCount + 1;
  Serial.println("Finsished insert");
}

String selectAll(char* tableName) {
  Cursor< int, ion_value_t > *my_cursor = getSchemaCursorByTableName(tableName);
  String result = "";
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = result + (String) value;
  }
  return result;
}

// Helpers
int stringToInt(char* str) {
    int result = 0;
    for(int i = (strlen(str)-1); i >= 0; i--) {
        int charValue = (str[i] - 96);
        result = result + (charValue);
    }
    return result;
}

void printTableByName(char* tableName) {
  // show insert indeed worked by printing table schema
  Cursor< int, ion_value_t > *my_cursor = getSchemaCursorByTableName(tableName);
  String result = "";
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = result + (String) value;
  }
  Serial.println("Printing table...");
  Serial.println(result);
}

void sendMessageToTopic(String result) {
  char* msg = result.c_str();
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = (void*) msg;
  message.payloadlen = strlen(msg)+1;
  client.publish(topic, message);
}

void messageReceived(MQTT::MessageData& md) {
  MQTT::Message &message = md.message;
  String payload = (char *) message.payload;
  Serial.println("message received");
  Serial.println(payload);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  
  char* opCode = root["op_code"];
  char* tableName = root["query"]["table"];
  Serial.println("Table: " + (String)tableName + "; opCode: " + (String)opCode);
  
  // create table 
  if((String) opCode == "c") {
    char* fieldString = root["query"]["fields"];
    createTable(tableName, fieldString);
    printTableByName(tableName);
  }

  // describe table
  if((String) opCode == "d") {
    describeTable(tableName);
  }

  // insert into table
  if((String) opCode == "i") {
    char* fields = root["query"]["fields"];
    insertInto(tableName, fields);
    Serial.println("Finished insert");
  }

  // select from table
  if((String) opCode == "s") {
    String result = selectAll(tableName);
    Serial.println("Select all result: " + result);
    sendMessageToTopic(result);
  }
}

// main
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
