const { flatten } = require('lodash');

function getResultSetAttributes(resultSet) {
  const schemaString = resultSet[0].entries.split('\n')[1]; // Assumes pages remained serialized for each client. 
  const attributes = schemaString.split(';') // tokenize compressed schema elements
    .filter((val, idx, arr) => idx < arr.length - 1) // remove junk element caused by terminal ;
    .reduce((acc, attribute) => { // reduce the tokens to a single mapping of attribute name to data type
      const [ attrName, attrType ] = attribute.split(':');
      return Object.assign(acc, { [attrName]: attrType });
  }, {});
}

function buildClientTuplePages(resultSet) {
  const clientSchemasProcessed = [];
  const clientTuplePages = resultSet.map(result => {
    const client = result.client;
    result.entries = result.entries.replace(';EOP', '').replace('\n;EOR', '').split('\n');
    if (clientSchemasProcessed.indexOf(client) === -1) {
      result.entries = result.entries.filter((entry, idx) => {
        return idx > 1;
      }); // Handle schema data stored in indexes 0 and 1 for all clients. Assumes pages remained serialized for each client. 
      clientSchemasProcessed.push(client);
    }
    const compressedTuples = result.entries;
    const tuplePages = compressedTuples.map(row => {
      const fields = row.split(':').filter((datum, idx) => (idx + 1) % 2); // remove field names stored in compressed rows
      return fields;
    });
    const tuples = flatten(tuplePages);
    return {
      client: client,
      tuples: tuples,
    };
  });
  return clientTuplePages;
}

function groupleTuplePagesByClient(clientTuplePages) {
  return clientTuplePages.reduce((acc, clientTuplePage) => {
    const pageClient = clientTuplePage.client;
    const currentClientTuples = acc[pageClient] ? acc[pageClient] : [];
    const pageTuples = clientTuplePage.tuples;
    const clientTuples = [...currentClientTuples, ...pageTuples]; // merge arrays with current content for this client in acc
    return Object.assign(acc, { [pageClient]: clientTuples });
  }, {});
}

function parseResultSet(resultSet) {
  const attributes = getResultSetAttributes(resultSet);
  const clientTuplePages = buildClientTuplePages(resultSet);
  const clientTuples = groupleTuplePagesByClient(clientTuplePages);
  return {
    attributes: attributes,
    clientTuples: clientTuples,
  };
}

module.exports = {
  getResultSetAttributes,
  buildClientTuplePages,
  groupleTuplePagesByClient,
  parseResultSet,
}