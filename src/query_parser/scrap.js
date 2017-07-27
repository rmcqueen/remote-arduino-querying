const lib = require('./lib.js');

const data = "[{\"client\": \"Arduino1\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;;EOP\"}, {\"client\": \"Arduino1\", \"entries\":\"spencer:name;\\n;EOR\"}]";
const parsedData = lib.parseResultSet(JSON.parse(data));
console.log(JSON.stringify(parsedData, null, 2));
