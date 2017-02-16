#ifndef __NODE_GENERALIZEDHOUGH_H
#define __NODE_GENERALIZEDHOUGH_H
#include "OpenCV.h"
#include <opencv2/imgproc.hpp>

class GeneralizedHoughGuilWrap:public Nan::ObjectWrap {
public:
  cv::Ptr<cv::GeneralizedHoughGuil> ptr;
  static Nan::Persistent<FunctionTemplate> constructor;
  static void Init(Local<Object> target);
  static NAN_METHOD(New);

  GeneralizedHoughGuilWrap(cv::Ptr<cv::GeneralizedHoughGuil>);

  JSFUNC(Detect)
  JSFUNC(SetTemplate)
};

#endif