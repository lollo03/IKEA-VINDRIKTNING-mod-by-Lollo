// 1 DATABASE AIRQUALITY
// 1 TABELLA (data):
// |Data rivelazione|Dati...|DEVICEID|
// 2 TABELLA (devices):
// |deviceID|FRIENDLYNAME|LASTIPING|token|

const express = require('express')
const mariadb = require('mariadb');
var bodyParser = require('body-parser')
var cors = require('cors');
require("dotenv").config();

const app = express()
app.use(bodyParser.json());       // to support JSON-encoded bodies
app.use(bodyParser.urlencoded({     // to support URL-encoded bodies
    extended: true
}));
app.use(cors())

const port = 80
const regOnlyNumbers = /^\d+(\.\d+)*$$/
const regNumbersAndLetters = /^[a-zA-Z0-9]*$/

const pool = mariadb.createPool({
    host: process.env.mariadb_host,
    user: process.env.mariadb_user,
    password: process.env.mariadb_password,
    database: process.env.mariadb_database,
    connectionLimit: 5
});

async function connectDB() {
    return await pool.getConnection()
}

async function checkToken(token) {
    conn = pool
    let res = await conn.query("SELECT token FROM `devices`")
    let tokenCache = res.map(function (res) {
        return res['token'];
    });
    if (tokenCache.includes(token)) {
        return 1
    }
    return 0
}


app.post('/new', async (req, res) => {
    var ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress
    console.log(`Ricevuto /new da ${ip}`)
    if (req.headers["token"] && !req.headers["token"].match(regNumbersAndLetters)) {
        console.log("La richiesta non ha passato i controlli regex!")
        res.sendStatus(401)
        return
    }
    if (!await checkToken(req.headers["token"]) || !req.headers["token"]) { //il token non è nel db
        console.log("Access token non valido!")
        res.sendStatus(401)
        return
    }
    console.log("Token riconosciuto, scrivo nel database")
    if (!req.body["PM25"] || !req.body["deviceID"]) {
        console.log("La richiesta non ha i parametri body necessari!")
        res.sendStatus(400)
        return
    }
    if (!req.body["PM25"].match(regOnlyNumbers) || !req.body["deviceID"].match(regOnlyNumbers)) {
        console.log("La richiesta non ha passato i controlli regex!")
        res.sendStatus(400)
        return
    }
    conn = pool
    try { //un bel catch-all
        await conn.query(`INSERT INTO data (dateTime, PM25, deviceID) VALUE(NOW(), ${req.body["PM25"]}, ${req.body["deviceID"]});`)
    } catch (e) {
        console.log(e);
        res.sendStatus(500)
        return
    }
    console.log('Scrittura completata...')
    res.sendStatus(200)
})

app.listen(port, async () => {
    await connectDB()
    console.log(`App avviata su ${port}, database connesso!`)
})