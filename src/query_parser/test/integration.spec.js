const mqtt = require('mqtt');
const Promise = require('bluebird');
const { expect } = require('chai');
const publishQueryData = require('./../publishQueryData.js'); 
const { parseResultSet } = require('./../lib.js');
const { applyWhere } = require('./../applyWhere.js');

const nonTerminalPage = "team\nname:s;\ndavid:name;\nryan:name;\ndustin:name;;\nEOP\u0000";
const terminalPage = "spencer:name;\n;EOR\u0000";

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
  console.log('testCallback executed');
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
          const expectedResultSet = JSON.parse("[[{\"client\": \"test\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;;\\nEOP\\u0000\"}, {\"client\": \"test\", \"entries\":\"spencer:name;\\n;EOR\\u0000\"}]]");
          expect(resultSet).to.deep.equal(expectedResultSet);
        });
    });
  });

  describe('lib integrations', () => {
    it('can parse the arduino responses into an object', () => {
      const queryString = 'SELECT * FROM team WHERE name LIKE spencer';
      const targets = ['test'];
      const expectedResultSet = {
        attributes: {
          name: 'String',
        },
        clientTuples: {
          test: [ 'spencer,' ],
        }
      };
      return publishQueryData(queryString, targets, testCallback)
        .then(resultSet => {
          const result = applyWhere(parseResultSet(resultSet), queryString);
          expect(result).to.deep.equal(expectedResultSet);
        });
    });
  });
});


