const express = require('express');

const app = express();

// Add headers
app.use(function(request, response, next) {
  response.setHeader('Access-Control-Allow-Origin', "http://" + request.headers.host + ':3000');
  response.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');
  response.setHeader('Access-Control-Allow-Headers', 'X-Requested-With,content-type');
  next();
});

require('./routes.js')(app);

const port = 3000;
const host = '0.0.0.0';
app.listen(port, host); // starts up the server on the configured host and port

console.log(`temp app running at http://${host}:${port}/`);