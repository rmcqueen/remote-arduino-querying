#include <IonDB.h>
#include <SD.h> 
#include <SPI.h> 
#include <stdio.h>

Dictionary < int, ion_value_t > *aDict = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), 16, 4);
Dictionary < int, ion_value_t > *alsoDict = new SkipList < int, ion_value_t > (key_type_numeric_signed, sizeof(int), 16, 4);

int flushSkipList(Dictionary <int, ion_value_t > *dict) {
  Serial.println("Closing SkipList...");
  char *tableName = dict->get(1); // all dictionaries have the table name in the first
  FILE *dataFile;
  dataFile = fopen(tableName, "w+b");
  Cursor< int, void* > *tableCursor = dict->allRecords();
  int recordCount = 0;
  int *key = malloc(sizeof(int));
  char *value = malloc(16);
  while (tableCursor->next()) {
    int thisKey = tableCursor->getKey();
    memcpy(key, &thisKey, sizeof(int));
    memcpy(value, tableCursor->getValue(), 16);
    printf("%d,%s\n", *key, value);
    fwrite(key, sizeof(int), 1, dataFile);
    fwrite(value, 16, 1, dataFile);
    recordCount++;
  }
  free(key);
  free(value);
  fwrite(&recordCount, sizeof(int), 1, dataFile);
  fclose(dataFile);
  delete dict;
  delete tableCursor;
  return 0;
}

int openSkipList(Dictionary <int, ion_value_t > *dict, char* tableName) {
  Serial.println("Opening SkipList...");
  FILE *dataFile;
  dataFile = fopen(tableName, "r+b");
  int recordCount = 3;
  fseek(dataFile, -1 * sizeof(int), SEEK_END);
  fread(&recordCount, sizeof(int), 1, dataFile);
  printf("Record count read = %d\n", recordCount);
  fseek(dataFile, 0, SEEK_SET);

  for (int i = 0; i < recordCount; i++) {
    int *key = malloc(sizeof(int));
    char *value = malloc(16);
    fread(key, sizeof(int), 1, dataFile);
    fread(value, 16, 1, dataFile);
    printf("%d,%s\n", *key, value);
    dict->insert(*key, value);
  }
  fclose(dataFile);
  return 0;
}

void setup() {
  Serial.begin(9600);
  SD.begin(4);
  Serial.println("Setting up");
  // put your setup code here, to run once:
  aDict->insert(1, "val1");
  aDict->insert(2, "val2");
  aDict->insert(3, "val3");
  flushSkipList(aDict);
  openSkipList(alsoDict, "val1");
  Serial.println("Made it out alive!");
  Cursor< int, void* > *tableCursor = alsoDict->allRecords();
  int recordCount = 0;
  int *key = malloc(sizeof(int));
  char *value = malloc(16);
  while (tableCursor->next()) {
    printf("%d,%s\n", tableCursor->getKey(), tableCursor->getValue());
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
