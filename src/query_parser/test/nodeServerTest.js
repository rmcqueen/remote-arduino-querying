var chai= require('chai');
var chaiHttp = require('chai-http');
var server = require('../server.js');
var should = chai.should();
var expect = require('chai').expect;
chai.use(chaiHttp);

describe('Server connection.', () => {
    it('it should connect to the node.js server', (done) => {
        chai.request(server)
            .get('/publish_query/')
            .query({queryString: 'CREATE TABLE team(name string)', targets: 'Arduino1'})
            .end(function(err, res) {
                expect(res).to.have.status(200);
                should.equal(err, null);
                done();
            });
    });

    it('it should not expect a post request', (done) => {
        chai.request(server)
            .post('/publish_query/')
            .query({queryString: 'CREATE TABLE team(name string)', targets: 'Arduino1'})
            .end(function(err, res) {
                expect(res).to.have.status(404);
                done();
            });
    });

    it('it should not connect to the node.js server with the wrong IP', (done) => {
        chai.request('http://127.0.1:3000')
            .get('/publish_query/')
            .query({queryString: 'CREATE TABLE team(name string)', targets: 'Arduino1'})
            .end(function(err, res) {
                expect(res).to.be.undefined;
                done();
            });
    }).timeout(5000);

});