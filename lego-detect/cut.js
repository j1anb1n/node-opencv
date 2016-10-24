var cv = require('../lib/opencv');

module.exports = function (img, cont) {
  var size = img.size();
  var height = size[0];
  var width = size[1];
  var rect = cont.minAreaRect();
  var { angle } = rect;
  var maxSize = Math.ceil(Math.sqrt(Math.pow(height, 2)+Math.pow(width, 2)));
  var xs = rect.points.map(p => p.x);
  var ys = rect.points.map(p => p.y);
  var x1 = Math.min.apply(null, xs);
  var y1 = Math.min.apply(null, ys);
  var x2 = Math.max.apply(null, xs);
  var y2 = Math.max.apply(null, ys);
  var blankImage = cv.Matrix.Zeros(maxSize, maxSize, img.type());
  var center = [
    (x1 + x2) / 2,
    (y1 + y2) / 2
  ];

  size = [x2-x1, y2-y1];
  var offsetX = (maxSize - size[0]) / 2;
  var offsetY = (maxSize - size[1]) / 2;
  sub = img.getRectSubPix(size, center);
  console.log(sub.size(), blankImage.size());
  sub.copyTo(blankImage, offsetX, offsetY );
  height = rect.size.height;
  width = rect.size.width;

  if (height < width) {
    angle += 90;
  }

  blankImage.rotate(angle, offsetX + size[0] / 2, offsetY + size[1] / 2);
  return blankImage.getRectSubPix(
    [Math.min(height, width), Math.max(height, width)],
    [offsetX + size[0]/2, offsetY + size[1] / 2]
  );
}