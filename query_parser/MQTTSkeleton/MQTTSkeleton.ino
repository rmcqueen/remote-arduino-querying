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

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x09
};

const char* topic = "arduino1";
const char* outTopic = "query/Arduino1";
int* ptrRecordCount; //TODO filthy hack, clean me
int recordCount;
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
  char hostname[] = "192.168.1.100";
  int statusId = ipstack.connect(hostname, port);
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.clientID.cstring = (char*) "Arduino1";
  statusId = client.connect(data);
  
  client.subscribe(topic, MQTT::QOS2, messageArrived);
//sendMessageToTopic("checking in");
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
   Dictionary < int, ion_value_t > *table = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 3);
   table->insert(1, tableName);
   table->insert(2,fieldString);
  
  // save table in memory
  Dictionary< int, ion_value_t> *tableCache = malloc(tableSize);
  memset(tableCache, 0, tableSize);
  memcpy(tableCache, table, sizeof(*table));
  tables->insert(stringToInt(tableName), tableCache);
  Serial.println("Finished creating table");
}


Dictionary < int, ion_value_t>* getTableByName(char* tableName) {
  return ((Dictionary < int, ion_value_t>*) tables->get(stringToInt(tableName)));
}

Dictionary <int, ion_value_t>* getTableByTableName(char* tableName) {
    // save table in memory
    void *tableAddress = tables->get(stringToInt(tableName));
    return ((Dictionary< int, ion_value_t>*) tableAddress);
}

Cursor<int, void*>* getSchemaCursorByTableName(char* tableName) {
    // save table in memory
    void *tableAddress = tables->get((char*) stringToInt(tableName));
    return ((Dictionary< int, ion_value_t>*) tableAddress)->allRecords();
}

void describeTable(char* tableName) {
  Cursor< int, void* > *my_cursor = getSchemaCursorByTableName(tableName);
  String result = "";
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = result + (String) result + "\n";
    Serial.println((String)value);
  }
} 

void insertInto(char* tableName, char* tuple) {
  Dictionary < int, ion_value_t > *table = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)));
  table->insert(*ptrRecordCount,tuple);
  *ptrRecordCount =  *ptrRecordCount + 1;
  Serial.println("Finsished insert");
}

char* selectAll(char* tableName) {
   Cursor< int, void* > *my_cursor = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)))->allRecords();
   char* result = malloc(sizeof(char));
   while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = realloc(result,(sizeof(char) * (strlen(value) + strlen(result))+1));
    strcat(result,"\n");
    strcat(result,value);
  }
  delete my_cursor;
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
  Serial.println("Printing......");
  Cursor< int, void* > *my_cursor = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)))->allRecords();
     while (my_cursor->next()) {
        Serial.println((char*) my_cursor->getValue());
     }
     delete my_cursor;
}

void sendMessageToTopic(char* result) {
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = (void*) result;
  message.payloadlen = strlen(result)+1;
  client.publish(outTopic, message);
} 

void messageArrived(MQTT::MessageData& md) {
  char payload[128];
  MQTT::Message &message = md.message;
  sprintf(payload,(char*)message.payload);
  payload[message.payloadlen] = '\0';
  Serial.println(payload);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  
  char* opCode = root["op_code"];
  char* tableName = root["query"]["table"];
  //Serial.println("Table: " + (String)tableName + "; opCode: " + (String)opCode);
  
  // create table 
  if((String) opCode == "c") {
     char* fieldString = root["query"]["fields"];
     char* input = malloc(sizeof(char) * strlen(fieldString)+1);
     strcpy(input,fieldString);
     createTable(tableName, input);
    //printTableByName(tableName);
  }

  // describe table
  if((String) opCode == "d") {
    //describeTable(tableName);
  }

  // insert into table
  if((String) opCode == "i") {
    char* fields = root["query"]["fields"];
    char* input = malloc(sizeof(char) * strlen(fields)+1);
    strcpy(input,fields);
    insertInto(tableName, input);
    Serial.println("Finished insert");
  }

  // select from table
  if((String) opCode == "s") {
    sendMessageToTopic(selectAll(tableName));
  }
}

// main
void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);  
  connect();
  recordCount = 3;
  ptrRecordCount = &recordCount;
}

void loop() {
  if (!client.isConnected())
    connect();
  client.yield(1000);
}
