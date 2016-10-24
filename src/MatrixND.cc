#include "OpenCV.h"
#include "MatrixND.h"
#include <nan.h>

Nan::Persistent<FunctionTemplate> MatrixND::constructor;

void MatrixND::Init(Local<Object> target) {
  Nan::HandleScope scope;

  // Class/contructor
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(MatrixND::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("MatrixND").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "saveSync", SaveSync);
  Nan::SetMethod(ctor, "loadSync", LoadSync);

  target->Set(Nan::New("MatrixND").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(MatrixND::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("Cannot instantiate without new");
  }

  MatrixND *res = new MatrixND(cv::MatND());

  res->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

MatrixND::MatrixND(cv::MatND _mat) {
  mat = _mat;
}

MatrixND::MatrixND() {

}

NAN_METHOD(MatrixND::LoadSync) {
  Nan::HandleScope scope;

  std::string filename = std::string(*Nan::Utf8String(info[0]->ToString()));
  cv::FileStorage fs(filename, cv::FileStorage::READ);
  cv::MatND mat;

  if (!fs.isOpened()) {
    Nan::ThrowError("Cannot load file");
  }

  fs["matrix"] >> mat;

  Local<Object> ret = Nan::New(MatrixND::constructor)->GetFunction()->NewInstance();
  MatrixND *res = new MatrixND(mat);
  res->Wrap(ret);

  info.GetReturnValue().Set(ret);
}

NAN_METHOD(MatrixND::SaveSync) {
  SETUP_FUNCTION(MatrixND)

  std::string filename = std::string(*Nan::Utf8String(info[0]->ToString()));
  cv::FileStorage fs(filename, cv::FileStorage::WRITE);

  if (!fs.isOpened()) {
    Nan::ThrowError("Cannot load file");
  }

  fs << "matrix" << self->mat;

  fs.release();

  info.GetReturnValue().Set(Nan::Null());
}