const chai= require('chai');
const chaiHttp = require('chai-http');
const server = require('../server.js');
const should = chai.should();
const expect = require('chai').expect;
chai.use(chaiHttp);

describe('Server connection.', () => {

    // I need to test this at home before I do a pull request

    it('should pass if an arduino is connected and moqutte is running', (done) => {
        chai.request(server)
            .get('/publish_query/')
            .query({queryString: 'CREATE TABLE team(name string)', targets: 'Arduino1'})
            .end(function(err, res) {
                expect(res).to.have.status(200);
                should.equal(err, null);
                done();
            });
    });

    it('should not expect a post request', (done) => {
        chai.request(server)
            .post('/publish_query/')
            .query({queryString: 'CREATE TABLE team(name string)', targets: 'Arduino1'})
            .end(function(err, res) {
                expect(res).to.have.status(404);
                done();
            });
    });

    it('should not connect to the node.js server with the wrong IP', (done) => {
        chai.request('http://127.0.1:3000')
            .get('/publish_query/')
            .query({queryString: 'CREATE TABLE team(name string)', targets: 'Arduino1'})
            .end(function(err, res) {
                expect(res).to.be.undefined;
                done();
            });
    }).timeout(5000);

    it('should connect to the node.js server', (done) => {
        chai.request(server)
            .get('/server_connection_test')
            .end(function(err, res) {
                expect(res).to.have.status(200);
                done();
            });
    });
});