# Ion-Field-Network

### Library Requirements
1. Arduino:
    - IonDB v1.2.0 (https://github.com/iondbproject/iondb)
    - ArduinoJson v5.11.1 (https://github.com/bblanchon/ArduinoJson)
    - Arduino MQTT v1.4.1 (https://github.com/monstrenyatko/ArduinoMqtt)
    - ArduinoUnit (for tests) v2.2.0 (https://github.com/mmurdoch/arduinounit)

2. MQTT Broker
    - Moquette MQTT Broker v0.10 (https://projects.eclipse.org/projects/iot.moquette)

3. NodeJS
    - NodeJS v6.1 or higher (https://nodejs.org/en/download/)
    - Yarn v0.27.5 or higher (https://yarnpkg.com/lang/en/docs/install/)

### Setup Instructions
 1. Install Moquette (URL is in the Library Requirements section)

 2. Once Moquette is installed, replace the moquette.conf file in `/path/to/moquette/location/distribution-0.10-bundle-tar/config/` with the moquette.conf provided in the config folder of this repository. For a unix based operating system use:
 ```
 mv /path/to/repo/conf/moquette.conf /path/to/moquette/location/distribution-0.10-bundle-tar/config/
 ```

3. Start Moquette by navigating to the bin folder included in the installation location you chose, and run it based on your operating system. There is a script for both Unix based systems, and Windows, however, this project has only been tested on Unix systems.
```
cd /path/to/moquette/location/distribution-0.10-bundle-tar/bin
./moquette.sh
```

3. Navigate to the project's root directory and run Yarn to install the necessary dependencies for NodeJS, as well as start it:
```
cd ~/path/to/repo/location/src/query_parser
yarn start
```

4. Navigate to the `web_portal` directory and open `main.html` with Chrome, or Firefox (These are the only two browsers Ion-Field-Network has been tested on).

5. Open the Arduino IDE, and open the `client.ino` file located in `/path/to/repo/src/arduino_client/client/client.ino`
 - Ensure all dependencies are installed, and the IP address/MAC address are configured to your system's specifications

6. Build the `client.ino` file

7. Congratulations! You should now see Arduino1 in the "Available Arduinos" section on the web portal.

### Default MQTT Topic Settings
 - Queries made from the web portal will be published the query to `query/ARDUINO_NAME_HERE` for each checkbox selected. Arduinos are connected automatically subscribed to a topic of this format in `onConnect()` in `src/arudino_client/client.ino` The channel subscribed to can be found in the `topic` variable in this same file and it set to `query/Arduino1` by default.
 - Each the client.ino has a clientId specified. In the repo, it is set to `Arduino1` by default.
 - If you want to query one arduino and not another, this must be changed for each Arduino. This channel is set in the `outTopic` variable in `src/arudino_client/client.ino` and is set to `result/Arduino1` by default.
 - Upon connection, the clients will publish the message `online` to the topic `status/Arduino1` by default. The status topic is listened to by the web portal which adds new checkboxes in the list of targetable arduinos upon receiving.
 - When an Arduino goes offline, a last-will-and-testemant message `offline` is published to `status/Arduino` by default, trigger the removal of the associated checkbox from the webportal.

### Tests
- All of the tests are located in the folder respective to the components being tested. For example, if you wish to view tests for the Arduino code, you can find it located in `src/arduino_client/test/`. This project is heavily tested, however, it still lacks in the area of regression, and some integration testing. Please refer to the Contributing section if you wish to provide additional tests, or have feedback on the current tests.

### Known Gotchas
 - Web portal failing to connect? Make sure the relevant ports (:1883 and :8080) are all free and clean
 - Arduino failing to connect? Make sure your the hostname variable used is your IPv4 address
 - The same Arduino sometime works and sometimes doesnt? Try changing your MAC address a little bit - the router may still think it is in use from a previous run.
 - MQTT communications not playing nice in general? Use the config/moquette.conf as your moquette configuration
 - Arduino IDE versions that are < 1.8.3 throw static analysis exceptions for a void pointer being unable to convert to a char pointer. Untested in early versions of 1.8.X
 
 ### Future Features
 1. A query tracking system is under development in the `query-tracker` branch, and uses Postgres to store the query, as well as assign an ID to a query due to the nature of this project. It is expected the results of a query will take a while to perform, and a way to monitor of the status is required.
    - Would be nice to send an email indication to the user when their query reaches a new stage (i.e, from pending to processing)

2. Syntax Highlighting
    - Indicate to the user ahead of time if there is an issue with the general syntax of their query

3. Lenient syntax
    - Allow for upper/lowercase SQL keywords (select, insert, etc.)
    - Parenthesis placement is very finnicky at this moment in time

### Contributing
- Contributions are <b>very</b> much appreciated. If there is a feature, or change you feel should be included make a pull request with this feature.
- If issues are discovered, please make an issue ticket indicating what the problem is, what category it applies to, and what caused the issue. An example of a issue ticket submission may look like this:

Title:
```
Query Parser: Invalid Parentheses Checking
```
Description:
```
Brief: When parsing parentheses, they are not parsed when there is no space between keywords

Query: CREATE TABLE dogs(name, age) VALUES ('ralph', 3);
```

