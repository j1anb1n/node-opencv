var cv = require('../lib/opencv');
var window = new cv.NamedWindow('img', 0);

cv.readImage('/Users/chenjianbin/Downloads/icon_nangua.png', (err, im) => {
  if (err) {
    console.error(err);
    return;
  }

  var img = im.copy();

  img = img.cvtColor('CV_BGR2GRAY')
        .threshold(0, 100, 'Binary')
        .dilate(cv.Constants.MORPH_ELLIPSE, 3);

  var ret = img.findContours(
    cv.Constants.RETR_EXTERNAL,
    cv.Constants.CHAIN_APPROX_SIMPLE
  );

  ret.contours.sort(c => c.area())[0].saveSync('/Users/chenjianbin/Downloads/cont.yml');

  var cont = cv.Contour.loadSync('/Users/chenjianbin/Downloads/cont.yml');

  im.drawContour(cont, [255, 0, 0], 3);

  window.show(im);
  window.blockingWaitKey(0, 0);
});