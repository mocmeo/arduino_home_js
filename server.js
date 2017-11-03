var express = require('express');
var app = express();
var http = require('http').Server(app);
var five = require('johnny-five');
var board = new five.Board();
var io = require('socket.io')(http);


app.use(express.static(__dirname + '/public'));
app.get('/', function(req, res) {
    res.sendFile(__dirname + '/index.html');
});


board.on('ready', function() {
    var led = new five.Led.RGB({
        pins: {
            red: 11,
            green: 10,
            blue: 9
        }
    });
    led.on();
    led.color("#FF0000");

    var setStateColor = function(state) {
        led.color({
          red: state.value.r,
          green: state.value.g,
          blue: state.value.b
      });
    };

    io.on('connection', function(client) {
        client.on('join', function(handshake) {
            console.log(handshake);
        });

        client.on('hex', function(data) {
            led.color('#' + data.value);
        });
    });
});

var port = process.env.PORT || 3000;
http.listen(3000, function(req, res) {
    console.log(`Server listening on port: http://localhost:${port}`);
});