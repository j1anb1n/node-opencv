var cv = require('../lib/opencv');
var img = cv.imread(__dirname + '/image/1.jpeg');
var window = new cv.NamedWindow('win', 0);
var oriWin = new cv.NamedWindow('origin', 0);
var origin = img.copy();
// img.gaussianBlur().cvtColor('CV_BGR2GRAY');
// var ret = img
// .findContours(
//   cv.Constants.RETR_FLOODFILL,
//   cv.Constants.CHAIN_APPROX_SIMPLE
// );

// origin.drawAllContours(ret.contours, [0, 0, 255], 5);

//
img
.gaussianBlur()
.floodFill({
  seedPoint: [1, 1],
  newColor: [0,0,0],
  loDiff : [1,1,1],
  upDiff:[1,1,1]
});

img
  .cvtColor('CV_BGR2GRAY');
  // .threshold(0, 255, cv.Constants.THRESH_BINARY );

var canny = img.canny(0, 30);

var ret = img
  .findContours(
    cv.Constants.RETR_EXTERNAL,
    cv.Constants.CHAIN_APPROX_SIMPLE
  );
origin.drawAllContours(ret.contours, [255, 0, 0], 3);
//
// canny.cvtColor('CV_GRAY2BGR').findContours
// circles.forEach(circle => {
//   origin.circle(circle, [255,0,0], 3);
// });
// console.log(circles);
//         // .cvtColor('CV_BGR2GRAY')
//         // .threshold(0, 250, cv.Constants.THRESH_BINARY_INV + cv.Constants.THRESH_OTSU);

window.show(canny);
// oriWin.show(origin);
window.blockingWaitKey(0, 0);