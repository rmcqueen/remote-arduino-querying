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
	onSuccess : onConnect
});

// Connection
function onConnect() {
	console.log("onConnect");
	client.subscribe("clients", {
		qos : 2
	});
	client.subscribe("one", {
		qos : 2
	});
	client.subscribe("two", {
		qos : 2
	});
	client.subscribe("three", {
		qos : 2
	});
	client.subscribe("four", {
		qos : 2
	});
	client.subscribe("status/#", {
		qos : 2
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
	destination = message.destinationName.split('/')[0]
	clientId = message.destinationName.split('/')[1];
	console.log(clientId);

	if (destination == "status") {
		if(message.payloadString == "online" && $.inArray(clientId, connectedClients) == -1) {
			connectedClients.push(clientId);
			createCheckBoxes(clientId);
		}

		if(message.payloadString == "offline" && $.inArray(clientId, connectedClients) != -1) {
			connectedClients.splice($.inArray(clientId, connectedClients), 1);
			removeCheckboxes(clientId);
		}
	}
	if (message.destinationName == "one") {
		document.getElementById("resultArea").innerHTML = "Results Arduino1:"
				+ message.payloadString;
		appendcsvInputData("\nArduino1:" + message.payloadString);
	}
	if (message.destinationName == "two") {
		document.getElementById("resultArea").innerHTML = "Results Arduino2:"
				+ message.payloadString;
		appendcsvInputData("\nArduino2:" + message.payloadString);
	}
	if (message.destinationName == "three") {
		document.getElementById("resultArea").innerHTML = "Results Arduino3:"
				+ message.payloadString;
		appendcsvInputData("\nArduino3:" + message.payloadString);
	}
	if (message.destinationName == "four") {
		document.getElementById("resultArea").innerHTML = "Results Arduino4:"
				+ message.payloadString;
		appendcsvInputData("\nArduino4:" + message.payloadString);
	}
}


//Temporary until real query works properly
function parse(s) {
	 var result = s.split(" ");
	 return result[result.length-1];
}

//Sends publish message
function send(publishClientMessage) {
	if (!client) {
		return;
	}

	// refresh
	document.getElementById('publish').value = "";

	var message = new Paho.MQTT.Message(parse(publishClientMessage)); //Temporary until real query works properly
	message.qos = 2;
	if (document.getElementById("arduino1").checked) {
		message.destinationName = "arduino1";
		client.send(message);
	}
	if (document.getElementById("arduino2").checked) {
		message.destinationName = "arduino2";
		client.send(message);
	}
	if (document.getElementById("arduino3").checked) {
		message.destinationName = "arduino3";
		client.send(message);
	}
	if (document.getElementById("arduino4").checked) {
		message.destinationName = "arduino4";
		client.send(message);
	}
}

//Adds data to be stored in csv
function appendcsvInputData(message) {
	  csvInputData += message +"\n";
}

//Downloads csv file
function CSV_download() {
    var data = csvInputData.split("\n");
    var csvData = "data:text/csv;charset=utf-8,";

    data.forEach(function(element,index) {
      if( index < (data.length-1) )
        csvData += element + "\n";
      else
        csvData += element;
    });

    var encodedUri = encodeURI(csvData);
    var download = document.createElement("a");
    download.setAttribute("href",encodedUri);
    download.setAttribute("download", "results.csv");
    download.click();
}

function createCheckBoxes(name) {
	container = $('#checkbox')
	$('<input />', { type: 'checkbox', id: name, value: name}).appendTo(container);
	$('<label />', { 'for': name, text: name}).appendTo(container);
	$('<div />').appendTo(container);
}

function removeCheckboxes(name) {
	console.log(name);
	$('#'+name).remove();
	$('label[for=' + name + ']').remove();
}
