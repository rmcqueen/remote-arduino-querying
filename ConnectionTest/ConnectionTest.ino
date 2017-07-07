/******************************************************************************/
/**
@file     ConnectionTest.ino
@brief    Used to test prototype for automatic client status online/offline
*/
/******************************************************************************/
#define MQTTCLIENT_QOS2 1
#include <SPI.h>
#include <Ethernet.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

//Network settings
IPAddress ip(192, 168, 1, 30);
IPAddress server(192, 168, 1, 66);
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x03
};

//MQTT client setup
EthernetClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

//incoming message buffer
char inMsgBuffer[128];

//subscribe topic
const char* topic = "arduino1"; 

void messageArrived(MQTT::MessageData& md)
{
  //Test incoming message
  MQTT::Message &message = md.message;
  sprintf(inMsgBuffer, "Message Payload %s\n", (char*)message.payload);
  Serial.print(inMsgBuffer);
  
  //Test sending outgoing message
  MQTT::Message PubMessage;// = md.message;
  char buf[128];
  sprintf(buf,"Arduino message");
  PubMessage.qos = MQTT::QOS2;
  PubMessage.retained = false;
  PubMessage.dup = false;
  PubMessage.payload = (void*)buf;
  PubMessage.payloadlen = strlen(buf)+1;
  client.publish("one", PubMessage);
}

void connect()
{
  int port = 1883;
  char hostname[] = "192.168.1.66";
  ipstack.connect(hostname, port);
  
  //************************************ONLINE/OFFLINE detecting code*********************************************************** 
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
  client.publish("status/Arduino1", message);                               //Set to status/<ID>
 //************************************ONLINE/OFFLINE detecting code************************************************************
 
  client.subscribe(topic, MQTT::QOS2, messageArrived);   
}

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac); 
  connect();
}

void loop()
{
  if (!client.isConnected())
    connect();
  client.yield(1000);
}
