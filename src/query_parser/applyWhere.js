const { parseResultSet } = require('./lib.js')

function parseWhereClauseFromSelect(selectStatement) {
  const whereClauseIndex = selectStatement.toLowerCase().indexOf('where');
  if (whereClauseIndex === -1) return false;
  return selectStatement.substr(whereClauseIndex);
}

function parseOperator(whereClause) {
  const normalizedWhereClause = whereClause.toLowerCase();
  const operators = ['>', '=', '<', 'like'];
  const operator = operators.filter(operator => normalizedWhereClause.indexOf(operator) !== -1);
  if (operator[0]) {
    return operator[0];
  }
  throw new Error('Invalid operand in WHERE clause');
}

function parseWhere(whereClause) {
  const comparison = whereClause.split('WHERE ')[1].toLowerCase();
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
    default: 
      throw new Error(`Could not resolve filter operation. Operator: ${operator}`);
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

function applyWhere(resultSet, selectStatement) {
  const whereClause = parseWhereClauseFromSelect(selectStatement);
  if (whereClause === false) return resultSet;
  const where = parseWhere(whereClause);
  return applyWhereCondition(resultSet, where);
}

module.exports = {
  parseWhereClauseFromSelect,
  parseOperator,
  parseWhere,
  applyWhere,
}
