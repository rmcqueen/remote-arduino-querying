const pg = require('pg');
const connectionString = process.env.DATABASE_URL || 'postgres://localhost:5432/arduino_query_tracker';

function track(arduinoID) {
    pg.connect(connectionString, (err, client, done) => {
        if(err) {
            done();
            console.log(err);
            return;
        }
        client.query('INSERT INTO clients(arduinoID, status, updated_at) values($1, $2, $3)',
            [arduinoID, 'Processing Query', generateTimeStamp()]);
        query.on('end', () => {
            done();
            return;
        });
    });
}


function updateStatus(arduinoID, status) {
    pg.connect(connectionString, (err, client, done) => {
        if(err) {
            done();
            console.log(err);
            return;
        }
        // Since we want to preserve the history of our query statuses, we insert a new row instead
        client.query('INSERT INTO clients(arduinoID, status, updated_at) values($1, $2, $3)', 
            [arduinoID, status, generateTimeStamp]);
    });
}


function getStatus(arduinoIDs) {
     pg.connect(connectionString, (err, client, done) => {
        if(err) {
            done();
            console.log(err);
            return;
        }

        const query = client.query('SELECT * FROM clients WHERE arduinoID=($1)', 
            [arduinoID]);

        query.on('row', (row) => {
            results.push(row);
        });

        query.on('end', () => {
            done();
            return results;
        });
    });
}


function generateTimeStamp() {
    var date = new Date();

    var hour = date.getHours();
    hour = (hour < 10 ? "0": "") + hour;

    var min  = date.getMinutes();
    min = (min < 10 ? "0" : "") + min;
    
    var sec  = date.getSeconds();
    sec = (sec < 10 ? "0" : "") + sec;

    var year = date.getFullYear();

    var month = date.getMonth() + 1;
    month = (month < 10 ? "0" : "") + month;

    var day  = date.getDate();
    day = (day < 10 ? "0" : "") + day;

    return year + ":" + month + ":" + day + ":" + hour + ":" + min + ":" + sec;
}