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
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22

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
#include <ArduinoUnit.h>
#include <DHT.h> //only if you have a DHT sensor, otherwise remove this
