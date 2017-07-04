module.exports = sql => {
  const tableName = sql.split(' FROM ')[1].split(';')[0];
  const fieldString = sql.split('SELECT ')[1].split(' FROM')[0].split(', ').join(',');

  function constructFieldList(fieldString) {
    return fieldString.split(',')
  }

  const req = {
    table: tableName,
    fields: constructFieldList(fieldString),
  }

  return req;
}