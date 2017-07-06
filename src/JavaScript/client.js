/**
 * Web portal
 */

// network variables
var hostname = "127.0.0.1";
var port = 8080;
var connectedClients = [];
// Create a client instance
var client = new Paho.MQTT.Client(hostname, Number(port), "clientId");

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

//data to be added to csv file.
var csvInputData = "";

// connect the client
client.connect({
    onSuccess: onConnect
});

// Connection
function onConnect() {
    client.subscribe("status/#", {
        qos: 2
    });
    client.subscribe("query/#", {
        qos: 2
    });
}


// Lost connection
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.log("onConnectionLost:" + responseObject.errorMessage);
    }
}


// Arriving messages
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
    } else {
        displayResults(clientId, message.payloadString);
    }
}


//Temporary until real query works properly
function parse(s) {
    var result = s.split(" ");
    return result[result.length - 1];
}


//Sends publish message
function send(publishClientMessage) {
    if (!client) {
        return;
	}

    // refresh
    document.getElementById("publish").value = "";

    var message = new Paho.MQTT.Message(parse(publishClientMessage)); //Temporary until real query works properly
    message.qos = 2;

    $("input[type=checkbox]").each(function() {
        if (this.checked) {
            message.destinationName = this.id;
			console.log("Sending message");
            client.send(message);
        }
    });
}


//Adds data to be stored in csv
function appendcsvInputData(message)
{
    csvInputData += message + "\n";
}


//Downloads csv file
function CSV_download()
{
    var data = csvInputData.split("\n");
    var csvData = "data:text/csv;charset=utf-8,";

    data.forEach(function(element, index) {
        if (index < (data.length - 1)) {
            csvData += element + "\n";
        }
        else {
            csvData += element;
        }
    });

    var encodedUri = encodeURI(csvData);
    var download = document.createElement("a");
    download.setAttribute("href", encodedUri);
    download.setAttribute("download", "results.csv");
    download.click();
}


function createCheckBoxes(name)
{
    container = $("#checkbox");
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


function removeCheckboxes(name)
{
    $("#" + name).remove();
    $("label[for=" + name + "]").remove();
}

// Displays the results into the result window
function displayResults(clientId, message)
{
    if (clientId) {
        document.getElementById("resultArea").innerHTML = "Results from " +
            clientId + ": " + message;
        appendcsvInputData("\n" + clientId + ": " + message);
    }
}
