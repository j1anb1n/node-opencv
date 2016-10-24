var cv = require('./lib/opencv');
var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);

var camera = new cv.VideoCapture('rtsp://192.168.1.127:8554/unicast');

server.listen(8123);

app.get('/', function (req, res) {
  res.sendfile(__dirname + '/index.html');
});

io.on('connection', function (socket) {
  socket.emit('connected');
  socket.on('take', function (data) {
    camera.read((err, frame) => {
      if (err) {
        return;
      }

      socket.emit('toke', { data: frame.toBuffer().toString('base64')});
    });
  });
});