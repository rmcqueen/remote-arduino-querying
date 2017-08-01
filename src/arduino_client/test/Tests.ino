#line 2 "A_client.ino"

//Check status of ethernet in setup
int ethernetStatus;

/**
@brief    Tests for a sucessful ethernet connection.
*/
test(ethernetConnection) 
{
  //ethernet connection
  if (ethernetStatus != 1)
    Serial.println("Failed Ethernet connection");
  assertEqual(1, ethernetStatus);
}

/**
@brief    Tests for a sucessful broker connection.
*/
test(connectTest)
{
  //if ethernet fails automatically fail connectionTest
  if(ethernetStatus != 1)
    assertEqual(0,1);
     
  //broker connection
  int statusId = connect();
  if (statusId != CONNECTION_OK)
    Serial.println("Failed Broker connection");
  assertEqual(CONNECTION_OK, statusId);
}

/**
@brief    Tests for a sucessful table creation.
*/
test(createTable)
{
  //single column
  assertNotEqual(createTable("test1", "name:s;"), DUPLICATE_TABLE_ERROR);

  //duplicate table creation
  assertEqual(createTable("test1", "name:s;"), DUPLICATE_TABLE_ERROR);

  char * table1 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test1")))->get(1);
  char * fieldString1 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test1")))->get(2);

  //single column test
  assertEqual("test1", table1);
  assertEqual("name:s;", fieldString1);


  //multiColumn
  assertNotEqual(createTable("test2", "name:s;age:i;"), DUPLICATE_TABLE_ERROR);
  char* table2 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test2")))->get(1);
  char* fieldString2 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test2")))->get(2);

  //multiColumn test
  assertEqual("test2", table2);
  assertEqual("name:s;age:i;", fieldString2);

  delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test1")));
  delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test2")));
}

/**
@brief    Tests for a sucessful tuple insertion.
*/
test(insertInto) 
{

  //Non existent table
  assertEqual(NULL_TABLE_ERROR, insertInto("tab", "value"));

  createTable("test", "name:s;");
  assertNotEqual(NULL_TABLE_ERROR, insertInto("test", "nameTest1:name;"));
  assertNotEqual(NULL_TABLE_ERROR, insertInto("test", "nameTest2:name;"));

  char * tuple1 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test")))->get(3);
  char * tuple2 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test")))->get(4);

  assertEqual("nameTest1:name;", tuple1);
  assertEqual("nameTest2:name;", tuple2);

  delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test")));
}

/**
@brief    Tests for a correct result set by selecting all tuples in a table.
*/
test(selectAll) 
{
  createTable("test3", "name:s;");
  insertInto("test3", "this is");
  insertInto("test3", "a multi");
  insertInto("test3", "tuple test");

  //Test after initial insert
  assertEqual("\ntest3\nname:s;\nthis is\na multi\ntuple test", selectAll("test3"));

  insertInto("test3", "and it passes");

  //Test after additional insert
  assertEqual("\ntest3\nname:s;\nthis is\na multi\ntuple test\nand it passes", selectAll("test3"));

  //Test invalid table
  assertEqual(TABLE_NOT_FOUND, selectAll("tab"));

  delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test3")));
}

/**
@brief    Tests for sucessfully formatted arriving MQTT messages
*/
test(messageArrived) 
{
  //create message
  char * result = "{\"op_code\":\"c\",\"query\":{\"table\":\"test4\",\"fields\":\"name:s;\"}}";
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = result;
  message.payloadlen = strlen(result) + 1;

  MQTTString topic = MQTTString_initializer;
  MQTT::MessageData *md = new MQTT::MessageData(topic, message);
  assertEqual(messageArrived( *md ), CREATE_TABLE_OP);

  result = "{\"op_code\":\"i\",\"query\":{\"table\":\"test4\",\"fields\":\"testing:name;\"}}";
  message.payload = result;
  message.payloadlen = strlen(result) + 1;

  md = new MQTT::MessageData(topic, message);
  assertEqual(messageArrived( *md ), INSERT_ELEMENT_OP);

  result = "{\"op_code\":\"s\",\"query\":{\"table\":\"test4\"}}";
  message.payload = result;
  message.payloadlen = strlen(result) + 1;

  md = new MQTT::MessageData(topic, message);
  assertEqual(messageArrived( *md ), SELECT_ALL_OP);
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("Connecting to Ethernet... (Timeout time is 60 seconds)");
  ethernetStatus = Ethernet.begin(mac);
  recordCount = 3;
  ptrRecordCount = &recordCount;
}

void loop() 
{
  Test::run();
  client.yield(100);
}
