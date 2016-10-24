#include "OpenCV.h"

class Contour: public Nan::ObjectWrap {
public:
  cv::Mat mat;
  std::vector<cv::Point> contour;

  static Nan::Persistent<FunctionTemplate> constructor;
  static void Init(Local<Object> target);
  static NAN_METHOD(New);
  static NAN_METHOD(LoadSync);

  Contour();
  Contour(std::vector<cv::Point>);

  JSFUNC(Point)
  JSFUNC(Points)
  JSFUNC(Size)
  JSFUNC(Area)
  JSFUNC(ArcLength)
  JSFUNC(ApproxPolyDP)
  JSFUNC(ConvexHull)
  JSFUNC(BoundingRect)
  JSFUNC(MinAreaRect)
  JSFUNC(FitEllipse)
  JSFUNC(IsConvex)
  JSFUNC(Moments)
  JSFUNC(SaveSync)
};

