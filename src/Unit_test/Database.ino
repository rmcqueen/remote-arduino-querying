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
  table->insert(*ptrRecordCount, tuple);
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
