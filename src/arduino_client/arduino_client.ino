#define DUPLICATE_TABLE_ERROR -1
#define NULL_TABLE_ERROR -1
#define TABLE_CREATED_OK 0
#define ELEMENT_INSERTED_OK 0
#define TABLE_NOT_FOUND "-1"
#define CONNECTION_OK 0
#define CONNECTION_FAILED -1
#define INVALID_QUERY -1
#define CREATE_TABLE_OP 0
#define INSERT_ELEMENT_OP 1
#define SELECT_ALL_OP 2
#define MQTTCLIENT_QOS2 1
#define MAX_INCOMING_Q0S2_MESSAGES 10

#include <ArduinoUnit.h>
#include <ArduinoJson.h>
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
#include <ArduinoUnit.h>

//Network settings
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
char hostname[] = "192.168.1.64";
EthernetClient c;
IPStack ipstack(c);

//MQTT Settings
const char* clientId = "Arduino1";
const char* topic = "query/Arduino1";
const char* outTopic = "result/Arduino1";
const char* statusTopic = "status/Arduino1";
const int MAX_MQTT_PACKET_SIZE = 512;
MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE> client = MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE>(ipstack);

//Tables
Dictionary < int, ion_value_t > *tables = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 4);
Dictionary < int, ion_value_t > *maxTableSize = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), 50, 4);
int tableSize = sizeof(*maxTableSize);
int* ptrRecordCount; //TODO filthy hack, clean me
int recordCount;

/**
  @brief    Connects the arduino client to the broker.
  @return   The resulting status based on the connection status.
*/
int connect()
{
  Serial.println("Connecting...");
  int port = 1883;
  ipstack.connect(hostname, port);

  //LWT message
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.willFlag = 1;
  data.clientID.cstring = clientId;
  data.will.topicName.cstring = statusTopic;
  data.will.qos = 2;
  data.will.retained = 1;
  data.will.message.cstring = (char*) "offline";
  client.connect(data);

  //Online flag
  char buf[8];
  sprintf(buf, "online");
  MQTT::Message message;
  message.qos = 2;
  message.retained = true;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf);
  client.publish(statusTopic, message);
  client.subscribe(topic, MQTT::QOS2, messageArrived);
  if (client.isConnected()) {
    Serial.println("Connected!");
    return CONNECTION_OK;
  }
  else {
    return CONNECTION_FAILED;
  }
}

/**
  @brief    Creates a new table in the database.

  @param    tableName
        The name for the table being created.
  @param    fieldString
        The information about the table schema.
  @return   A status describing the result of the creation.
*/
int createTable(char* tableName, char* fieldString)
{
  tables->get(stringToInt(tableName));
  if (!(err_item_not_found == tables->last_status.error))
    return DUPLICATE_TABLE_ERROR;

  // init dictionary
  Dictionary < int, ion_value_t > *table = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 3);
  table->insert(1, tableName);
  table->insert(2, fieldString);

  // save table in memory
  Dictionary< int, ion_value_t> *tableCache = malloc(tableSize);
  memset(tableCache, 0, tableSize);
  memcpy(tableCache, table, sizeof(*table));
  tables->insert(stringToInt(tableName), tableCache);
  //Serial.println("Finished creating table");
  return TABLE_CREATED_OK;
}

Dictionary < int, ion_value_t>* getTableByName(char* tableName)
{
  return ((Dictionary < int, ion_value_t>*) tables->get(stringToInt(tableName)));
}

Dictionary <int, ion_value_t>* getTableByTableName(char* tableName)
{
  // save table in memory
  void *tableAddress = tables->get(stringToInt(tableName));
  return ((Dictionary< int, ion_value_t>*) tableAddress);
}

Cursor<int, void*>* getSchemaCursorByTableName(char* tableName)
{
  // save table in memory
  void *tableAddress = tables->get((char*) stringToInt(tableName));
  return ((Dictionary< int, ion_value_t>*) tableAddress)->allRecords();
}

void describeTable(char* tableName)
{
  Cursor< int, void* > *my_cursor = getSchemaCursorByTableName(tableName);
  String result = "";
  while (my_cursor->next())
  {
    char* value = my_cursor->getValue();
    result = result + (String) result + "\n";
    Serial.println((String)value);
  }
}

/**
  @brief    Insert a value into a table.

  @param    tableName
        The name for the table being created.
  @param    tuple
        The tuple being inserted into the table.
  @returns  A status describing the result of the insertion.
*/
int insertInto(char* tableName, char* tuple)
{
  Dictionary < int, ion_value_t > *table = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)));
  if (err_item_not_found == tables->last_status.error) {
    return NULL_TABLE_ERROR;
  }
  Serial.println("Table gotten");
  Serial.println(tuple);
  table->insert(*ptrRecordCount, tuple);
  Serial.println("Record inserted...");
  *ptrRecordCount =  *ptrRecordCount + 1;
  return ELEMENT_INSERTED_OK;
}

/**
  @brief    Selects all tuples in a given table.

  @param    tableName
        The name for the table being queried.
  @returns  A complete set of all the tuples in the table or the status describing the result of the query.
*/
char* selectAll(char* tableName)
{
  Dictionary < int, ion_value_t > *table = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)));
  if (err_item_not_found == tables->last_status.error) {
    return TABLE_NOT_FOUND;
  }
  Cursor< int, void* > *my_cursor = table->allRecords();
  char* result = (char*) malloc(1);
  result[0] = '\0';
  while (my_cursor->next())
  {
    char* value = my_cursor->getValue();
    result = realloc(result, (sizeof(char) * (strlen(value) + strlen(result)) + 1));
    strcat(result, "\n");
    strcat(result, value);
  }
  delete my_cursor;
  return result;
}

/**
  @brief    Converts a char* into an integer.

  @param    str
        The char* being converted.
  @returns  An integer value based on the char*.
*/
int stringToInt(char* str)
{
  int result = 0;
  for (int i = (strlen(str) - 1); i >= 0; i--)
  {
    int charValue = (str[i] - 96);
    result = result + (charValue);
  }
  return result;
}

void printTableByName(char* tableName)
{
  Cursor< int, void* > *my_cursor = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)))->allRecords();
  while (my_cursor->next())
  {
    Serial.println((char*) my_cursor->getValue());
  }
  delete my_cursor;
}

/**
  @brief    Sends an MQTT message to the 'outTopic'.

  @param    result
        The contents of the outgoing message.
*/
void sendMessageToTopic(char* result)
{
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = (void*) result;
  message.payloadlen = strlen(result) + 1;
  client.publish(outTopic, message);
}

/**
  @brief    Recieves incoming MQTT messages containing query instructions.

  @param    md
          MQTT data from an incoming MQTT message.
  @returns  A status describing the result of the incoming query message.
*/
int messageArrived(MQTT::MessageData& md)
{
  char payload[MAX_MQTT_PACKET_SIZE];
  MQTT::Message &message = md.message;
  sprintf(payload, (char*)message.payload);
  payload[message.payloadlen] = '\0';
  //Serial.println(payload);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);

  char* opCode = root["op_code"];
  char* tableName = root["query"]["table"];

  // create table
  if ((String) opCode == "c") {
    char* fieldString = root["query"]["fields"];
    char* input = malloc(sizeof(char) * strlen(fieldString) + 1);
    strcpy(input, fieldString);
    createTable(tableName, input);
    sendMessageToTopic("Table created");
    return TABLE_CREATED_OK;
  }

  // describe table
  if ((String) opCode == "d") {
    describeTable(tableName);
    return 0;
  }

  // insert into table
  if ((String) opCode == "i") {
    char* fields = root["query"]["fields"];
    char* input = malloc(sizeof(char) * strlen(fields) + 1);
    strcpy(input, fields);
    insertInto(tableName, input);
    sendMessageToTopic("Record inserted");
    //Serial.println("Finished insert");
    return 1;
  }

  // select from table
  if ((String) opCode == "s") {
    char *result = selectAll(tableName);
    sendMessageToTopic(result);
    free(result);
    return 2;
  }
  return INVALID_QUERY;
}

/**
  @brief    Sets up all required elements on startup.
*/
void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);
  connect();
  recordCount = 3;
  ptrRecordCount = &recordCount;
}

/**
  @brief    Main code that is run repeatedly.
*/
void loop() {
  if (!client.isConnected()) {
    connect();
  }
  client.yield(100);
}




