module.exports = sql => {
  const fieldNamesString = sql.split('(')[1].split(')')[0].split(', ').join(',');
  const fieldValuesString = sql.split('(')[2].split(')')[0].split(', ').join(',');

  function constructFieldMap(fieldNamesString, fieldValuesString) {
    const fieldMap = [];
    const fieldNames = fieldNamesString.split(',');
    const fieldValues = fieldValuesString.split(',');
    for(i = 0; i < fieldNames.length; i++) {
      const fieldKeyValue = { field: fieldNames[i], value: fieldValues[i] };
      fieldMap.push(fieldKeyValue);
    }
    return fieldMap;
  }

  const req = {
    table: tableName,
    fields: constructFieldMap(fieldNamesString, fieldValuesString),
  }

  return req;
}