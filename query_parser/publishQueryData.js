const mqtt = require('mqtt')
const url = require('url');
const { parseCreateTable, parseInsert, parseSelect } = require('./parsers')
const Promise = require("bluebird");

module.exports = queryString => {
  return new Promise((resolve, reject) => {
    const operationType = getOperationType(queryString);
    const parse = getQueryParser(operationType);
    const queryJson = JSON.stringify({ operation_type: operationType, query_data: parse(queryString) });

    return publishQueryData(queryJson);

    function getOperationType(queryString) {
      const operations = ['CREATE TABLE', 'SELECT', 'INSERT INTO'];
      return operations.filter(operation => queryString.indexOf(operation) === 0)[0];
    }

    function getQueryParser(operationType) {
      switch(getOperationType(operationType)) {
        case 'CREATE TABLE':
          return parseCreateTable;
        case 'SELECT':
          return parseSelect
        case 'INSERT INTO':
          return parseInsert;
        default:
          return new Error('Could not resolve operation type');
      }
    }

    function publishQueryData(queryData) {
      const client  = mqtt.connect('http://localhost:1883');
      client.on('connect', function () {
        client.subscribe('QUERY')
        client.publish('QUERY', queryData);
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
}


 