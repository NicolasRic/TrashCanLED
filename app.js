const express = require('express')
const got = require('got');
const config = require('./config.json');
const app = express()
const port = 5000

app.get('/TrashCanLED/next/', function (req, res) {
    got(config.API).then(response => {
        next = JSON.parse(response.body)["next_event"]["flags"]
        msg = []

        for (let i in next) {
            if(next[i]["subject"] == "Recycling") 
                msg.push(config.Recycle_colour)
            else if(next[i]["subject"] == "Organics")
                msg.push(config.Organic_colour)
            else if (next[i]["subject"] == "Garbage")
                msg.push(config.Garbage_colour)
            else
                console.log("Unknown param: " + next[i]["subject"])
        }
        msg.push(msg[0])
        res.send(msg)
    }).catch(err => {
        console.log(err)
        res.send(config.Cycle) 
    });
})

app.get('/', function (req, res) {
    res.send("Hello")
})

app.listen(port, () => console.log(`App listening on port ${port}!`))