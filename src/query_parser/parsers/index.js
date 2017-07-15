const parseCreateTable = require('./parseCreateTable.js');
const parseDescribe = require('./parseDescribe.js');
const parseSelect = require('./parseSelect.js');
const parseInsert = require('./parseInsert.js');

module.exports = {
  parseCreateTable,
  parseSelect,
  parseInsert,
  parseDescribe,
}