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
    return CONNECTION_OK;
  } 
  else {
    return CONNECTION_FAILED;
  }
}
