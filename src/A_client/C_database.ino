int createTable(char* tableName, char* fieldString) {
  Serial.println("createTable called");
  tables->get(stringToInt(tableName));
  if(!(err_item_not_found == tables->last_status.error))
     return -1;

  // init dictionary
   Dictionary < int, ion_value_t > *table = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 3);
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
   Dictionary < int, ion_value_t > *table = ((Dictionary < int, ion_value_t >*) tables->get(stringToInt(tableName)));
   if(err_item_not_found == tables->last_status.error)
      return "-1";
   Cursor< int, void* > *my_cursor = table->allRecords();
   char* result = (char*) malloc(1);
   result[0] = '\0';
   while (my_cursor->next()) {
    char* value = my_cursor->getValue();
    result = realloc(result,(sizeof(char) * (strlen(value) + strlen(result))+1));
    strcat(result,"\n");
    strcat(result, value);
  }
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
