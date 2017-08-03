const whereString = 'WHERE name = spencer';
const resultSet = {
  "attributes": {
    "name": "String"
  },
  "clientTuples": {
    "Arduino1": [
      "david",
      "ryan",
      "dustin",
      "spencer"
    ]
  }
}

function parseOperator(whereClause) {
  const operators = ['>', '=', '<', 'like'];
  const operator = operators.filter(operator => whereClause.indexOf(operator) !== -1);
  if (operator[0]) {
    return operator[0];
  }
  throw new Error('Invalid operand in WHERE clause');
}

function parseWhere(whereClause) {
  const comparison = whereClause.split('WHERE ')[1];
  const operator = parseOperator(whereClause)
  console.log(operator);
  const [ field, value ] = comparison.split(` ${operator} `);
  return { field: field, value: value, operator: operator };
}

function applyGreaterThan() {}
function applyLessThan() {}

function applyEquality(resultSet, where) {
  const columnNumber = Object.keys(resultSet.attributes).indexOf(where.field);
  const filteredClientTuples = Object.keys(resultSet.clientTuples).reduce((acc, client) => {
    // split tuples on ,
    // apply where to the ith tuple, where i = index of operand in attributes.keys
    const filteredTuples = resultSet.clientTuples[client].filter(tuple => {
      const fieldValue = tuple.split(',')[columnNumber]; // assumes tuples are always ordered the same way as attributes.keys() in resultSet
      return fieldValue == where.value;
    });
    return Object.assign(acc, { [client]: filteredTuples });
  }, {});
  return Object.assign({}, resultSet, { clientTuples: filteredClientTuples });
}

function applyWhere(resultSet, where) {
  switch(where.operator) {
    case '=':
    case 'like':
      return applyEquality(resultSet, where);
    case '>':
      return applyGreaterThan(resultSet, where);
    case '<':
      return applyLessThan(resultSet, where);
  }
}

function where(resultSet, whereString) {
  const where = parseWhere(whereString);
  console.log(where);
  return applyWhere(resultSet, where);
}

const filteredResultSet = where(resultSet, whereString);
console.log(filteredResultSet);