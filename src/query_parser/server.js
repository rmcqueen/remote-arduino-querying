const express = require('express');

const app = express();
require('./routes.js')(app);

const port = 3000;
const host = '0.0.0.0';
app.listen(port, host); // starts up the server on the configured host and port

console.log(`temp app running at http://${host}:${port}/`);