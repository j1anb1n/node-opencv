#include "OpenCV.h"
#include "GeneralizedHough.h"
#include "Matrix.h"
#include <nan.h>
#include <opencv2/imgproc.hpp>

Nan::Persistent<FunctionTemplate> GeneralizedHoughGuilWrap::constructor;

void GeneralizedHoughGuilWrap::Init(Local<Object> target) {
  Nan::HandleScope scope;

  // Class/contructor
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(GeneralizedHoughGuilWrap::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("GeneralizedHoughGuil").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "detect", Detect);
  Nan::SetPrototypeMethod(ctor, "setTemplate", SetTemplate);

  target->Set(Nan::New("GeneralizedHoughGuil").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(GeneralizedHoughGuilWrap::New) {
  Nan::HandleScope scope;
  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("GeneralizedHoughGuil cannot instantiate without new");
    return;
  }

  Local<Object> config = info[0]->ToObject();
  int levels         = config->Get(Nan::New<String>("levels").ToLocalChecked())->IntegerValue();
  int angleThresh    = config->Get(Nan::New<String>("angleThresh").ToLocalChecked())->IntegerValue();
  int scaleThresh    = config->Get(Nan::New<String>("scaleThresh").ToLocalChecked())->IntegerValue();
  int posThresh      = config->Get(Nan::New<String>("posThresh").ToLocalChecked())->IntegerValue();
  int maxBufSize     = config->Get(Nan::New<String>("maxBufSize").ToLocalChecked())->IntegerValue();
  double minDist     = config->Get(Nan::New<String>("minDist").ToLocalChecked())->NumberValue();
  double dp          = config->Get(Nan::New<String>("dp").ToLocalChecked())->NumberValue();
  double minScale    = config->Get(Nan::New<String>("minScale").ToLocalChecked())->NumberValue();
  double maxScale    = config->Get(Nan::New<String>("maxScale").ToLocalChecked())->NumberValue();
  double scaleStep   = config->Get(Nan::New<String>("scaleStep").ToLocalChecked())->NumberValue();
  double minAngle    = config->Get(Nan::New<String>("minAngle").ToLocalChecked())->NumberValue();
  double maxAngle    = config->Get(Nan::New<String>("maxAngle").ToLocalChecked())->NumberValue();
  double angleStep   = config->Get(Nan::New<String>("angleStep").ToLocalChecked())->NumberValue();

  cv::Ptr<cv::GeneralizedHoughGuil> guil = cv::createGeneralizedHoughGuil();

  guil->setMinDist(minDist);
  guil->setLevels(levels);
  guil->setDp(dp);
  guil->setMaxBufferSize(maxBufSize);

  guil->setMinAngle(minAngle);
  guil->setMaxAngle(maxAngle);
  guil->setAngleStep(angleStep);
  guil->setAngleThresh(angleThresh);

  guil->setMinScale(minScale);
  guil->setMaxScale(maxScale);
  guil->setScaleStep(scaleStep);
  guil->setScaleThresh(scaleThresh);

  guil->setPosThresh(posThresh);

  GeneralizedHoughGuilWrap *res = new GeneralizedHoughGuilWrap(guil);
  res->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

GeneralizedHoughGuilWrap::GeneralizedHoughGuilWrap(cv::Ptr<cv::GeneralizedHoughGuil> _ptr) {
  ptr = _ptr;
}

NAN_METHOD(GeneralizedHoughGuilWrap::SetTemplate) {
  SETUP_FUNCTION(GeneralizedHoughGuilWrap)
  Matrix *image = Nan::ObjectWrap::Unwrap<Matrix>(info[0]->ToObject());

  self->ptr->setTemplate(image->mat);

  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GeneralizedHoughGuilWrap::Detect) {
  SETUP_FUNCTION(GeneralizedHoughGuilWrap)
  Matrix *image = Nan::ObjectWrap::Unwrap<Matrix>(info[0]->ToObject());

  std::vector<cv::Vec4f> position;
  self->ptr->detect(image->mat, position);

  Local<Array> ret = Nan::New<Array>(position.size());
  int cols = image->mat.cols;
  int rows = image->mat.rows;

  for (std::vector<int>::size_type i = 0; i < position.size(); i++) {
    cv::RotatedRect rect;
    cv::Point2f pos(position[i][0], position[i][1]);
    float scale = position[i][2];
    float angle = position[i][3];
    float width = cols * scale;
    float height = rows * scale;

    rect.center = pos;
    rect.size = cv::Size2f(width, height);
    rect.angle = angle;

    cv::Point2f pts[4];
    rect.points(pts);

    Local<Object> item   = Nan::New<Object>();
    Local<Object> center = Nan::New<Object>();
    Local<Object> size   = Nan::New<Object>();
    Local<Array> points  = Nan::New<Array>(4);

    center->Set(Nan::New<String>("x").ToLocalChecked(), Nan::New<Number>(position[i][0]));
    center->Set(Nan::New<String>("y").ToLocalChecked(), Nan::New<Number>(position[i][1]));

    size->Set(Nan::New<String>("width").ToLocalChecked(), Nan::New<Number>(width));
    size->Set(Nan::New<String>("height").ToLocalChecked(), Nan::New<Number>(height));

    for (unsigned int i=0; i<4; i++) {
      Local<Object> point = Nan::New<Object>();
      point->Set(Nan::New<String>("x").ToLocalChecked(), Nan::New<Number>(pts[i].x));
      point->Set(Nan::New<String>("y").ToLocalChecked(), Nan::New<Number>(pts[i].y));

      points->Set(i, point);
    }

    item->Set(Nan::New<String>("center").ToLocalChecked(), center);
    item->Set(Nan::New<String>("size").ToLocalChecked(), size);
    item->Set(Nan::New<String>("points").ToLocalChecked(), points);
    item->Set(Nan::New<String>("angle").ToLocalChecked(), Nan::New<Number>(position[i][3]));
    ret->Set(i, item);
  }

  info.GetReturnValue().Set(ret);
}