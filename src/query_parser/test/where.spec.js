
const { expect } = require('chai');
const {
  parseOperator,
  parseWhere,
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
  describe('parseOperator()', ()=> {
    it('can parse and normalize to lower case the operator of a WHERE clause correctly', () => {
      const expectedParsedOperator = 'like';
      const parsedOperator = parseOperator(whereClause);
      expect(parsedOperator).to.deep.equal(expectedParsedOperator);
    });

    it('throws an error for invalid operators', () => {
      try {
        const parsedOperator = parseOperator('WHERE this INVALID_OPERATOR throws');
        expect(false).to.equal(true); // fail if this code is reached
      } catch(err) {
        expect(true).to.equal(true);
      }
    });
  });

  describe('parseWhere()', () => {
    it('can parse a WHERE clause string', () => {
      const expectedParsedWhere = { field: 'name', value: 'spencer', operator: 'like' };
      const parsedWhere = parseWhere(whereClause);
      expect(parsedWhere).to.deep.equal(expectedParsedWhere);
    });
  });

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