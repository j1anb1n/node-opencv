#include "OpenCV.h"
#include "Matrix.h"
#include <nan.h>

void OpenCV::Init(Local<Object> target) {
  Nan::HandleScope scope;

  // Version string.
  char out [21];
  int n = sprintf(out, "%i.%i", CV_MAJOR_VERSION, CV_MINOR_VERSION);
  target->Set(Nan::New<String>("version").ToLocalChecked(), Nan::New<String>(out, n).ToLocalChecked());

  Nan::SetMethod(target, "readImage", ReadImage);
  Nan::SetMethod(target, "findContours", FindContours);
}

NAN_METHOD(OpenCV::ReadImage) {
  Nan::EscapableHandleScope scope;

  REQ_FUN_ARG(1, cb);

  Local<Value> argv[2];
  argv[0] = Nan::Null();

  Local<Object> im_h = Nan::New(Matrix::constructor)->GetFunction()->NewInstance();
  Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(im_h);
  argv[1] = im_h;

  try {
    cv::Mat mat;

    if (info[0]->IsNumber() && info[1]->IsNumber()) {
      int width, height;

      width = info[0]->Uint32Value();
      height = info[1]->Uint32Value();
      mat = *(new cv::Mat(width, height, CV_64FC1));

    } else if (info[0]->IsString()) {
      std::string filename = std::string(*Nan::Utf8String(info[0]->ToString()));
      mat = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);

    } else if (Buffer::HasInstance(info[0])) {
      uint8_t *buf = (uint8_t *) Buffer::Data(info[0]->ToObject());
      unsigned len = Buffer::Length(info[0]->ToObject());

      cv::Mat *mbuf = new cv::Mat(len, 1, CV_64FC1, buf);
      mat = cv::imdecode(*mbuf, CV_LOAD_IMAGE_UNCHANGED);

      if (mat.empty()) {
        argv[0] = Nan::Error("Error loading file");
      }
    }

    img->mat = mat;
  } catch (cv::Exception& e) {
    argv[0] = Nan::Error(e.what());
    argv[1] = Nan::Null();
  }

  Nan::TryCatch try_catch;
  cb->Call(Nan::GetCurrentContext()->Global(), 2, argv);

  if (try_catch.HasCaught()) {
    Nan::FatalException(try_catch);
  }

  return;
}

NAN_METHOD(OpenCV::FindContours) {
  Nan::HandleScope scope;

  if (info.Length() < 3) {
    Nan::ThrowTypeError("Argument length error");
  }

  int mode = cv::RETR_LIST;
  int chain = cv::CHAIN_APPROX_SIMPLE;

  cv::Mat mat;

  try {
    if (Buffer::HasInstance(info[0])) {
      uint8_t *buf = (uint8_t *) Buffer::Data(info[0]->ToObject());
      unsigned len = Buffer::Length(info[0]->ToObject());
      cv::Mat *mbuf = new cv::Mat(len, 1, CV_64FC1, buf);
      mat = cv::imdecode(*mbuf, -1);
    } else {
      Matrix *_img = Nan::ObjectWrap::Unwrap<Matrix>(info[0]->ToObject());
      mat = (_img->mat).clone();
    }
  } catch (cv::Exception& e) {
    const char* msg = e.what();
    Nan::ThrowError(msg);
  }

  if (mat.empty()) {
    Nan::ThrowTypeError("Error loading file");
  }

  if (info[1]->IsNumber()) {
    mode = info[1]->IntegerValue();
  }

  if (info[2]->IsNumber()) {
    chain = info[2]->IntegerValue();
  }

  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(mat, contours, hierarchy, mode, chain);
  Local<Array> contours_data = Nan::New<Array>(contours.size());

  for (std::vector<int>::size_type i = 0; i != contours.size(); i++) {
    std::vector<cv::Point> points = contours[i];
    Local<Array> contour_data = Nan::New<Array>(points.size());

    for (std::vector<int>::size_type j = 0; j != points.size(); j++) {
      Local<Array> point_data = Nan::New<Array>(2);
      point_data->Set(0, Nan::New<Number>(points[j].x));
      point_data->Set(1, Nan::New<Number>(points[j].y));

      contour_data->Set(j, point_data);
    }
    contours_data->Set(i, contour_data);
  }

  Local<Array> hierarchy_data = Nan::New<Array>(hierarchy.size());
  for (std::vector<int>::size_type i = 0; i != hierarchy.size(); i++) {
    Local<Array> contour_data = Nan::New<Array>(4);
    contour_data->Set(0, Nan::New<Number>(hierarchy[i][0]));
    contour_data->Set(1, Nan::New<Number>(hierarchy[i][1]));
    contour_data->Set(2, Nan::New<Number>(hierarchy[i][2]));
    contour_data->Set(3, Nan::New<Number>(hierarchy[i][3]));

    hierarchy_data->Set(i, contour_data);
  }

  Local<Object> data = Nan::New<Object>();
  data->Set(Nan::New<String>("contours").ToLocalChecked(), contours_data);
  data->Set(Nan::New<String>("hierarchy").ToLocalChecked(), hierarchy_data);

  info.GetReturnValue().Set(data);
}