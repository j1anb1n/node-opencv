var cv = require('./opencv');
var cutImageByContour = require('./cut-image-by-contour');
var { createUUID } = require('./util');
module.exports = function (originImage) {
  var image = originImage
        .copy()
        .cvtColor('CV_BGR2GRAY')
        .gaussianBlur()
        .threshold(0, 10, cv.Constants.THRESH_BINARY_INV + cv.Constants.THRESH_OTSU)
        .dilate(cv.Constants.MORPH_ELLIPSE, 3);

  var ret = image.findContours(
    cv.Constants.RETR_EXTERNAL,
    cv.Constants.CHAIN_APPROX_SIMPLE
  );

  var contours = ret.contours
    .filter(c => c.area() > 50);

  var objects = contours
    .map((c) => {
      var cut = cutImageByContour(originImage, c);
      var rect = c.boundingRect();
      var points = c.points();
      var shapeImage = cv.Matrix.Zeros(rect.height + 10, rect.width + 10, originImage.type());
      points = points.map(point => [point.x - rect.x + 5, point.y - rect.y + 5]);

      shapeImage.drawContour(new cv.Contour(points), [0, 255, 255], 3);

      return {
        uuid: createUUID(),
        image: cut,
        contour: c,
        canny: cut.copy()
                  .cvtColor('CV_BGR2GRAY')
                  .gaussianBlur()
                  .threshold(0, 220, cv.Constants.THRESH_BINARY_INV + cv.Constants.THRESH_OTSU)
                  .canny(),
        hist: cut.calcHist(),
        shapeImage
      };
    });

  return objects;
};