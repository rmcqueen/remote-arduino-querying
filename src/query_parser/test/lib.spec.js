const {
  getOperationType,
  getQueryParser,  
  getResultSetAttributes,
  buildClientTuplePages,
  groupTuplePagesByClient,
  parseResultSet,
} = require('./../lib.js');

const resultSet = {};

const createTable = 'CREATE TABLE team(name string, age int);';
const insertInto = 'INSERT INTO team(name, age) VALUES(spencer, 24);';
const selectAll = 'SELECT * FROM team;';

describe('getOperationType', () => {
  it('gets the correct operation type for create table', () => {
    const sql = '';
    const operationType = getOperationType(sql);
    expect(operationType).to.equal('c');
  });

  it('gets the correct operation type for insert', () => {
    const sql = '';
    const operationType = getOperationType(sql);
    expect(operationType).to.equal('i');
  });

  it('gets the correct operation type for select', () => {
    const sql = '';
    const operationType = getOperationType(sql);
    expect(operationType).to.equal('s');
  });
});

describe('getQueryParser', () => {
  it('gets the correct query parser for create table', () => {
    const sql = '';
    const queryParser = getQueryParser(sql);
    expect(queryParser).to.equal('c');
  });

  it('gets the correct query parser for insert', () => {
    const sql = '';
    const queryParser = getQueryParser(sql);
    expect(queryParser).to.equal('i');
  });

  it('gets the correct query parser for select', () => {
    const sql = '';
    const queryParser = getQueryParser(sql);
    expect(queryParser).to.equal('s');
  });
});

describe('getResultSetAttributes', () => {
  it('gets the attributes for the result set', () => {
    const expectedAttributes = '';
    const attributes = getResultSetAttributes(resultSet);
    expect(attributes).to.equal(expectedAttributes);
  });
});

describe('buildClientTuplePages', () => {
  it('builds an array of client tuple mappings', () => {
    const expectedClientTuplePages = '';
    const clientTuplePage = buildClientTuplePages(resultSet);
    expect(clientTuplePage).to.equal(expectedClientTuplePages);
  });
});

describe('groupTuplePagesByClient', () => {
  it('groups touples by client id', () => {
    const expectedGroupedTuplePages = '';
    const groupedTuplePages = groupTuplePagesByClient(resultSet);
    expect(groupedTuplePages).to.equal(expectedGroupedTuplePages);
  });
});

describe('parseResultSet', () => {
  it('parses the compressed set into an object', () => {
    const expectedParsedResult = '';
    const parsedResult = parseResultSet(result);
    expect(parsedResult).to.equal(expectedParsedResult);
  });
});