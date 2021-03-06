const { flatten } = require('lodash');
const { parseCreateTable, parseDescribe, parseInsert, parseSelect } = require('./parsers')

const dataTypeMapping = {
  i: 'int',
  s: 'String',
}

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

function getResultSetAttributes(resultSet) {
  const schemaString = resultSet[0][0].entries.split('\n')[1]; // Assumes pages remained serialized for each client. 
  const attributes = schemaString.split(';') // tokenize compressed schema elements
    .filter((val, idx, arr) => idx < arr.length - 1) // remove junk element caused by terminal ;
    .reduce((acc, attribute) => { // reduce the tokens to a single mapping of attribute name to data type
      const [ attrName, attrType ] = attribute.split(':');
      return Object.assign(acc, { [attrName]: dataTypeMapping[attrType] });
  }, {});
    return attributes;
}

function buildClientTuplePages(resultSet) {
  const clientSchemasProcessed = [];
  const clientTuplePages = resultSet.map(result => {
    const client = result.client;
    result.entries = result.entries.replace('\n\n;EOP\u0000', '').replace('\n;EOR\u0000', '').split('\n');
    if (clientSchemasProcessed.indexOf(client) === -1) {
      result.entries = result.entries.filter((entry, idx) => {
        return idx > 1;
      }); // Handle schema data stored in indexes 0 and 1 for all clients. Assumes pages remained serialized for each client. 
      clientSchemasProcessed.push(client);
    }
    const compressedTuples = result.entries;
    const tuplePages = compressedTuples.map(row => {
      const fields = row.split(';').map(attr => attr.split(':')[0]).join() // remove field names stored in compressed rows
      return fields;
    });
    const tuples = flatten(tuplePages).filter(tuple => tuple != '');
    console.log(tuples);
    return {
      client: client,
      tuples: tuples,
    };
  });
  return clientTuplePages;
}

function groupTuplePagesByClient(clientTuplePages) {
  return clientTuplePages.reduce((acc, clientTuplePage) => {
    const pageClient = clientTuplePage.client;
    const currentClientTuples = acc[pageClient] ? acc[pageClient] : [];
    const pageTuples = clientTuplePage.tuples;
    const clientTuples = [...currentClientTuples, ...pageTuples]; // merge arrays with current content for this client in acc
    return Object.assign(acc, { [pageClient]: clientTuples });
  }, {});
}

function flattenClientTuples(clientTuples) {
  return clientTuples.reduce((acc, clientTuple) => {
    const client = Object.keys(clientTuple)[0];
    return Object.assign(acc, { [client]: clientTuple[client] });
  });
}

function resultSetNeedsParsing(resultSet) {
  const resultSetString = JSON.stringify(resultSet);
  const blacklist = ['Table created', 'Record inserted']
  const filtered = blacklist.filter(response => resultSetString.indexOf(response) !== -1);
  return filtered.length === 0;
}

function parseResultSet(resultSet) {
  if (!resultSetNeedsParsing(resultSet)) return false;
  const attributes = getResultSetAttributes(resultSet);
  const clientTuplePages = resultSet.map(buildClientTuplePages);
  const clientTuples = clientTuplePages.map(groupTuplePagesByClient);
  const flattenedClientTuples = flattenClientTuples(clientTuples);
  return {
    attributes: attributes,
    clientTuples: flattenedClientTuples,
  };
}

module.exports = {
  getOperationType,
  getQueryParser,
  getResultSetAttributes,
  buildClientTuplePages,
  groupTuplePagesByClient,
  parseResultSet,
}