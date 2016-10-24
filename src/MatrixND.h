#ifndef __NODE_MATRIXND_H
#define __NODE_MATRIXND_H
#include "OpenCV.h"
#if CV_MAJOR_VERSION >= 3

class MatrixND:public Nan::ObjectWrap {
public:
  cv::MatND mat;
  static Nan::Persistent<FunctionTemplate> constructor;
  static void Init(Local<Object> target);
  static NAN_METHOD(New);

  MatrixND();
  MatrixND(cv::MatND);

  JSFUNC(SaveSync);
  JSFUNC(LoadSync);
};

#endif
#endif