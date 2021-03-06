const { expect } = require('chai');
const parsers = require('./../parsers');
const {
  getOperationType,
  getQueryParser,  
  getResultSetAttributes,
  buildClientTuplePages,
  groupTuplePagesByClient,
  parseResultSet,
} = require('../lib.js');

const resultSet = JSON.parse("[{\"client\": \"Arduino1\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;\\n\\n;EOP\\u0000\"}, {\"client\": \"Arduino1\", \"entries\":\"spencer:name;\\n;EOR\\u0000\"}]");

// Test Data
const createTable = 'CREATE TABLE team(name string, age int);';
const insertInto = 'INSERT INTO team(name, age) VALUES(spencer, 24);';
const selectAll = 'SELECT * FROM team;';
const expectedClientTuplePages = [
  { client: 'Arduino1', tuples: [ 'david', 'ryan', 'dustin' ] },
  { client: 'Arduino1', tuples: [ 'spencer' ] }
];
const expectedClientTuples = { Arduino1: [ 'david', 'ryan', 'dustin', 'spencer' ] };

function debugObject(data) {
  console.log(JSON.stringify(data, null, 2));
}

describe('lib', () => {
  describe('getResultSetAttributes', () => {
    it('builds an array of client tuple mappings', () => {
      const expectedAttributes = { name: 'String' };
      const nestedResultSet = JSON.parse("[[{\"client\": \"Arduino1\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;\\n\\n;EOP\\u0000\"}, {\"client\": \"Arduino1\", \"entries\":\"spencer:name;\\n;EOR\\u0000\"}]]");
      const attributes = getResultSetAttributes(nestedResultSet);
      expect(attributes).to.deep.equal(expectedAttributes);
    });
  });

  describe('buildClientTuplePages', () => {
    it('builds an array of client tuple mappings', () => {
      const expectedClientTuplePages = [
        { client: 'Arduino1', tuples: [ 'david,', 'ryan,', 'dustin,' ] },
        { client: 'Arduino1', tuples: [ 'spencer,' ] }
      ];
      const clientTuplePages = buildClientTuplePages(resultSet);
      expect(clientTuplePages).to.deep.equal(expectedClientTuplePages);
    });
  });

  describe('groupTuplePagesByClient', () => {
    it('groups touples by client id', () => {
      const clientTuples = groupTuplePagesByClient(expectedClientTuplePages);
      expect(clientTuples).to.deep.equal(expectedClientTuples);
    });
  });

  describe('parseResultSet', () => {
    it('parses the compressed set into an object', () => {
      const expectedParsedResult = {
        attributes: {
          name: 'String'
        },
        clientTuples: {
          Arduino1: [
            'david,',
            'ryan,',
            'dustin,',
            'spencer,',
          ]
        }
      }
      const resultSetObj = JSON.parse("[[{\"client\": \"Arduino1\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;\\n\\n;EOP\\u0000\"}, {\"client\": \"Arduino1\", \"entries\":\"spencer:name;\\n;EOR\\u0000\"}]]");
      const parsedResult = parseResultSet(resultSetObj);
      expect(parsedResult).to.deep.equal(expectedParsedResult);
    });
  });

  describe('getOperationType', () => {
    it('gets the correct operation type for create table', () => {
      const sql = createTable;
      const operationType = getOperationType(sql);
      expect(operationType).to.equal('CREATE');
    });

    it('gets the correct operation type for insert', () => {
      const sql = insertInto;
      const operationType = getOperationType(sql);
      expect(operationType).to.equal('INSERT');
    });

    it('gets the correct operation type for select', () => {
      const sql = selectAll;
      const operationType = getOperationType(sql);
      expect(operationType).to.equal('SELECT');
    });
  });

  describe('getQueryParser', () => {
    it('gets the correct query parser for create table', () => {
      const sql = createTable;
      const queryParser = getQueryParser(sql);
      expect(queryParser).to.equal(parsers.parseCreateTable);
    });

    it('gets the correct query parser for insert', () => {
      const sql = insertInto;
      const queryParser = getQueryParser(sql);
      expect(queryParser).to.equal(parsers.parseInsert);
    });

    it('gets the correct query parser for select', () => {
      const sql = selectAll;
      const queryParser = getQueryParser(sql);
      expect(queryParser).to.equal(parsers.parseSelect);
    });

    it('throws an error when it cannot resolve the parser', () => {
      try {
        const sql = 'INVALID SQL';
        const queryParser = getQueryParser(sql);
        expect(false).to.equal(true);
      } catch(err) {
        expect(true).to.equal(true)
      }
    });
  });
});
