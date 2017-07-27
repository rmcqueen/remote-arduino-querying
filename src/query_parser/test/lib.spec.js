const { expect } = require('chai');
const {
  getOperationType,
  getQueryParser,  
  getResultSetAttributes,
  buildClientTuplePages,
  groupTuplePagesByClient,
  parseResultSet,
} = require('../lib.js');

const resultSet = JSON.parse("[{\"client\": \"Arduino1\", \"entries\": \"team\\nname:s;\\ndavid:name;\\nryan:name;\\ndustin:name;;EOP\"}, {\"client\": \"Arduino1\", \"entries\":\"spencer:name;\\n;EOR\"}]");

describe('buildClientTuplePages', () => {
  it('builds an array of client tuple mappings', () => {
    const expectedClientTuplePages = [
      { client: 'Arduino1', tuples: [ 'david', 'ryan', 'dustin' ] },
      { client: 'Arduino1', tuples: [ 'spencer' ] }
    ];
    const clientTuplePage = buildClientTuplePages(resultSet);
    expect(clientTuplePage).to.equal(expectedClientTuplePages);
  });
});

describe('groupTuplePagesByClient', () => {
  it('groups touples by client id', () => {
    const expectedClientTuples = { Arduino1: [ 'david', 'ryan', 'dustin', 'spencer' ] };
    const clientTuples = groupTuplePagesByClient(resultSet);
    expect(clientTuples).to.equal(expectedClientTuples);
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
          david,
          ryan,
          dustin,
          spencer,
        ]
      }
    }
    const parsedResult = parseResultSet(result);
    expect(parsedResult).to.equal(expectedParsedResult);
  });
});

const createTable = 'CREATE TABLE team(name string, age int);';
const insertInto = 'INSERT INTO team(name, age) VALUES(spencer, 24);';
const selectAll = 'SELECT * FROM team;';

describe('getOperationType', () => {
  it('gets the correct operation type for create table', () => {
    const sql = createTable;
    const operationType = getOperationType(sql);
    expect(operationType).to.equal('c');
  });

  it('gets the correct operation type for insert', () => {
    const sql = insertInto;
    const operationType = getOperationType(sql);
    expect(operationType).to.equal('i');
  });

  it('gets the correct operation type for select', () => {
    const sql = selectAll;
    const operationType = getOperationType(sql);
    expect(operationType).to.equal('s');
  });
});

describe('getQueryParser', () => {
  it('gets the correct query parser for create table', () => {
    const sql = createTable;
    const queryParser = getQueryParser(sql);
    expect(queryParser).to.equal('c');
  });

  it('gets the correct query parser for insert', () => {
    const sql = insertInto;
    const queryParser = getQueryParser(sql);
    expect(queryParser).to.equal('i');
  });

  it('gets the correct query parser for select', () => {
    const sql = selectAll;
    const queryParser = getQueryParser(sql);
    expect(queryParser).to.equal('s');
  });
});

describe('getResultSetAttributes', () => {
  it('gets the attributes for the result set', () => {
    const expectedAttributes = { name: 'String' };
    const attributes = getResultSetAttributes(resultSet);
    expect(attributes).to.equal(expectedAttributes);
  });
});

describe('buildClientTuplePages', () => {
  it('builds an array of client tuple mappings', () => {
    const expectedClientTuplePages = [
      { client: 'Arduino1', tuples: [ 'david', 'ryan', 'dustin' ] },
      { client: 'Arduino1', tuples: [ 'spencer' ] }
    ];
    const clientTuplePage = buildClientTuplePages(resultSet);
    expect(clientTuplePage).to.equal(expectedClientTuplePages);
  });
});

describe('groupTuplePagesByClient', () => {
  it('groups touples by client id', () => {
    const expectedClientTuples = { Arduino1: [ 'david', 'ryan', 'dustin', 'spencer' ] };
    const clientTuples = groupTuplePagesByClient(resultSet);
    expect(clientTuples).to.equal(expectedClientTuples);
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
          david,
          ryan,
          dustin,
          spencer,
        ]
      }
    }
    const parsedResult = parseResultSet(result);
    expect(parsedResult).to.equal(expectedParsedResult);
  });
});