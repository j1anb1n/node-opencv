#ifndef __NODE_BACKGROUNDSUBTRACTORKNN_H
#define __NODE_BACKGROUNDSUBTRACTORKNN_H
#include "OpenCV.h"
#if CV_MAJOR_VERSION >= 3
#include <opencv2/bgsegm.hpp>
// #include <opencv2/background_segm.hpp>

class BackgroundSubtractorKNNWrap:public Nan::ObjectWrap {
public:
  cv::Ptr<cv::BackgroundSubtractorKNN> subtractor;
  static Nan::Persistent<FunctionTemplate> constructor;
  static void Init(Local<Object> target);
  static NAN_METHOD(New);

  BackgroundSubtractorKNNWrap(cv::Ptr<cv::BackgroundSubtractorKNN>);

  JSFUNC(Apply)
};

#endif
#endif