
const { expect } = require('chai');
const {
  parseOperator,
  parseWhere,
  applyEquality,
  applyGreaterThan,
  applyLessThan,
  applyWhereCondition,
  applyWhere,
} = require('./../applyWhere.js');

const whereClause = 'WHERE name LIKE spencer';
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

describe('applyWhere.js', () => {
  describe('applyWhere()', () => {
    it('can filter a parsed result set based on a condition', () =>  {
      const expectedResultSet = {
        attributes: {
          name: 'String',
          age: 'int'
        },
        clientTuples: {
          Arduino1: [ 'spencer,24' ],
          Arduino2: [ 'spencer,24' ]
        }
      };
      const filteredResultSet = applyWhere(resultSet, whereClause);
      expect(filteredResultSet).to.deep.equal(expectedResultSet);
    });
  })
})