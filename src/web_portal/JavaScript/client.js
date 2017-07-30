/**
 * Web portal
 */

// network variables
const hostname = "127.0.0.1";
const port = 8080;
var connectedClients = [];

// Create a client instance
var client = new Paho.MQTT.Client(hostname, Number(port), "web_portal");

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

//data to be added to csv file.
var csvInputData = "";

// connect the client
client.connect({
    onSuccess: onConnect
});


/*
* Purpose: when the client is first started, begin listening to the two main
* topics of this project. The status channel indicating which Arduinos are
* available to be picked from, and the queries results that are being sent back
*/
function onConnect() {
    console.log('Connected!');

    $('#statusBox').html('Connected');
    $('#statusBox').attr('class', 'label alert-success');

    client.subscribe("status/#", {
        qos: 2
    });
    client.subscribe("result/#", {
        qos: 2
    });
}


/*
* Purpose: if a connection is lost to the broker, print the error message to the
* console to inform the user what caused the issue.
*
* @param Object responseObject a response indicating what went wrong with the
* request
*/
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.log("onConnectionLost:" + responseObject.errorMessage);
    }

    $('#statusBox').html('Disconnected');
    $('#statusBox').attr('class', 'label label-warning');
}


/*
* Purpose: when a new message is received from the broker, determine the clientId
* of the Arduino that is came from, as well as the messages desired topic.
* sent in the form of: DESIRED_TOPIC/CLIENT_ID
*
* @param Message message a MQTT message object containing information about the
* message received. Contains the destination/topic, and the string of information
* it sent back. See mqttws31.js for more information
*/
function onMessageArrived(message) {
    destination = message.destinationName.split("/")[0];
    clientId = message.destinationName.split("/")[1];

    // If the topic is status, we know an Arduino is either connecting, or
    // has been disconnected
    if (destination == "status") {
        if (message.payloadString == "online" && $.inArray(clientId, connectedClients) == -1) {
            connectedClients.push(clientId);
            createCheckBoxes(clientId);
        }

        if (message.payloadString == "offline" && $.inArray(clientId, connectedClients) != -1) {
            connectedClients.splice($.inArray(clientId, connectedClients), 1);
            removeCheckboxes(clientId);
        }
    }

    if (destination == "result") {
        displayResults(clientId, message.payloadString);
        fillProgressBar(1, 100);
    } else {
        console.log("No results");
    }
}

/*
* Purpose: When a message has been delivered, the progress bar will fill up.
* A message being delivered means the processing that this client does is finished,
* and the message has been sent to the server.
*
 * @param Message message a MQTT message object containing information about the
 * message received. Contains the destination/topic, and the string of information
 * it sent back. See mqttws31.js for more information
 */
function onMessageDelivered(message) {
    fillProgressBar(1, 66);
}


/*
* Purpose: the purpose of this function is to send the user's query to their
* specified Arduinos (as outlined by the checked checkboxes). The request is
* sent to a server for parsing.

* @return null null if there is no client connected, returns a null as it
* cannot process the request
*/
function send() {
	// Get the query from the query box
    var userQuery = $('#publish').val();
    var selectedArduinos = [];

	// Reset the values within the text area
    $('#publish').val("");

	// Return if there is no available MQTT client to use
    if (!client) {
        return;
    }

    // Go over each checked checkbox, and append them to a list
    $('input[type=checkbox]').each(function() {
        if (this.checked) {
            selectedArduinos.push(this.id);
        }
    });
	// Perform an AJAX request to the server for query parsing
    $.ajax({
        url: "http://localhost:3000/publish_query",
        type: "get",
        data: {
            queryString: userQuery,
            targets: selectedArduinos
        },
        success: function(result) {
            alert('Success');
        },
        error: function() {
            console.log('Error');
        }

    });

    $('#successBox').show();
    $('#successBox').fadeOut(3000);
    fillProgressBar(1, 33);
}


/*
* Purpose: appends a new line between each new message received from the broker
* to enhance readability for the user when they interpret their query results
*/
function appendcsvInputData(message) {
    csvInputData += message + "\n";
}


/*
* Purpose: enables a user to download the results of their query into a CSV
* formatted file as 'results.csv'
*/
function CSV_download() {
    var data = csvInputData.split("\n");
    var csvData = "data:text/csv;charset=utf-8,";

    data.forEach(function(element, index) {
        if (index < (data.length - 1)) {
            csvData += element + "\n";
        } else {
            csvData += element;
        }
    });

    var encodedUri = encodeURI(csvData);
    var download = document.createElement("a");
    download.setAttribute("href", encodedUri);
    download.setAttribute("download", "results.csv");
    download.click();
}


/*
* Purpose: this function creates checkboxes for the user to select from. It is
* based on whether or not there are any Arduinos connected to the INTRAnet
*
* @param string name represents the clientId of the Arduino, and creates a new
* checkbox wrapped in a div
*/
function createCheckBoxes(name) {
    container = $('#checkbox');
    $("<input />", {
        type: "checkbox",
        id: name,
        value: name
    }).appendTo(container);
    $("<label />", {
        "for": name,
        text: name
    }).appendTo(container);
    $("<div />").appendTo(container);
}


/*
* Purpose: removes any checkboxes from the main page if the connection between
* an Arduino and the broker is disconnected, or lost due to an Arduino crashing
*
* @param string name represents the clientId of the Arduino, and removes the
* checkbox that corresponds to it
*/
function removeCheckboxes(name) {
    $("#" + name).remove();
    $("label[for=" + name + "]").remove();
}


/*
* Purpose: displays the results from the user's query into the "results" area
* of the web page.
*
* @param string clientId the id that represents the Arduino the query result
* came from
*
* @param string message the payloadString that was obtained from the user's query
* should show what values were requested in the database (if they exist)
*/
function displayResults(clientId, message) {
    const newResultEntry = `${clientId}: {csv: ${message}},`
    const currentResultString = $('#resultArea').text();
    $('#resultArea').val(`${newResultEntry}${currentResultString}`);
    appendcsvInputData("\n" + clientId + ":\n" + message);
}

/*
* Purpose: this function fills up the progress bar in 3 steps. The first 1/3
* of the bar gets filled after a message has been sent from the web client.
* The second 1/3 of the bar gets filled up once a message has been delivered
* to the server. Once a message has been received and written out on the
* web client, it will fill up the whole progress bar.
* After 2 seconds, the progress bar resets itself.
*
* @param int start the position the progress bar will start to fill at.
*
* @param int finish the position the progress bar will finish filling at.
*/
function fillProgressBar(start, finish) {

    var startingWidth = start;
    var progressBar = document.getElementById("progressBar");
    var clear = setInterval(stopFill, 1);

    function stopFill() {
        if (startingWidth >= finish) {
            clearInterval(clear);
        } else {
            startingWidth++;
            progressBar.style.width = startingWidth + '%';
            if (startingWidth <= 33) {
                progressBar.innerHTML = "Message Sent...";
            }
            else if (startingWidth <= 66) {
                progressBar.innerHTML = "Message Delivered...";
            }
            else if (startingWidth > 66) {
                progressBar.innerHTML = "Message Received";
                progressBar.className = "progress-bar progress-bar-success";

                setTimeout(function () {
                    progressBar.style.width = 0 + '%';
                    progressBar.innerHTML = ""
                }, 2000);

            }

        }
    }
}
