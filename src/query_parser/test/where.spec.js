
const { expect } = require('chai');
const { 
  parseWhereClauseFromSelect,
  parseOperator,
  parseWhere,
  applyWhere,
} = require('./../applyWhere.js');

const selectStatement = 'SELECT * FROM team WHERE name LIKE spencer';
const selectWithNoWhere = 'SELECT * FROM team';
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
  describe('parseWhereFromSelect()', ()=> {
    it('can parse the WHERE clause from a select statement', () => {
      const parsedWhereClause = parseWhereClauseFromSelect(selectStatement);
      expect(parsedWhereClause).to.equal(whereClause);
    });

    it('returns false when no where clause present', () => {
      const selectWithNoWhere = 'SELECT * FROM team';
      const parsedWhereClause = parseWhereClauseFromSelect(selectWithNoWhere);
      expect(parsedWhereClause).to.equal(false);
    });
  });

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
      const filteredResultSet = applyWhere(resultSet, selectStatement);
      expect(filteredResultSet).to.deep.equal(expectedResultSet);
    });

    it('returns the resultSet passed in if no WHERE clause in select', () =>  {
      const filteredResultSet = applyWhere(resultSet, selectWithNoWhere);
      expect(filteredResultSet).to.deep.equal(resultSet);
    });
  });
});