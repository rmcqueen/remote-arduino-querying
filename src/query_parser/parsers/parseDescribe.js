module.exports = sql => {
  console.log({ table: sql.split('DESCRIBE ')[1] });
  return { table: sql.split('DESCRIBE ')[1] };
}

