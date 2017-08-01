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
 
### Default MQTT Topic Settings
 - Queries made from the web portal will be published the query to query/Arduino# for each checkbox selected. Arduinos are connected automatically subscribed to a topic of this format in `onConnect()` in `src/arudino_client/client.ino` The channel subscribed to can be found in the `topic` variable in this same file and it set to `query/Arduino1` by default.
 - Each the client.ino has a clientId specified. In the repo, it is set to Arduino1 by default.
 - If you want to query one arduino and not another, this must be changed for each Arduino. This channel is set in the `outTopic` variable in `src/arudino_client/client.ino` and is set to `result/Arduino1` by default.
 - Upon connection, the clients will publish the message `online` to the topic `status/Arduino1` by default. The status topic is listened to by the web portal which adds new checkboxes in the list of targetable arduinos upon receiving.
 - When an Arduino goes offline, a last-will-and-testemant message `offline` is published to `status/Arduino1` by default, trigger the removal of the associated checkbox from the webportal.
