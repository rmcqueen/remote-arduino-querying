const publishQueryData = require('./publishQueryData');

module.exports = (app) => {
  /**
   * normal usage would look like this
   * const versionRoute = require('./routes/version');
   * app.get('/version', versionRoute);
   * The idea is that responders should not be kept with the routes, only routes go here
   */
  app.get('/publish_query', (req, res) => {
    const queryString = decodeURIComponent(res.queryString);
    publishQueryData(queryString);
  });
};