

/******************************************************************************/
/**
@file     ArduinoMQTTClient.ino
@author   Dustin Olychuck
@brief    Implementation of an Arduino MQTT Client
*/
/******************************************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <IPStack.h>
#include <Countdown.h>
#define MQTTCLIENT_QOS2 1
#define MAX_INCOMING_Q0S2_MESSAGES 10 
#include <MQTTClient.h>
#include <IonDB.h>
#include <SD.h>


//tables 
Dictionary < int, ion_value_t > *colour = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 3);
Dictionary < int, ion_value_t > *car = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 3);
Dictionary < int, ion_value_t > *food = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 3);
Dictionary < int, ion_value_t > *team = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 5);

//master table
Dictionary < int, void* > *tables = new SkipList < int, void* > (key_type_numeric_signed, sizeof(int), sizeof(void*), 4);
const int tableSize = sizeof(*tables);

//Network settings
IPAddress ip(192, 168, 1, 68);
IPAddress server(127, 0, 0, 1);
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
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
  MQTT::Message &message = md.message;

 Cursor < int, void* > *my_cursor = ((Dictionary < int, ion_value_t >*) tables->get((char*)hash(message.payload)))->allRecords();
 char* result = malloc(sizeof(char));
  while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = realloc(result,sizeof(value) + sizeof(result)+2);
    strcat(result,"\n");
    strcat(result,value);
  }
  delete my_cursor;
  
  //Test sending outgoing message
  MQTT::Message message1;
  message1.qos = MQTT::QOS2;
  message1.retained = false;
  message1.dup = false;
  message1.payload = (void*)result;
  message1.payloadlen = strlen(result)+1;
  int statusId = client.publish("one", message1);
  free(result);
  if (statusId != 0)
  {
   //TODO test status
  } 
}


void connect()
{
  int port = 1883;
  int statusId = ipstack.connect(server, port);
  if (statusId != 1)
  {
   //TODO test status
  }

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.clientID.cstring = (char*) "Arduino1";
  statusId = client.connect(data);
  if (statusId != 0)
  {
    //TODO test status
  }
  
  statusId = client.subscribe(topic, MQTT::QOS2, messageArrived);   
  if (statusId != 0)
  {
    //TODO test status
  } 
}

int hash(char* str) {
    int result = 0;
    int power = 1;
    for(int i = (strlen(str)-1); i >= 0; i--) {
        int charValue = (str[i] - 96);
        result = result + (charValue * power);
        power = power*41;
    }
    return result;
}

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  connect();
  
  /* creating values. ion_value_t = string */
  ion_value_t red = (ion_value_t) "Red";
  ion_value_t orange = (ion_value_t) "Orange";
  ion_value_t blue = (ion_value_t) "Blue";
  
  ion_value_t honda = (ion_value_t) "Honda";
  ion_value_t ford = (ion_value_t) "Ford";
  ion_value_t mazda = (ion_value_t) "Mazda";
  
  ion_value_t fish = (ion_value_t) "Fish";
  ion_value_t burger = (ion_value_t) "Burger";
  ion_value_t pizza = (ion_value_t) "Pizza";
  
  ion_value_t david = (ion_value_t) "David Smekal";
  ion_value_t ryan = (ion_value_t) "Ryan Mcqueen";
  ion_value_t dustin = (ion_value_t) "Dustin Olychuck";
  ion_value_t spencer = (ion_value_t) "Spencer Macbeth";
  
  /* inserting key value pair into dictionary */
  colour->insert(1, red);
  colour->insert(2, orange);
  colour->insert(3, blue);
  
  car->insert(1, honda);
  car->insert(2, ford);
  car->insert(3, mazda);
  
  food->insert(1, fish);
  food->insert(2, burger);
  food->insert(3, pizza);
  
  team->insert(1, david);
  team->insert(2, ryan);
  team->insert(3, dustin);
  team->insert(4, spencer);

  //Master table inserts
  tables->insert(hash("colour"),colour);
  tables->insert(hash("car"),car);
  tables->insert(hash("team"),team);
  tables->insert(hash("food"),food);
  
  /* You should check the status on every operation to ensure good data integrity */
  if (err_ok != colour->last_status.error) {
    printf("Oh no! Something went wrong with my get operation\n");
  }
}

void loop()
{
  if (!client.isConnected())
    connect();
  client.yield(1000);
}
