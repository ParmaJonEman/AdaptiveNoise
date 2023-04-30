//
// Created by jonem on 3/4/2023.
//

#ifndef ADAPTIVE_MAIN_H
#define ADAPTIVE_MAIN_H
#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <algorithm>
#include <iterator>
using namespace cv;
using namespace std;
static int parseParameters(int argc, char** argv, String* imageFile1, String* imageFile2, String* imagefile3, int* howNoisy);
Mat createSalt(Mat original, int howSalty);
Mat createPepper(Mat original, int howPeppery);
Mat adaptiveMeanFilter(Mat noisyImage);
Mat adaptiveMedianFilter(Mat noisyImage, int maxKernel);

#endif //ADAPTIVE_MAIN_H
