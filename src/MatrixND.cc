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