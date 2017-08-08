const { expect } = require('chai');
const { parseCreateTable, parseDescribe, parseInsert, parseSelect } = require('../parsers');

const createTable = 'CREATE TABLE team(name string, age int);';
const insertInto = 'INSERT INTO team(name, age) VALUES(spencer, 24);';
const selectAll = 'SELECT * FROM team;';
const describeTable = 'DESCRIBE team;';

describe('parsers', () => {
  describe('parseCreate', () => {
    it('produces the correct compressed table and field combination', () => {
      const expectedCompressedCreate = { table: 'team', fields: 'name:s;age:i;' };
      const compressedCreate = parseCreateTable(createTable);
      expect(compressedCreate).to.deep.equal(expectedCompressedCreate);
    });
  });

  describe('parseInsert', () => {
    it('produces the correct compressed table and field combination', () => {
      const expectedCompressedInsert = { table: 'team', fields: 'spencer:name;24:age;' };
      const compressedInsert = parseInsert(insertInto);
      expect(compressedInsert).to.deep.equal(expectedCompressedInsert);
    });
  });

  describe('parseSelect', () => {
    it('produces the correct compressed table and field combination', () => {
      const expectedCompressedSelect = { table: 'team' };
      const compressedSelect = parseSelect(selectAll);
      expect(compressedSelect).to.deep.equal(expectedCompressedSelect);
    });
  });

  describe('parseDescribe', () => {
    it('produces the correct compressed table and field combination', () => {
      const expectedCompressedDescribe = { table: 'team' };
      const compressedDescribe = parseDescribe(describeTable);
      expect(compressedDescribe).to.deep.equal(expectedCompressedDescribe);
    });
  });
});