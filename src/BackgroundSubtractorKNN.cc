#include "OpenCV.h"
#include "BackgroundSubtractorKNN.h"
#include "Matrix.h"
#include <nan.h>
#include <iostream>

Nan::Persistent<FunctionTemplate> BackgroundSubtractorKNNWrap::constructor;

void BackgroundSubtractorKNNWrap::Init(Local<Object> target) {
    Nan::HandleScope scope;

    // Class/contructor
    Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(BackgroundSubtractorKNNWrap::New);
    constructor.Reset(ctor);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(Nan::New("BackgroundSubtractorKNN").ToLocalChecked());

    Nan::SetPrototypeMethod(ctor, "apply", Apply);

    target->Set(Nan::New("BackgroundSubtractorKNN").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(BackgroundSubtractorKNNWrap::New) {
    Nan::HandleScope scope;

    if (info.This()->InternalFieldCount() == 0) {
      Nan::ThrowTypeError("Cannot instantiate without new");
    }

    int history = 200;
    double threshold = 400.0;
    bool detectShadow = true;

    if (info[0]->IsNumber()) {
        history = info[0]->IntegerValue();
    }

    if (info[1]->IsNumber()) {
        threshold = info[1]->NumberValue();
    }

    if (info[2]->IsBoolean()) {
        detectShadow = info[2]->BooleanValue();
    }

    cv::Ptr<cv::BackgroundSubtractorKNN> bs =
        cv::createBackgroundSubtractorKNN(history, threshold, detectShadow);
    BackgroundSubtractorKNNWrap *v = new BackgroundSubtractorKNNWrap(bs);

    v->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

BackgroundSubtractorKNNWrap::BackgroundSubtractorKNNWrap(
    cv::Ptr<cv::BackgroundSubtractorKNN> _subtractor) {
    subtractor = _subtractor;
}

NAN_METHOD(BackgroundSubtractorKNNWrap::Apply) {
    SETUP_FUNCTION(BackgroundSubtractorKNNWrap);

    if (info.Length() == 0) {
        Nan::ThrowTypeError("Input image missing");
    }

    try {
        Local<Object> fgMask =
            Nan::New(Matrix::constructor)->GetFunction()->NewInstance();
        Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(fgMask);

        cv::Mat mat;

        if (Buffer::HasInstance(info[0])) {
          uint8_t *buf = (uint8_t *) Buffer::Data(info[0]->ToObject());
          unsigned len = Buffer::Length(info[0]->ToObject());
          cv::Mat *mbuf = new cv::Mat(len, 1, CV_64FC1, buf);
          mat = cv::imdecode(*mbuf, -1);
        } else {
          Matrix *_img = Nan::ObjectWrap::Unwrap<Matrix>(info[0]->ToObject());
          mat = (_img->mat).clone();
        }

        if (mat.empty()) {
          return Nan::ThrowTypeError("Error loading file");
        }

        cv::Mat _fgMask;
        self->subtractor->apply(mat, _fgMask);

        img->mat = _fgMask;
        mat.release();

        info.GetReturnValue().Set(fgMask);
    } catch (cv::Exception& e) {
        const char* msg = e.what();
        Nan::ThrowError(msg);
    }
}