module.exports = sql => {
  const tableName = sql.split('CREATE TABLE ')[1].split('(')[0];
  const fieldString = sql.split('(')[1].split(')')[0].split(', ').join(',');

  function constructFieldMap(fieldString) {
    fieldMap = [];
    const fields = fieldString.split(',');
    fields.forEach(field => {
      const fieldPieces = field.split(' ');
      const fieldKeyValue = {key: fieldPieces[0], value: fieldPieces[1]};
      fieldMap.push(fieldKeyValue);
    })
    return fieldMap;
  }

  const req = {
    table: tableName,
    fields: constructFieldMap(fieldString),
  }

  return req;
}

