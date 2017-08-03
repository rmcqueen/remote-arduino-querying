const whereString = 'WHERE name = spencer';
const resultSet = {
  "attributes": {
    "name": "String",
    "age": "int",
  },
  "clientTuples": {
    "Arduino1": [
      "david,28",
      "ryan,21",
      "dustin,30",
      "spencer,24",
    ],
    "Arduino2": [
      "david,28",
      "ryan,21",
      "dustin,30",
      "spencer,24",
    ],
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
  const [ field, value ] = comparison.split(` ${operator} `);
  return { field: field, value: value, operator: operator };
}

function applyEquality(fieldValue, filterValue) {
  return fieldValue == filterValue;
}

function applyGreaterThan(fieldValue, filterValue) {
  return fieldValue > filterValue;
}

function applyLessThan(fieldValue, filterValue) {
  return fieldValue < filterValue;
}

function getFilterOperation(operator) {
  switch(operator) {
    case '=':
    case 'like':
      return applyEquality;
    case '>':
      return applyGreaterThan;
    case '<':
      return applyLessThan;
  }
}

function applyWhereCondition(resultSet, where) {
  const { field, value, operator } = where;
  const columnNumber = Object.keys(resultSet.attributes).indexOf(field);
  const filterOperation = getFilterOperation(operator);
  const filteredClientTuples = Object.keys(resultSet.clientTuples).reduce((acc, client) => {
    // split tuples on ,
    // apply where to the ith tuple, where i = index of operand in attributes.keys
    const filteredTuples = resultSet.clientTuples[client].filter(tuple => {
      const fieldValue = tuple.split(',')[columnNumber]; // assumes tuples are always ordered the same way as attributes.keys() in resultSet
      return filterOperation(fieldValue, value);
    });
    return Object.assign(acc, { [client]: filteredTuples });
  }, {});
  return Object.assign({}, resultSet, { clientTuples: filteredClientTuples });
}

function applyWhere(resultSet, whereString) {
  const where = parseWhere(whereString);
  return applyWhereCondition(resultSet, where);
}

const filteredResultSet = applyWhere(resultSet, whereString);
console.log(filteredResultSet);