#line 2 "A_client.ino"
test(connectTest) 
{
  int statusId = connect();
  assertEqual(0,statusId);
}

test(createTable)
{ 
   //single column
  assertNotEqual(createTable("test1","name:s;"),-1);
  char * table1 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test1")))->get(1);
  char * fieldString1 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test1")))->get(2);

  //single column test
  assertEqual("test1",table1);
  assertEqual("name:s;",fieldString1);

  
  //multiColumn
  assertNotEqual(createTable("test2","name:s;age:i;"),-1);
  char* table2 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test2")))->get(1);
  char* fieldString2 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test2")))->get(2);

  //multiColumn test
  assertEqual("test2",table2);
  assertEqual("name:s;age:i;",fieldString2);

  //duplicate table creation
  assertEqual(createTable("test1","name:s;"),-1);
  
  delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test1")));
  delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test2")));
}

test(insertInto) {
     //TODO*********insert invalid value*********
      
     //Non existant table
     assertEqual(-1,insertInto("tab","value"));
     
     createTable("test","name:s;");
     insertInto("test","nameTest1:name;");
     insertInto("test","nameTest2:name;");
    
     char * tuple1 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test")))->get(3);
     char * tuple2 = (char*) ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test")))->get(4);

     assertEqual("nameTest1:name;",tuple1);
     assertEqual("nameTest2:name;",tuple2);

     delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test")));
}


test(selectAll) {
    createTable("test3","name:s;");
    insertInto("test3","this is");
    insertInto("test3","a multi");
    insertInto("test3","tuple test");

    //Test after initial insert
    assertEqual("\ntest3\nname:s;\nthis is\na multi\ntuple test",selectAll("test3"));

    insertInto("test3","and it passes");

    //Test after additional insert
    assertEqual("\ntest3\nname:s;\nthis is\na multi\ntuple test\nand it passes",selectAll("test3"));

    //Test invalid table
    assertEqual("-1",selectAll("tab"));

    delete ((Dictionary < int, ion_value_t >*) tables->get(stringToInt("test3")));
}

test(messageArrived) {
  //create message
  char * result = "{\"op_code\":\"c\",\"query\":{\"table\":\"test4\",\"fields\":\"name:s;\"}}";
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = result;
  message.payloadlen = strlen(result)+1;

  MQTTString topic = MQTTString_initializer;
  MQTT::MessageData *md = new MQTT::MessageData(topic,message);
  assertEqual(messageArrived( *md ),0);

  result = "{\"op_code\":\"i\",\"query\":{\"table\":\"test4\",\"fields\":\"testing:name;\"}}";
  message.payload = result;
  message.payloadlen = strlen(result)+1;

  md = new MQTT::MessageData(topic,message);
  assertEqual(messageArrived( *md ),1);

  result = "{\"op_code\":\"s\",\"query\":{\"table\":\"test4\"}}";
  message.payload = result;
  message.payloadlen = strlen(result)+1;

  md = new MQTT::MessageData(topic,message);
  assertEqual(messageArrived( *md ),2);
}


void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);  
  recordCount = 3;
  ptrRecordCount = &recordCount;
}


void loop() {
  Test::run();
  client.yield(100);
}

