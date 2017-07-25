module.exports = resultSet => {
  const parsedResultSet = parsedResultSet(resultSet);

  function parseResultSet(resultSet) {
    const parsedResultSet = {};
    const schemaString = resultSet.entries.split('\n')[1];
    const attributes = schemaString.split(':').filter((item, index) => {
      return index % 2; // filter even elements as just data types
    });
    resultSet.forEach(result => {
      const client = result.client;
      if (!parsedResultSet[client]) {
        result = result.shift().shift(); // handle schema data case
        parsedResultSet[client] = [];
      } 
      const mappedResult = result.map(row => {
        const fields = row.split(',');
        const mappedRow = attributes.reduce((acc, attr, index) => {
          return Object.assign(acc, { [attr]: fields[index] });
        }, {});
      });
      parsedResultSet.push(mappedResult);
    });
    return parsedResultSet;
  }

}