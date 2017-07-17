# project-2-ion-ion-field-network

### Setup Instructions
 - The Moquette MQTT broker software needs to be installed to use this project and is available for free here https://projects.eclipse.org/projects/iot.moquette
 - Once Moquette is installed, replace the moquette.conf file included with the moquette.conf availble in the config folder of this repository
 - The Arduino client .ino builds can be ran on desired Arduino boards with the free Arduino IDE. These files
 - Before deploying these builds, the users must configure their hostname. The hostname can be obtained in many ways, though an easy one is vai the DhcpAddressPrinter.ino script available in the Arduino Ethernet examples
 - The web portal is plain old JavaScript and should run in any javascript-enabled browser, though chrome is the only which has been tested
 - The query-parsing sever can be started from the query-parser directory in the terminal with `yarn start`

### Known Gotchas
 - Web portal failing to connect? Make sure the relevant ports (:1883 and :8080) are all free and clean
 - Arduino failing to connect? Make sure your the hostname variable used is your ip4 address
 - The same Arduino sometime works and sometimes doesnt? Try changing your mac addres declared a little bit - the router may still think its in use from a previous run
 - MQTT communications not playing nice in general? Use the config/moquette.conf as your moquette configuration
 - Arduino IDE versions that are < 1.8.3 throw static analysis exceptions for a void pointer being unable to convert to a char pointer. Untested in early versions of 1.8.X