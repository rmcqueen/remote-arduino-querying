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
#include <SD.h> 
#include <IonDB.h>
#include <IPStack.h>
#include <Countdown.h>

//Network
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x04
};
EthernetClient c;
IPStack ipstack(c);


//MQTT 
const char* topic = "query/Arduino4";
const char* outTopic = "result/Arduino4";
const int MAX_MQTT_PACKET_SIZE = 512;
MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE> client = MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE>(ipstack);

unsigned long lastMillis = 0;

//Tables
Dictionary < int, void* > *tables = new FlatFile < int, void* > (key_type_numeric_signed, sizeof(int), sizeof(void*), 4);
Dictionary < int, void* > *maxTableSize = new FlatFile < int, void* > (key_type_numeric_signed, sizeof(int), 50, 4);
int tableSize = sizeof(*maxTableSize);
int* ptrRecordCount; //TODO filthy hack, clean me
int recordCount;

int flushSkipList(Dictionary <int, ion_value_t > *dict) {
  Serial.println("Closing SkipList...");
  char *tableName = dict->get(1); // all dictionaries have the table name in the first
  FILE *dataFile;
  dataFile = fopen(tableName, "w+b");
  Cursor< int, void* > *tableCursor = dict->allRecords();
  int recordCount = 0;
  int *key = malloc(sizeof(int));
  char *value = malloc(16);
  while (tableCursor->next()) {
    int thisKey = tableCursor->getKey();
    memcpy(key, &thisKey, sizeof(int));
    memcpy(value, tableCursor->getValue(), 16);
    printf("%d,%s\n", *key, value);
    fwrite(key, sizeof(int), 1, dataFile);
    fwrite(value, 16, 1, dataFile);
    recordCount++;
  }
  free(key);
  free(value);
  fwrite(&recordCount, sizeof(int), 1, dataFile);
  fclose(dataFile);
  delete dict;
  delete tableCursor;
  return 0;
}

int openSkipList(Dictionary <int, ion_value_t > *dict, char* tableName) {
  Serial.println("Opening SkipList...");
  FILE *dataFile;
  dataFile = fopen(tableName, "r+b");
  int recordCount = 3;
  fseek(dataFile, -1 * sizeof(int), SEEK_END);
  fread(&recordCount, sizeof(int), 1, dataFile);
  printf("Record count read = %d\n", recordCount);
  fseek(dataFile, 0, SEEK_SET);

  for (int i = 0; i < recordCount; i++) {
    int *key = malloc(sizeof(int));
    char *value = malloc(16);
    fread(key, sizeof(int), 1, dataFile);
    fread(value, 16, 1, dataFile);
    printf("%d,%s\n", *key, value);
    dict->insert(*key, value);
  }
  fclose(dataFile);
  return 0;
}

int connect() {
  Serial.println("Connecting...");
  int port = 1883;
  char hostname[] = "192.168.1.4"; // CHANGE ME TO YOUR HOSTNAME
  ipstack.connect(hostname, port);
 
  
  // ONLINE/OFFLINE detecting code
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
  data.willFlag = 1;
  data.clientID.cstring = (char*) "Arduino1";                               //Set for each Arduino
  data.will.topicName.cstring = (char*) "status/Arduino1";                  //Set to status/ <ID>
  data.will.qos = 2;
  data.will.retained = 1;
  data.will.message.cstring = (char*) "offline";
  client.connect(data);

  //Online flag
  char buf[128];
  sprintf(buf,"online");
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = true;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf);
  client.publish("status/Arduino4", message);      
  client.subscribe(topic, MQTT::QOS2, messageArrived);
  if(client.isConnected()) {
    Serial.println("Connected!");
    return 0;
  } else {
    Serial.println("Not yet connected...");
    return -1;
  }
}

int createTable(char* tableName, char* fieldString) {
  Serial.println("createTable called");
  tables->get(stringToInt(tableName));
  if(!(err_item_not_found == tables->last_status.error))
     return -1;

  // init dictionary
   Dictionary < int, ion_value_t > *table = new FlatFile < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 3);
   table->insert(1, tableName);
   table->insert(2, fieldString);
  
  // save table in memory
  Dictionary< int, ion_value_t> *tableCache = malloc(tableSize);
  memset(tableCache, 0, tableSize);
  memcpy(tableCache, table, sizeof(*table));
  tables->insert(stringToInt(tableName), tableCache);
  Serial.println("Finished creating table");
  return 0;
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

int insertInto(char* tableName, char* tuple) {
  Dictionary < int, ion_value_t > *table = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)));
  if(err_item_not_found == tables->last_status.error)
     return -1;
  Serial.println("Table gotten");
  Serial.println(tuple);
  table->insert(*ptrRecordCount, tuple);
  Serial.println("Record inserted...");
  *ptrRecordCount =  *ptrRecordCount + 1;
  return 0;
}

char* selectAll(char* tableName) {
  int recordsBuffered = 0;
  int maxRecordsPerPage = 5;
  Dictionary < int, ion_value_t > *table = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)));
  if(err_item_not_found == tables->last_status.error)
    return "-1";
  Cursor< int, void* > *my_cursor = table->allRecords();
  char* result = (char*) malloc(1);
  result[0] = '\0';
  char* value;
  while (my_cursor->next()) {
    if(recordsBuffered == maxRecordsPerPage) {
      Serial.println("Sending page...");
      value = ";EOP";
      result = realloc(result,(sizeof(char) * (strlen(value) + strlen(result))+1));
      strcat(result,"\n");
      strcat(result, value);
      sendMessageToTopic(result);
      delay(500);
      result = malloc(1);
      result[0] = '\0';
      recordsBuffered = 0;
      continue;
    }
    value = my_cursor->getValue();
    result = realloc(result,(sizeof(char) * (strlen(value) + strlen(result))+1));
    strcat(result,"\n");
    strcat(result, value);
    recordsBuffered++;
  }
  value = ";EOR";
  result = realloc(result,(sizeof(char) * (strlen(value) + strlen(result))+1));
  strcat(result,"\n");
  strcat(result, value);
  sendMessageToTopic(result);
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
  printf("Sending %s to %s\n", result, outTopic);
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = (void*) result;
  message.payloadlen = strlen(result)+1;
  client.publish(outTopic, message);
}

int messageArrived(MQTT::MessageData& md) {
  char payload[MAX_MQTT_PACKET_SIZE];
  MQTT::Message &message = md.message;
  sprintf(payload,(char*)message.payload);
  payload[message.payloadlen] = '\0';
  Serial.println(payload);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  
  char* opCode = root["op_code"];
  char* tableName = root["query"]["table"];
  
  // create table 
  if((String) opCode == "c") {
     char* fieldString = root["query"]["fields"];
     char* input = malloc(sizeof(char) * strlen(fieldString)+1);
     strcpy(input,fieldString);
     createTable(tableName, input);
     sendMessageToTopic("Table created;EOR");
     return 0;
  }

  // describe table
  if((String) opCode == "d") {
    describeTable(tableName);
     return 0;
  }

  // insert into table
  if((String) opCode == "i") {
    char* fields = root["query"]["fields"];
    char* input = malloc(sizeof(char) * strlen(fields)+1);
    strcpy(input, fields);
    insertInto(tableName, input);
    sendMessageToTopic("Record inserted;EOR");
    Serial.println("Finished insert");
     return 1;
  }

  // select from table
  if((String) opCode == "s") {
    selectAll(tableName);
    return 2;
  }
  return -1;
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
  client.yield(100);
}


