function compressFieldArray(fieldArray) {
  const [ fieldName, fieldType ] = fieldArray;
  return compressFieldElements(fieldName, fieldType);
}

function appendFieldDatum(fieldDatum, fieldData) {
  return `${fieldData}${fieldDatum};`; // add field data to data string
}

function compressFieldElements(fieldName, fieldType) {
    const typeCode = fieldType[0].toLowerCase(); // TODO dirty, clean me
    return `${fieldName}:${typeCode}`; // requires additional processing to allow for use of ; and : in query string constants
}

module.exports = {
  appendFieldDatum,
  compressFieldElements,
}