#pragma once
#include <vector>
#include <assert.h>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

#define Bd1 20
#define Bd2 30
#define Bu1 225
#define Bu2 235
#define kkk 28
#define k_f 0.7
#define k_s (1-k_f)
#define k_gain 2 //ÔöÒæµ÷œÚ³¬²ÎÊý
#define maxgain 100
#define k_p 0.85

class AutoExposure : public Algorithm {
public:

    void setCurrExpTime(float currExpTime);
    float getCurrExpTime();
    vector<float> getNextExpTime(Mat& img, float Speed);
    vector<float> getMyNextExpTime(Mat& img, float speed);
private:
    //Camera* mpCamera;
    float mCurrExpTime = 2;
    float mCurrGain = 0;

    float mLambda = 10;
    float mDelta = 0.050;
    vector<float> mvAnchorGammas = { 0.10, 0.50, 1.0, 1.50, 2.0, 2.50, 3.0 };
    float mKp = 0.750;
    int mSize = 76800;


    void gammaCorrection(Mat& img, Mat& out, float gamma);
    void logMapping(Mat& img, Mat& out);
    float computeIQ(Mat& img);
    bool polyFit(vector<cv::Point2f>& points, int n, Mat& A);
    float getOptimGamma(Mat& img);
    vector<float> control_ExpTime(float ExpTime, float Speed,float Gain_k);
    void calhiststatics(cv::Mat hi);
    int Cb, Cb1,Cm, Cw, Cw1;
    float lastExpTime, lastGain,lastave, ave;
};

//vector<float> control_ExpTime(float ExpTime, float Speed);
vector<float> readTxt(string file);
int writeTxt(vector<vector<float> > data);
vector<Mat> ReadImage(cv::String pattern);
