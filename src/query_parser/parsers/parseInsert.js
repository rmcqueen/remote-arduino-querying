module.exports = sql => {
  const tableName = sql.split('INSERT INTO ')[1].split('(')[0];
  const fieldNamesString = sql.split('(')[1].split(')')[0].split(', ').join(',');
  const fieldValuesString = sql.split('(')[2].split(')')[0].split(', ').join(',');

  function constructFieldMap(fieldNamesString, fieldValuesString) {
    let fieldData = "";
    const fieldNames = fieldNamesString.split(',');
    const fieldValues = fieldValuesString.split(',');
    for(i = 0; i < fieldNames.length; i++) {
      // const [ fieldName, fieldType ] = [fieldNames[i], fieldValues[i]];
      // const fieldDatum = compressFieldElements(fieldName, fieldType);
      // fieldData = appendFieldDatum(fieldDatum, fieldData);
      fieldData = fieldData + fieldValues[i] + ':' + fieldNames[i] + ';'
    }
    return fieldData;
  }
  
  function appendFieldDatum(fieldDatum, fieldData) {
    return `${fieldData}${fieldDatum};`; // add field data to data string
  }

  function compressFieldElements(fieldName, fieldType) {
      const typeCode = fieldType[0].toLowerCase(); // TODO dirty, clean me
      return `${fieldName}:${typeCode}`; // requires additional processing to allow for use of ; and : in query string constants
  }

  const req = {
    table: tableName,
    fields: constructFieldMap(fieldNamesString, fieldValuesString),
  }

  return req;
}
