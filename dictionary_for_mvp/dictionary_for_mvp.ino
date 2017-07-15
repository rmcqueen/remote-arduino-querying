#include <IonDB.h>
#include <SD.h>

void
setup(
) {
  SD.begin(13);
  Serial.begin(9600);
  /* Creating the dictionary */
  Dictionary < int, ion_value_t > *colour = new FlatFile < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 5);
  Dictionary < int, ion_value_t > *car = new FlatFile < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 5);
  Dictionary < int, ion_value_t > *food = new FlatFile < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 5);
  Dictionary < int, ion_value_t > *team = new FlatFile < int, ion_value_t > (key_type_numeric_signed, sizeof(int), sizeof(ion_value_t), 4);
  
  /* creating values. ion_value_t = string */
  ion_value_t red = (ion_value_t) "Red";
  ion_value_t orange = (ion_value_t) "Orange";
  ion_value_t blue = (ion_value_t) "Blue";
  ion_value_t yellow = (ion_value_t) "Yellow";
  ion_value_t green = (ion_value_t) "Green";
  
  ion_value_t honda = (ion_value_t) "Honda";
  ion_value_t ford = (ion_value_t) "Ford";
  ion_value_t mazda = (ion_value_t) "Mazda";
  ion_value_t tesla = (ion_value_t) "Tesla";
  ion_value_t acura = (ion_value_t) "Acura";
  
  ion_value_t fish = (ion_value_t) "Fish";
  ion_value_t burger = (ion_value_t) "Burger";
  ion_value_t pizza = (ion_value_t) "Pizza";
  ion_value_t taco = (ion_value_t) "Taco";
  ion_value_t sushi = (ion_value_t) "Sushi";
  
  ion_value_t david = (ion_value_t) "David Smekal";
  ion_value_t ryan = (ion_value_t) "Ryan Mcqueen";
  ion_value_t dustin = (ion_value_t) "Dustin Olychuck";
  ion_value_t spencer = (ion_value_t) "Spencer Macbeth";
 
  /* inserting key value pair into dictionary */
  colour->insert(1, red);
  colour->insert(2, orange);
  colour->insert(3, blue);
  colour->insert(4, yellow);
  colour->insert(5, green);
  
  car->insert(1, honda);
  car->insert(2, ford);
  car->insert(3, mazda);
  car->insert(4, tesla);
  car->insert(5, acura);
  
  food->insert(1, fish);
  food->insert(2, burger);
  food->insert(3, pizza);
  food->insert(4, taco);
  food->insert(5, sushi);
  
  team->insert(1, david);
  team->insert(2, ryan);
  team->insert(3, dustin);
  team->insert(4, spencer);
  
/* creating a cursor to loop through all the records in the dictionary */
Cursor < int, ion_value_t > *colour_cursor = colour->allRecords();
Cursor < int, ion_value_t > *car_cursor = car->allRecords();
Cursor < int, ion_value_t > *food_cursor = food->allRecords();
Cursor < int, ion_value_t > *team_cursor = team->allRecords();
  
  printf("Colour table: \n");
  while (colour_cursor->next()) {
      int key   = colour_cursor->getKey();
      int value = colour_cursor->getValue();
    printf("%d -> %s\n", key, value);
  }
  
  printf("\nCar table: \n");
  while (car_cursor->next()) {
      int key   = car_cursor->getKey();
      int value = car_cursor->getValue();
    printf("%d -> %s\n", key, value);
  }
  
  printf("\nFood table: \n");
  while (food_cursor->next()) {
      int key   = food_cursor->getKey();
      int value = food_cursor->getValue();
    printf("%d -> %s\n", key, value);
  }
  
  printf("\nTeam table: \n");
  while (team_cursor->next()) {
      int key   = team_cursor->getKey();
      int value = team_cursor->getValue();
    printf("%d -> %s\n", key, value);
  }

  delete colour_cursor;
  delete car_cursor;
  delete food_cursor;
  delete team_cursor;
  
  
}
void
loop(
) {}
