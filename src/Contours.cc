#include "Contours.h"
#include "OpenCV.h"
#include <nan.h>

#include <iostream>

Nan::Persistent<FunctionTemplate> Contour::constructor;

void Contour::Init(Local<Object> target) {
  Nan::HandleScope scope;

  // Class/contructor
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(Contour::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("Contour").ToLocalChecked());

  // Prototype
  // Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
  Nan::SetPrototypeMethod(ctor, "point", Point);
  Nan::SetPrototypeMethod(ctor, "points", Points);
  Nan::SetPrototypeMethod(ctor, "size", Size);
  Nan::SetPrototypeMethod(ctor, "area", Area);
  Nan::SetPrototypeMethod(ctor, "arcLength", ArcLength);
  Nan::SetPrototypeMethod(ctor, "approxPolyDP", ApproxPolyDP);
  Nan::SetPrototypeMethod(ctor, "convexHull", ConvexHull);
  Nan::SetPrototypeMethod(ctor, "boundingRect", BoundingRect);
  Nan::SetPrototypeMethod(ctor, "minAreaRect", MinAreaRect);
  Nan::SetPrototypeMethod(ctor, "fitEllipse", FitEllipse);
  Nan::SetPrototypeMethod(ctor, "isConvex", IsConvex);
  Nan::SetPrototypeMethod(ctor, "moments", Moments);
  Nan::SetPrototypeMethod(ctor, "saveSync", SaveSync);
  Nan::SetMethod(ctor, "loadSync", LoadSync);
  target->Set(Nan::New("Contour").ToLocalChecked(), ctor->GetFunction());
};

NAN_METHOD(Contour::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("Cannot instantiate without new");
  }

  Contour *contour;

  if (info[0]->IsArray()){
    Local<Array> contour_data = Local<Array>::Cast(info[0]);
    std::vector<cv::Point> points;
    int contour_length = contour_data->Length();

    for (int i = 0; i < contour_length; i++) {
      Local<Array> point_data = Local<Array>::Cast(contour_data->Get(i));
      int x = point_data->Get(0)->IntegerValue();
      int y = point_data->Get(1)->IntegerValue();
      points.push_back(cv::Point(x, y));
    }
    contour = new Contour(points);
  } else {
    contour = new Contour;
  }

  contour->Wrap(info.Holder());
  info.GetReturnValue().Set(info.Holder());
}

Contour::Contour() :
    Nan::ObjectWrap() {
}

Contour::Contour(std::vector<cv::Point> points):Nan::ObjectWrap() {
  contour = points;
}

NAN_METHOD(Contour::Point) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());
  int index = info[0]->NumberValue();

  cv::Point point = self->contour[index];

  Local<Object> data = Nan::New<Object>();
  data->Set(Nan::New("x").ToLocalChecked(), Nan::New<Number>(point.x));
  data->Set(Nan::New("y").ToLocalChecked(), Nan::New<Number>(point.y));

  info.GetReturnValue().Set(data);
}

NAN_METHOD(Contour::Points) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  Local<Array> data = Nan::New<Array>(self->contour.size());

  for (std::vector<int>::size_type i = 0; i != self->contour.size(); i++) {
    Local<Object> point_data = Nan::New<Object>();
    point_data->Set(Nan::New<String>("x").ToLocalChecked(), Nan::New<Number>(self->contour[i].x));
    point_data->Set(Nan::New<String>("y").ToLocalChecked(), Nan::New<Number>(self->contour[i].y));

    data->Set(i, point_data);
  }

  info.GetReturnValue().Set(data);
}

// FIXME: this should better be called "Length" as ``Contours`` is an Array like
// structure also, this would allow to use ``Size`` for the function returning
// the number of corners in the contour for better consistency with OpenCV.
NAN_METHOD(Contour::Size) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  info.GetReturnValue().Set(Nan::New<Number>(self->contour.size()));
}

NAN_METHOD(Contour::Area) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  info.GetReturnValue().Set(Nan::New<Number>(contourArea(cv::Mat(self->contour))));
}

NAN_METHOD(Contour::ArcLength) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());
  bool isClosed = info[0]->BooleanValue();

  info.GetReturnValue().Set(Nan::New<Number>(arcLength(cv::Mat(self->contour), isClosed)));
}

NAN_METHOD(Contour::ApproxPolyDP) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());
  double epsilon = info[0]->NumberValue();
  bool isClosed = info[1]->BooleanValue();

  cv::Mat approxed;
  approxPolyDP(cv::Mat(self->contour), approxed, epsilon, isClosed);
  approxed.copyTo(self->contour);

  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(Contour::ConvexHull) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  bool clockwise = info[0]->BooleanValue();

  cv::Mat hull;
  cv::convexHull(cv::Mat(self->contour), hull, clockwise);
  hull.copyTo(self->contour);

  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(Contour::BoundingRect) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  cv::Rect bounding = cv::boundingRect(cv::Mat(self->contour));
  Local<Object> rect = Nan::New<Object>();

  rect->Set(Nan::New("x").ToLocalChecked(), Nan::New<Number>(bounding.x));
  rect->Set(Nan::New("y").ToLocalChecked(), Nan::New<Number>(bounding.y));
  rect->Set(Nan::New("width").ToLocalChecked(), Nan::New<Number>(bounding.width));
  rect->Set(Nan::New("height").ToLocalChecked(), Nan::New<Number>(bounding.height));

  info.GetReturnValue().Set(rect);
}

NAN_METHOD(Contour::MinAreaRect) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  cv::RotatedRect minimum = cv::minAreaRect(cv::Mat(self->contour));

  Local<Object> rect = Nan::New<Object>();
  rect->Set(Nan::New("angle").ToLocalChecked(), Nan::New<Number>(minimum.angle));

  Local<Object> size = Nan::New<Object>();
  size->Set(Nan::New("height").ToLocalChecked(), Nan::New<Number>(minimum.size.height));
  size->Set(Nan::New("width").ToLocalChecked(), Nan::New<Number>(minimum.size.width));
  rect->Set(Nan::New("size").ToLocalChecked(), size);

  Local<Object> center = Nan::New<Object>();
  center->Set(Nan::New("x").ToLocalChecked(), Nan::New<Number>(minimum.center.x));
  center->Set(Nan::New("y").ToLocalChecked(), Nan::New<Number>(minimum.center.y));

  v8::Local<v8::Array> points = Nan::New<Array>(4);

  cv::Point2f rect_points[4];
  minimum.points(rect_points);

  for (unsigned int i=0; i<4; i++) {
    Local<Object> point = Nan::New<Object>();
    point->Set(Nan::New("x").ToLocalChecked(), Nan::New<Number>(rect_points[i].x));
    point->Set(Nan::New("y").ToLocalChecked(), Nan::New<Number>(rect_points[i].y));
    points->Set(i, point);
  }

  rect->Set(Nan::New("points").ToLocalChecked(), points);

  info.GetReturnValue().Set(rect);
}

NAN_METHOD(Contour::FitEllipse) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  if (self->contour.size() >= 5) {  // Minimum number for an ellipse
    cv::RotatedRect ellipse = cv::fitEllipse(cv::Mat(self->contour));

    Local<Object> jsEllipse = Nan::New<Object>();
    jsEllipse->Set(Nan::New("angle").ToLocalChecked(), Nan::New<Number>(ellipse.angle));

    Local<Object> size = Nan::New<Object>();
    size->Set(Nan::New("height").ToLocalChecked(), Nan::New<Number>(ellipse.size.height));
    size->Set(Nan::New("width").ToLocalChecked(), Nan::New<Number>(ellipse.size.width));
    jsEllipse->Set(Nan::New("size").ToLocalChecked(), size);

    Local<Object> center = Nan::New<Object>();
    center->Set(Nan::New("x").ToLocalChecked(), Nan::New<Number>(ellipse.center.x));
    center->Set(Nan::New("y").ToLocalChecked(), Nan::New<Number>(ellipse.center.y));
    jsEllipse->Set(Nan::New("center").ToLocalChecked(), center);

    info.GetReturnValue().Set(jsEllipse);
  }

  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(Contour::IsConvex) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  info.GetReturnValue().Set(Nan::New<Boolean>(isContourConvex(cv::Mat(self->contour))));
}

NAN_METHOD(Contour::Moments) {
  Nan::HandleScope scope;

  Contour *self = Nan::ObjectWrap::Unwrap<Contour>(info.This());

  // Get the moments
  cv::Moments mu = moments( self->contour, false );

  Local<Object> res = Nan::New<Object>();

  res->Set(Nan::New("m00").ToLocalChecked(), Nan::New<Number>(mu.m00));
  res->Set(Nan::New("m10").ToLocalChecked(), Nan::New<Number>(mu.m10));
  res->Set(Nan::New("m01").ToLocalChecked(), Nan::New<Number>(mu.m01));
  res->Set(Nan::New("m11").ToLocalChecked(), Nan::New<Number>(mu.m11));

  info.GetReturnValue().Set(res);
}

NAN_METHOD(Contour::LoadSync) {
  Nan::HandleScope scope;

  std::string filename = std::string(*Nan::Utf8String(info[0]->ToString()));
  cv::FileStorage fs(filename, cv::FileStorage::READ);
  std::vector<cv::Point> points;

  if (!fs.isOpened()) {
    Nan::ThrowError("Cannot load file");
  }

  cv::FileNode data = fs["points"];

  if (data.type() != cv::FileNode::SEQ) {
    Nan::ThrowError("Contour data is not a sequence!");
  }

  cv::FileNodeIterator it = data.begin(), it_end = data.end();

  for (; it != it_end; ++it) {
    points.push_back(cv::Point((int)(*it)["x"], (int)(*it)["y"]));
  }

  Local<Object> ret = Nan::New(Contour::constructor)->GetFunction()->NewInstance();
  Contour *contour = new Contour(points);
  contour->Wrap(ret);

  fs.release();

  info.GetReturnValue().Set(ret);
}

NAN_METHOD(Contour::SaveSync) {
  SETUP_FUNCTION(Contour);
  std::string filename = std::string(*Nan::Utf8String(info[0]->ToString()));
  cv::FileStorage fs(filename, cv::FileStorage::WRITE);

  if (!fs.isOpened()) {
    Nan::ThrowError("Cannot load file");
  }

  fs << "points" << "[";
  for (std::vector<int>::size_type i = 0; i != self->contour.size(); i++) {
    fs << "{:" << "x" << self->contour[i].x << "y" << self->contour[i].y << "}";
  }
  fs << "]";

  fs.release();

  info.GetReturnValue().Set(Nan::Null());
}