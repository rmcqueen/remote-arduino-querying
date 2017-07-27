const { parseCreateTable, parseDescribe, parseInsert, parseSelect } = require('./parsers');

const createTable = 'CREATE TABLE team(name string, age int);';
const insertInto = 'INSERT INTO team(name, age) VALUES(spencer, 24);';
const selectAll = 'SELECT * FROM team;';

describe('parseCreate', () => {
  it('produces the correct compressed instruction', () => {
    const expectedCompressedCreate = '';
    const compressedCreate = parseCreateTable(createTable);
    expect(compressedCreate).to.equal(expectedCompressedCreate);
  });
});

describe('parseInsert', () => {
  it('produces the correct compressed instruction', () => {
    const expectedCompressedInsert = '';
    const compressedInsert = parseCreateTable(insertInto);
    expect(compressedInsert).to.equal(expectedCompressedInsert);
  });
});

describe('parseSelect', () => {
  it('produces the correct compressed instruction', () => {
    const expectedCompressedSelect = '';
    const compressedSelect = parseCreateTable(selectAll);
    expect(compressedSelect).to.equal(expectedCompressedSelect);
  });
});