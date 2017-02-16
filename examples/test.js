var cv = require('../lib/opencv');
var window = new cv.NamedWindow('circles', 0);
var img = cv.imread('/Users/chenjianbin/dev/wdd/lego-brick-recognition/database/multi/2.jpeg');

img
  .floodFill({
    seedPoint: [1, 1],
    newColor: [255, 255, 255],
    loDiff : [3,3,3],
    upDiff:[5,5,5]
  });
window.show(img);
window.blockingWaitKey(0, 0);