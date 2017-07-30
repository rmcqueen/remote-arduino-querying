const mqtt = require('mqtt');
const Promise = require('bluebird');
const { expect } = require('chai');
const publishQueryData = require('./../publishQueryData.js');

const nonTerminalPage = "{\"client\": \"test\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;;EOP\"}";
const terminalPage = "{\"client\": \"test\", \"entries\":\"spencer:name;\\n;EOR\"}";

function mockArduinoResponse(response) {
  // synchronous promise wrapper
  return new Promise((resolve) => {
    const client  = mqtt.connect('http://localhost:1883');
    client.on('connect', () => {
      client.subscribe('result/test');
      client.publish('result/test', response, { qos:2, retain: false });
    });

    // close the client and resolve when result is received
    client.on('message', (topic, payload) => {
      client.end();
      return resolve(null);
    });
  });
}

function testCallback() {
  console.log('testCallback executed')
  const pages = [nonTerminalPage, terminalPage];
  return Promise.each(pages, page => mockArduinoResponse(page));
}

describe('Arduino responses', () => {
  describe('multi-page results', () => {
    it('does not resolve until the end-of-result flag before resolving', () => {
      const queryString = 'SELECT * FROM team;';
      const targets = ['test'];
      return publishQueryData(queryString, targets, testCallback)
        .then(resultSet => {
          const expectedResultSet = JSON.parse("[[{\"client\": \"test\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;;EOP\"}, {\"client\": \"test\", \"entries\":\"spencer:name;\\n;EOR\"}]]");
          console.log(resultSet);
          expect(resultSet).to.deep.equal(expectedResultSet);
        });
    });
  })
});