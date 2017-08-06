const pg = require('pg');
const connectionString = process.env.DATABASE_URL || 'postgres://localhost:5432/arduino_query_tracker';

client = new pg.Client(connectionString);
client.connect();

const query = client.query(
  'CREATE TABLE items(id SERIAL PRIMARY KEY, text VARCHAR(40) not null, complete BOOLEAN)');
query.on('end', () => { client.end(); });

// const query = client.query(
//     'CREATE TABLE clients(id SERIAL PRIMARY KEY, arduinoID VARCHAR(255) not null, status VARCHAR(255) not null, updated_at timestamp)');
// query.on('end', () => {client.end(); });
