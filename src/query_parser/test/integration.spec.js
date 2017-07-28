const mqtt = require('mqtt');

const nonTerminalPage = "{\"client\": \"Arduino1\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;;EOP\"}";
const terminalPage = "{\"client\": \"Arduino1\", \"entries\":\"spencer:name;\\n;EOR\"}";

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

describe('Arduino responses', () => {
  it('does not resolve until the end-of-result flag before resolving', () => {
    
  }); 
});



