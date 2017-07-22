//Network
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x05
};
EthernetClient c;
IPStack ipstack(c);


//MQTT 
const char* topic = "query/Arduino1";
const char* outTopic = "result/Arduino1";
const int MAX_MQTT_PACKET_SIZE = 512;
MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE> client = MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE>(ipstack);

unsigned long lastMillis = 0;

//Tables
Dictionary < int, void* > *tables = new SkipList < int, void* > (key_type_numeric_signed, sizeof(int), sizeof(void*), 4);
Dictionary < int, void* > *maxTableSize = new SkipList < int, void* > (key_type_numeric_signed, sizeof(int), 50, 4);
int tableSize = sizeof(*maxTableSize);
int* ptrRecordCount; //TODO filthy hack, clean me
int recordCount;

int connect() {
  Serial.println("Connecting...");
  int port = 1883;
  char hostname[] = "192.168.1.100"; // CHANGE ME TO YOUR HOSTNAME
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
  client.publish("status/Arduino1", message);      
  client.subscribe(topic, MQTT::QOS2, messageArrived);
  if(client.isConnected()) {
    Serial.println("Connected!");
    return 0;
  } else {
    Serial.println("Not yet connected...");
    return -1;
  }
}

// main
/*
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
*/

