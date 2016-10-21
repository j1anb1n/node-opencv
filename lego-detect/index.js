var cv = require('../lib/opencv');
var camera = new cv.VideoCapture(0);
var window = new cv.NamedWindow('Video', 0);
var bs = new cv.BackgroundSubtractorKNN();

function loop () {
    camera.read((err, frame) => {
        if (err) throw err;

        var size = frame.size();
        if (size[0] * size[1] > 0) {
            var mask = bs.apply(frame);

            window.show(mask);
        }
        window.blockingWaitKey(0, 50);
        loop();
    })
}

loop();