const lib = require('./applyWhereClause.js');

const data = "[{\"client\": \"Arduino1\", \"entries\": \"team\\nname:s;\\nspencer:name;\\nspencer:name;\\nspencer:name;;EOP\"}, {\"client\": \"Arduino1\", \"entries\":\"spencer:name;\\n;EOR\"}]";
const parsedData = lib.parseResultSet(JSON.parse(data));
console.log(parsedData);
