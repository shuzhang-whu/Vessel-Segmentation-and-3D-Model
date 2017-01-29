#ifndef VESSEL_EXTRACT_H
#define VESSEL_EXTRACT_H
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  
#include <iostream>
#include <list>
#include <cmath>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include<fstream>
#define PI 3.1416
using namespace cv;
using namespace std;
 extern "C" _declspec(dllexport) cv::Mat getLineMask(int theta, int masksize);
 extern "C" _declspec(dllexport) cv::Mat rotatex(cv::Mat mask);
 extern "C" _declspec(dllexport) cv::Mat getBaseMask(int theta, int masksize);
 extern "C" _declspec(dllexport) cv::Mat drawLine(cv::Point pt0, cv::Point pt1, cv::Mat orgmask);
 extern "C" _declspec(dllexport) cv::Mat getLinePts(cv::Point pt0, cv::Point pt1); //·µ»Ø¶þÎ¬¾ØÕó;
 extern "C" _declspec(dllexport) cv::Mat grow_boundary(cv::Mat img, cv::Mat mask, int erosionsize = 11, int iterations = 50);
 extern "C" _declspec(dllexport) cv::Mat global_normalize(cv::Mat img, cv::Mat mask);
 extern "C" _declspec(dllexport) cv::Mat get_lineresponse(cv::Mat img, int linescale, int w = 15);
 extern "C" _declspec(dllexport) cv::Mat vessel_extract(cv::Mat img, cv::Mat mask_img);
 extern "C" _declspec(dllexport) cv::Mat find_connected(cv::Mat img, int area);
#endif