var cv = require('./opencv');
var cutImageByContour = require('./cut-image-by-contour');
var createUUID = require('./util').createUUID;

module.exports = function (originImage) {
  var size = originImage.size();
  var image = originImage
        .copy()
        .cvtColor('CV_BGR2GRAY')
        .threshold(0, 220, cv.Constants.THRESH_BINARY_INV + cv.Constants.THRESH_OTSU)
        .dilate(cv.Constants.MORPH_ELLIPSE, 3);

  var ret = image.findContours(
    cv.Constants.RETR_EXTERNAL,
    cv.Constants.CHAIN_APPROX_SIMPLE
  );

  var contours = ret.contours
    .filter(c => c.area() > 50)
    .sort(c => c.area());

  var objects = contours
    .map((c) => {
      var cut = cutImageByContour(originImage, c);
      var shapeImage = cv.Matrix.Zeros(size[0], size[1], originImage.type());

      shapeImage.drawContour(c, [0, 255, 255], 3);
      shapeImage = cutImageByContour(shapeImage, c);

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