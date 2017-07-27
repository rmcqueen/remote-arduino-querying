const { expect } = require('chai');
const { parseCreateTable, parseDescribe, parseInsert, parseSelect } = require('../parsers');

const createTable = 'CREATE TABLE team(name string, age int);';
const insertInto = 'INSERT INTO team(name, age) VALUES(spencer, 24);';
const selectAll = 'SELECT * FROM team;';

describe('parseCreate', () => {
  it('produces the correct compressed instruction', () => {
    const expectedCompressedCreate = '{"op_code":"c","query":{"table":"team","fields":"name:s;"}}';
    const compressedCreate = parseCreateTable(createTable);
    expect(compressedCreate).to.equal(expectedCompressedCreate);
  });
});

describe('parseInsert', () => {
  it('produces the correct compressed instruction', () => {
    const expectedCompressedInsert = '{"op_code":"i","query":{"table":"team","fields":"spencer:name;"}}';
    const compressedInsert = parseCreateTable(insertInto);
    expect(compressedInsert).to.equal(expectedCompressedInsert);
  });
});

describe('parseSelect', () => {
  it('produces the correct compressed instruction', () => {
    const expectedCompressedSelect = '{"op_code":"s","query":{"table":"team"}}';
    const compressedSelect = parseCreateTable(selectAll);
    expect(compressedSelect).to.equal(expectedCompressedSelect);
  });
});