var cv = require('../lib/opencv');

var template = cv.imread(__dirname + '/image/3.png', 0);
var query = cv.imread(__dirname + '/image/2.png', 0);

var guil = new cv.GeneralizedHoughGuil({
  levels: 360,
  angleThresh: 10000,
  scaleThresh: 1000,
  posThresh: 100,
  maxBufSize: 1000,
  minDist: 100,
  dp: 2,
  minScale: 0.5,
  maxScale: 2,
  scaleStep: 0.05,
  minAngle: 0,
  maxAngle: 360,
  angleStep: 1
});

guil.setTemplate(template);
var pos = guil.detect(query);

console.log(pos);