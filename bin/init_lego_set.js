var fs = require('fs');
var _ = require('underscore');
var request = require('request');
var mkdirp = require('mkdirp');

const SET_ID = '42039';

function readFile (filename) {
  return new Promise((res, rej) => {
    fs.readFile(filename, (err, buffer) => {
      if (err) {
        rej(err);
        return;
      }

      var content = buffer
                    .toString()
                    .split('\r\n')
                    .filter(l => !!l.trim());
      var title = content[0]
                    .trim()
                    .split('\t');
      var parts = content
        .splice(1)
        .map(row => {
          return row
            .trim()
            .split('\t')
            .reduce((ret, item, i) => {
              ret[title[i]] = item;
              return ret;
            }, {});
        });
      res(parts);
    });
  });
}

function readCode () {
  return readFile(__dirname + '/../data/lego/codes.txt')
    .then(CODES=> {
      return CODES.reduce((ret, row) => {
        if (!ret[row['Item No']]) {
          ret[row['Item No']] = [];
        }
        ret[row['Item No']].push({
          color: row['Color'],
          code: row['Code']
        });
        return ret;
      }, {});
    });
}

function readColor () {
  return readFile(__dirname + '/../data/lego/colors.txt')
    .then(COLORS => {
      return COLORS.reduce((ret, row) => {
        ret[row['Color ID']] = row['Color Name'];
        return ret;
      }, {});
    });
}

Promise.all([
  readCode(),
  readColor(),
  readFile(`${__dirname}/../data/lego/set/${SET_ID}.txt`)
])
  .then(
    (data) => {
      const CODES = data[0];
      const COLORS = data[1];
      const SET = data[2];

      var parts = SET.map(row => {
        var itemId = row['Item No'];
        var colorId = row['Color ID'];
        var colorName = COLORS[colorId];
        var code = _.find(CODES[itemId], i => i['color'] == colorName) || {};
        var image = `http://img.bricklink.com/ItemImage/PN/${colorId}/${itemId}.png`;

        return {
          image,
          colorName,
          colorId,
          itemId,
          code: code.code
        };
      });

      return parts.filter(p => !!p.code );
    }
  )
  .then(parts => {
    return Promise.all(
      parts
        .map(part => {
          var dir = `${__dirname}/../database/lego/parts/${part.code}/`;
          mkdirp.sync(dir);
          return new Promise((res, rej) => {
            request(part.image)
              .pipe(fs.createWriteStream(`${dir}/image.png`))
              .on('close', res)
              .on('error', rej);
          });
        })
    );
  })
  .catch(err => console.error(err));
