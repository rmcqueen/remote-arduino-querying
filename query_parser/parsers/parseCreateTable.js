module.exports = sql => {
  const tableName = sql.split('CREATE TABLE ')[1].split(' ')[0];
  const fieldString = sql.split('(')[1].split(')')[0].split(', ').join(',');

  function compressFields(fieldString) {
    let fieldData = '';
    const fields = fieldString.split(',');
    fields.forEach(field => {
      const [ fieldName, fieldType ] = field.split(' ');
      const fieldDatum = compressFieldElements(fieldName, fieldType);
      fieldData = appendFieldDatum(fieldDatum, fieldData)
    })
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
    fields: compressFields(fieldString),
  }
  console.log(req)
  return req;
}

