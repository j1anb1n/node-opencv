var fs = require('fs');
var cv = require('./opencv');
var mkdirp = require('mkdirp');
var path = require('path');
var basePath = `${__dirname}/../database`;

class Item {
  constructor (uuid) {
    var dir = [];
    this.uuid = uuid;
    this.faces = [];

    this.image = cv.imread(`${basePath}/${uuid}/image.png`);
    try {
      dir = fs.readdirSync(`${basePath}/${uuid}/faces`)
              .filter(p => p[0] != '.');
    } catch (ex) {
      /* ignore */
    }
    this.faces = dir
      .map((faceId) => {
        var facePath    = path.resolve(`${basePath}/${uuid}/faces/${faceId}`);
        var contour = cv.Contour.loadSync(`${facePath}/contour.yml`);
        var hist    = cv.MatrixND.loadSync(`${facePath}/hist.yml`);
        var image   = cv.imread(`${facePath}/image.jpg`);

        return { uuid: faceId, contour, hist, image };
      });
  }
  save () {
    this.faces.forEach((face) => {
      var dir = `${basePath}/${this.uuid}/faces/${face.uuid}`;
      mkdirp.sync(dir);
      face.contour.saveSync(`${dir}/contour.yml`);
      face.hist.saveSync(`${dir}/hist.yml`);
      face.image.save(`${dir}/image.jpg`);
      console.log('saved');
    });
  }
  addFace (face) {
    this.faces.push(face);
  }
  match (obj) {
    return this.faces.map((face) => {
      var contour = cv.matchShape(
        face.contour,
        obj.contour,
        cv.Constants.CV_CONTOURS_MATCH_I3
      );
      var hist = cv.compareHist(face.hist, obj.hist, cv.Constants.HISTCMP_HELLINGER);
      return {
        face,
        score: {
          contour,
          hist
        }
      };
    });
  }
}

Item.match = function (a, b) {
  var contScore = cv.matchShape(a.contour, b.contour, 1);
  var histScore = cv.compareHist(a.hist, b.hist, cv.Constants.HISTCMP_HELLINGER);

  return {
    contScore,
    histScore
  };
};

module.exports = Item;