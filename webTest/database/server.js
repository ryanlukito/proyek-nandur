const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql');
const cors = require('cors');

const app = express();
const port = 3100;

const db = mysql.createConnection({
    host:'localhost',
    user:'root',
    password:'',
    database:"project-nandur"
});

db.connect(err => {
    if (err) {
        throw err;
    }
    console.log('Connected to database');
});

app.use(cors());
app.use(bodyParser.json());

app.listen(port, () => {
    console.log(`Server running at port ${port}`);
})

app.post('/save-data', (req,res) => {
    const {Temperature, Humidity} = req.body;
    const query = "INSERT INTO datanandur (create_time, temperature, humidity) VALUES (NOW(), ?, ?)";
    db.query(query, [Temperature, Humidity], (err, result) => {
        if (err) {
            console.error('Failed to insert data:', err);
            res.status(500).send('Failed to insert data');
            return;
        }
        res.status(200).send('Data saved successfully');
    });
});

