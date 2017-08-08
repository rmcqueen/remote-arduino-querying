const publishQueryData = require('./publishQueryData.js');
const { parseResultSet } = require('./lib.js');
const { applyWhere } = require('./applyWhere.js');

module.exports = (app) => {
  /**
   * normal usage would look like this
   * const versionRoute = require('./routes/version');
   * app.get('/version', versionRoute);
   * The idea is that responders should not be kept with the routes, only routes go here
   */
  app.get('/publish_query', (req, res) => {
    console.log(req.query);
    const queryString = req.query.queryString.replace(/;$/, ''); // remove terminal semicolon if present
    const targets =  req.query.targets;
    publishQueryData(queryString, targets)
      .then(parseResultSet)
      .then(parsedResultSet => applyWhere(parsedResultSet, queryString))
      .then(result => res.send(result));
  });
};