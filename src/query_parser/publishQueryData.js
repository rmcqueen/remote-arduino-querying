const mqtt = require('mqtt')
const url = require('url');
const { parseCreateTable, parseDescribe, parseInsert, parseSelect } = require('./parsers')
const Promise = require("bluebird");

module.exports = (queryString, targets) => {
  return Promise.map(targets, target => {
    return new Promise((resolve, reject) => {
      const operationType = getOperationType(queryString);
      const parse = getQueryParser(operationType);
      const queryJson = JSON.stringify({ op_code: operationType[0].toLowerCase(), query: parse(queryString)});

      return publishQueryData(queryJson);

      function getOperationType(queryString) {
        const operations = ['CREATE', 'SELECT', 'INSERT', 'DESCRIBE'];
        return operations.filter(operation => queryString.split()[0].indexOf(operation) === 0)[0];
      }

      function getQueryParser(operationType) {
        switch(getOperationType(operationType)) {
          case 'CREATE':
            return parseCreateTable;
          case 'DESCRIBE':
            return parseDescribe;
          case 'SELECT':
            return parseSelect
          case 'INSERT':
            return parseInsert;
          default:
            return new Error('Could not resolve operation type');
        }
      }

      //TODO: Set publish topic based on "arduino: selectedArduinos" from get request
      function publishQueryData(queryData) {
        const client  = mqtt.connect('http://localhost:1883');
        client.on('connect', function () {
          client.subscribe('arduino1')
          client.publish(`query/${target}`, queryData, { qos:2, retain:false });
          console.log("published " + queryData);
        })

        return client.on('message', (topic, payload) => {
          console.log('message received!');
          console.log(topic, payload.toString());
          client.end();
          return resolve('message received: ' + payload.toString());
        })
      }
    });
  });
}


 
