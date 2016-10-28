var fs = require('fs');
var EventEmitter = require('events');
var express = require('express');
var http = require('http');
var SocketIO = require('socket.io');
var _ = require('underscore');
var SegfaultHandler = require('segfault-handler');

var Item = require('./lib/item');
var cv = require('./lib/opencv');
var objectDetector = require('./lib/object-detector');

SegfaultHandler.registerHandler("crash.log", function(signal, address, stack) {
  console.log(signal, address, stack);
});

var app = express();
var server = http.Server(app);
var io = SocketIO(server);
var camera = new cv.VideoCapture('rtsp://172.16.20.120:8554/unicast');
var videoEmitter = new EventEmitter();

function encode_base64 (img) {
  return 'data:image/jpeg;base64,'+img.toBuffer().toString('base64');
}
// const IMAGE_DIR = '/Users/chenjianbin/dev/wdd/lego-brick-recognition/database/multi';
const ITEM_DIR = `${__dirname}/database`;
// var images = fs.readdirSync(IMAGE_DIR)
//                .filter(p => p[0] != '.')
//                .map(p => cv.imread(`${IMAGE_DIR}/${p}`));
// var currentIndex = 0;

var items = fs.readdirSync(ITEM_DIR)
              .filter(p => p[0] != '.')
              .map(p => new Item(p));

function getNextImage (callback) {
  camera.read(callback);
}

server.listen(8123);
console.log('listen...');

app.use('/static', express.static('static', {
    fallthrough: false
}));

app.get('/', function (req, res) {
  res.sendfile(__dirname + '/index.html');
});

io.on('connection', function (socket) {
  var objects = [];

  socket.emit('initItems', items.map(item => {
    console.log(item.uuid, item.image);
    return {
      uuid: item.uuid,
      image: encode_base64(item.image)
    };
  }));

  socket.on('manualRecognize', (data) => {
    var item = _.find(items, item => item.uuid == data.item);
    var obj = _.find(objects, obj => obj.uuid == data.obj);
    item.addFace({
      uuid: data.obj,
      image: obj.image,
      contour: obj.contour,
      hist: obj.hist
    });

    item.save();
  });

  socket.on('getNextFrame', function () {
    var img = getNextImage();
    objects = objectDetector(img.copy());
    var detectImage = img.copy();
    if (objects.length) {
      socket.emit('objectFound', objects.map(obj => {
        return {
          uuid: obj.uuid,
          image: encode_base64(obj.image),
          shapeImage: encode_base64(obj.shapeImage),
          canny: encode_base64(obj.canny)
        };
      }));

      objects
        .map(obj => obj.contour.minAreaRect())
        .forEach(rect => {
          for (var i = 0; i < 4; i++) {
            let p1 = rect.points[i];
            let p2 = rect.points[(i+1) % 4];
            detectImage.line([p1.x, p1.y], [p2.x, p2.y], [255, 0,0], 5);
          }
        });

      setTimeout(function () {
        objects.forEach(obj => {
          var score = items
            .map(item => {
              return {
                uuid: item.uuid,
                image: item.image,
                faces: item.match(obj)
              };
            })
            .reduce((ret, item) => {
                if (item.faces.length) {
                  ret.push({
                    uuid: item.uuid,
                    image: encode_base64(item.image),
                    faces: item.faces.map(face => {
                      return {
                        uuid: face.face.uuid,
                        score: face.score,
                        image: encode_base64(face.face.image)
                      };
                    })
                  });
                }

                return ret;
            }, []);
          socket.emit('score', {
            uuid: obj.uuid,
            score
          });
        });
      }, 0);
    }
    socket.emit('frame', {
      image: encode_base64(detectImage)
    });
  });

  videoEmitter.on('objectFound', function (data) {
    socket.emit('objectFound', data.map(obj => {
      return {
        image: encode_base64(obj.image)
      };
    }));
  });

  videoEmitter.on('frame', (data) => {
    socket.emit('frame', {
      image: encode_base64(data.image)
    });
  });
});