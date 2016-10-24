var cv = require('../lib/opencv');
var cutImageByContour = require('./cut');

// var camera = new cv.VideoCapture('/rtsp://192.168.1.127:8554/unicast');
var camera = new cv.VideoCapture(0);
var window = new cv.NamedWindow('Video', 0);
var cutWindow = new cv.NamedWindow('Cut', 0);
var bs = new cv.BackgroundSubtractorKNN();

var firstFrame = null;
var prevHist = null;

function loop() {
  camera.read((err, originFrame) => {
    if (err) throw err;
    var size = originFrame.size();
    if (size[0] * size[1] == 0) {
      setTimeout(loop, 50);
      return;
    }
    var frame = originFrame.copy();

    if (!firstFrame) {
      firstFrame = originFrame.copy();
    }

    frame = frame.cvtColor('CV_BGR2GRAY')
         .threshold(0, 177, 'Binary Inverted', 'Otsu')
         .dilate(cv.Constants.MORPH_ELLIPSE, 3);

    var ret = frame.findContours(
      cv.Constants.RETR_EXTERNAL,
      cv.Contours.CHAIN_APPROX_SIMPLE
    );

    var cont = ret.contours
       .sort(c => c.area());

    let cut = cutImageByContour(originFrame.copy(), cont[0]);
    var hist = cut.calcHist([0, 1, 2], [8,8,8], [0, 256, 0, 256, 0, 256]);
    if (prevHist) {
      console.log(cv.compareHist(hist, prevHist));
    }
    prevHist = hist;

    cutWindow.show(cut);

    window.show(frame);
    window.blockingWaitKey(0, 50);
    loop();
  })
}

loop();