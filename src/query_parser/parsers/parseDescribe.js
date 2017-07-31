module.exports = sql => {
  const req = { table: sql.split('DESCRIBE ')[1].split(';')[0] };
  return req;
}

