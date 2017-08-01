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
  message.payloadlen = strlen(result)+1;
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
  sprintf(payload,(char*)message.payload);
  payload[message.payloadlen] = '\0';
  //Serial.println(payload);
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
     sendMessageToTopic("Table created");
     return TABLE_CREATED_OK;
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
    sendMessageToTopic("Record inserted");
     return 1;
  }

  // select from table
  if((String) opCode == "s") {
    char *result = selectAll(tableName);
    sendMessageToTopic(result);
    free(result);
    return 2;
  }
  return INVALID_QUERY;
}
