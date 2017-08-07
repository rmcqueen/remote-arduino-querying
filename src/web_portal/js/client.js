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
    if (destination === "status") {
        if (message.payloadString == "online" && $.inArray(clientId, connectedClients) == -1) {
            connectedClients.push(clientId);
            createCheckBoxes(clientId);
        }

        if (message.payloadString === "offline" && $.inArray(clientId, connectedClients) != -1) {
            connectedClients.splice($.inArray(clientId, connectedClients), 1);
            removeCheckboxes(clientId);
        }
    } else if (destination === "result") {
        updateLoadingIconText('Received');
        displayResults(clientId, message.payloadString);
    } else {
        updateLoadingIconText('Error');
    }
}

/*
* TODO: remove this? it doesn't appear to be used anywhere.
* Purpose: When a message has been delivered, the success alert text will update.
* A message being delivered means the processing that this client does is finished,
* and the message has been sent to the server.
*
 * @param Message message a MQTT message object containing information about the
 * message received. Contains the destination/topic, and the string of information
 * it sent back. See mqttws31.js for more information
 */
function onMessageDelivered(message) {
    // TODO: This needs to be refactored since the message param is not being used
    updateLoadingIconText('Delivered')
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

    // Note: this cannot be a falsy check (!valid....()) it must check type.
    if(validQueryEntered() === false) {
        return;
    }
    
    // Note: this cannot be a falsy check (!valid....()) it must check type.
    if(validArduinoSelected(selectedArduinos) === false) {
        return;
    }
    // Reset the values within the text area
    $('#publish').val('');

    // Store the user's query in the query history text area
    $('#queryhistory').append(userQuery + '\n');
	// Perform an AJAX request to the server for query parsing
    $.ajax({
        url: "http://localhost:3000/publish_query",
        type: "get",
        data: {
            queryString: userQuery,
            targets: selectedArduinos
        },
        success: function(result) {
        },
        error: function(err) {
            updateLoadingIconText('Error');
            console.log('Error in send()');
            console.log(err);
        }

    });
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
    $("#resultArea").append(newResultEntry);
    appendcsvInputData("\n" + clientId + ":\n" + message);
}


/*
* Purpose: update the text on the success banner to give the user an indication of where their query is at
*/
function updateLoadingIconText(status) {
    if (status === 'Delivered') {
        $('.alert').find('strong').html('Message Delivered! awaiting results...');
    } else if (status === 'Received') {
        $('.alert').find('strong').html('Message Received!');
        $('.loader').find('svg').remove();
        $('.loader').find('path').remove();
        $('.loader').append('<span class="glyphicon glyphicon-ok" aria-hidden="true" style="margin: 0 auto;"></span>');
    } else {
        $('.header_row').find('.alert').replaceWith(
                '<div class="alert alert-danger alert-dismissable fade in">' +
                '<span class="glyphicon glyphicon-remove" aria-hidden="true" style="margin: 0 auto;"></span>' +
                '<button type="button" class="close" data-dismiss="alert" aria-label="close">&times;</button>' +
                '<strong>Error:</strong> Error in query. Ensure the syntax is correct and try again.' +
                '</div>');
    }
}


/*
* Purpose: checks whether or not text has been entered into the textbox to reduce potential errors
*
* @return bool  indicates whether or not text has been entered in the textbox
*/
function validQueryEntered() {
    $('.header_row').empty();
    if ($('#publish').val() === '') {
        $('.header_row').append('<div class="alert alert-danger alert-dismissable fade in">' +
        '<button type="button" class="close" data-dismiss="alert" aria-label="close">&times;</button>' +
        '<strong>Error:</strong> A query must be entered.' +
        '</div>');
    return false;
    }
    else {
        return true;
    }
}


/*
* Purpose: this function checks whether or not a user has selected any arduinos before allowing them to proceed
* with sending their query
*
* @param String[] selectedArduinos  indicates which arduinos are selected
@ return boolean    whether or not the Arduino selected is valid (if there are any selected)
*/
function validArduinoSelected(selectedArduinos) {
    $('.header_row').empty();
    if (selectedArduinos.length === 0) {
        $('.header_row').append('<div class="alert alert-danger alert-dismissable fade in">' +
        '<button type="button" class="close" data-dismiss="alert" aria-label="close">&times;</button>' +
        '<strong>Error:</strong> An Arduino must be selected.' +
        '</div>');
        return false;
        }

    else {
        $('.header_row').append(
            '<div class="alert alert-success alert-dismissable">' +
                '<div class="loader loader--style3" title="2">' +
                '<svg version="1.1" id="loader-1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"' +
                    'width="40px" height="40px" viewBox="0 0 50 50" style="enable-background:new 0 0 50 50;" xml:space="preserve">' +
                '<path fill="#000" d="M43.935,25.145c0-10.318-8.364-18.683-18.683-18.683c-10.318,0-18.683,8.365-18.683,18.683h4.068c0-8.071,6.543-14.615,14.615-14.615c8.072,0,14.615,6.543,14.615,14.615H43.935z">' +
                '<animateTransform attributeType="xml"' +
                    'attributeName="transform"' +
                    'type="rotate"' +
                    'from="0 25 25"' +
                    'to="360 25 25"' +
                    'dur="0.6s"' +
                    'repeatCount="indefinite"/>' +
                '</path>' +
                '</svg>' +
                '</div>' +
                '<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>' +
                '<strong>Message Sent! awaiting results...</strong>' +
            '</div>');
        return true;
    }
}
