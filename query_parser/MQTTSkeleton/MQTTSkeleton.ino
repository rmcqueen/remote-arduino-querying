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

Dictionary < int, void* > *tables = new SkipList < int, void* > (key_type_numeric_signed, sizeof(int), sizeof(void*), 4);
Dictionary < int, void* > *maxTableSize = new SkipList < int, void* > (key_type_numeric_signed, sizeof(int), 50, 4);

int tableSize = sizeof(*maxTableSize);

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

int createTable(char* tableName, String fieldString) {
  Serial.println("createTable called!");
  Dictionary < int, ion_value_t > *table = new SkipList < int, ion_value_t > (key_type_numeric_signed, 32, 20, 3);
  ion_value_t ionTableName = (ion_value_t) "Team";
  ion_value_t ionFieldString = (ion_value_t) "f1:i;f2:s";
  table->insert(1, ionTableName);
  table->insert(2, ionFieldString);
  Cursor < int, void* > *my_cursor = table->allRecords();
  String result = "";
  
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = result + (String) value;
  }
  
  Serial.print("Table: ");
  Serial.println((String) result);
  
  // save table in memory
  Dictionary< int, ion_value_t> *tableCache = malloc(tableSize);
  memset(tableCache, 0, tableSize);
  memcpy(tableCache, table, sizeof(*table));
  my_cursor = tableCache->allRecords();
  result = "";
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = result + (String) value;
  }
  
  Serial.print("Clone: ");
  Serial.println((String) result);
  tables->insert((char*) stringToInt(tableName), tableCache);
  Serial.println("Finished creating table");
}

Dictionary< int, ion_value_t>* getTableByName(char* tableName) {
  return ((Dictionary < int, ion_value_t>*) tables->get((char*) stringToInt(tableName)));
}

Cursor<int, void*>* getSchemaCursorByTableName(char* tableName) {
    // save table in memory
    void *tableCache = malloc(tableSize);
    void *tableAddress = tables->get((char*) stringToInt(tableName));
    memcpy(tableCache, tableAddress, tableSize);
    return ((Dictionary< int, ion_value_t>*) tableCache)->allRecords();
}

void describeTable(char* tableName) {
  Cursor< int, void* > *my_cursor = getSchemaCursorByTableName(tableName);
  String result = "";
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = result + (String) result + ";";
    Serial.println((String)value);
  }
  
  //Test sending outgoing message
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = (void*) &result;
  message.payloadlen = sizeof(result)+1;
  int statusId = client.publish("one", message);
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
  const char* opCode = root["op_code"];
  Serial.println((String) opCode);

  // create table 
  if((String) opCode == "c") {
    const char* tableName = root["query"]["table"];
    const char* fieldString = root["query"]["fields"];
    createTable(tableName, fieldString);

    // show insert indeed worked by printing table schema
    Cursor< int, ion_value_t > *my_cursor = getSchemaCursorByTableName(tableName);
    String result = "";
    while (my_cursor->next()) {
      char* value = my_cursor->getValue();
      result = result + (String) value;
    }
    
    Serial.println("Final: ");
    Serial.println((String) result);
    //describeAllTables();
  }

  // describe table
  if((String) opCode == "d") {
    const char* tableName = root["query"]["table"];
    describeTable(tableName);
  }
}
