const mqtt = require('mqtt');
const url = require('url');
const { getOperationType, getQueryParser } = require('./lib.js');
const Promise = require("bluebird");

module.exports = (queryString, targets, testCallback = false) => {
  return Promise.map(targets, target => {
    return new Promise((resolve, reject) => {
      const operationType = getOperationType(queryString);
      const parse = getQueryParser(operationType);
      const queryJson = JSON.stringify({ op_code: operationType[0].toLowerCase(), query: parse(queryString)});

      return publishQueryData(queryJson);

      //TODO: Set publish topic based on "arduino: selectedArduinos" from get request
      function publishQueryData(queryData) {
        const result = [];
        const client  = mqtt.connect('http://localhost:1883');
        client.on('connect', function () {
          client.subscribe(`result/${target}`);
          client.publish(`query/${target}`, queryData, { qos:2, retain:false });
          console.log("published " + queryData);
          if (testCallback) testCallback();
        })

        return client.on('message', (topic, payload) => {
          console.log('message received');
          console.log(topic, payload.toString());
          result.push(JSON.parse(payload.toString()));
          if (payload.toString().indexOf(';EOR') !== -1) {
            client.end();
            return resolve(result);
          }
        })
      }
    });
  })
}


 
