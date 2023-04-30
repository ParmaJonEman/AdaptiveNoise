#include "main.h"

int main(int argc, char** argv) {
    String imageFile1;
    String imageFile2;
    String imageFile3;
    int howNoisy;

    if(!parseParameters(argc, argv, &imageFile1, &imageFile2, &imageFile3, &howNoisy)){
        return 0;
    }

    Mat originalImage = imread(imageFile1, 0);

    imshow("Original Image", originalImage);
    waitKey();

    Mat saltImage = createSalt(originalImage, howNoisy/2);
    imshow("Salt image", saltImage);
    waitKey();

    Mat pepperImage = createPepper(originalImage, howNoisy/2);
    imshow("Pepper image", pepperImage);
    waitKey();

    Mat noisyImage;
    bitwise_or(originalImage, saltImage, noisyImage);

    bitwise_and(noisyImage, pepperImage, noisyImage);

    imshow("Salty image", noisyImage);
    waitKey();

    Mat adaptiveMeanFilteredImage = adaptiveMeanFilter(noisyImage);
    Mat adaptiveMedianFilteredImage = adaptiveMedianFilter(noisyImage, 3);
    imwrite(imageFile2, adaptiveMeanFilteredImage);
    imwrite(imageFile3, adaptiveMedianFilteredImage);

    Mat meanDiff;
    Mat medianDiff;
    absdiff(noisyImage, adaptiveMeanFilteredImage, meanDiff);
    int sumOfDiffs;
    int sz = meanDiff.rows * meanDiff.cols;
    for(int i = 0; i < sz; i++){
        sumOfDiffs+=(int) meanDiff.data[i];
    }
    int averageDiff = sumOfDiffs/sz;
    cout << "Euclidean distance adaptive mean " << averageDiff << endl;

    //sum all difference values and divide by number of pixels
    absdiff(noisyImage, adaptiveMedianFilteredImage, medianDiff);

    int sumOfDiffsMedian;
    int szMedian = medianDiff.rows * medianDiff.cols;
    for(int i = 0; i < sz; i++){
        sumOfDiffsMedian+=(int) medianDiff.data[i];
    }
    int averageDiffMedian = sumOfDiffsMedian/szMedian;
    cout << "Euclidean distance adaptive median " << averageDiffMedian << endl;

    imshow("Mean Diff", meanDiff);
    imshow("Median Diff", medianDiff);
    waitKey();
    return 0;
}

Mat createSalt(Mat original, int howSalty){
    Mat salt = cv::Mat::zeros(original.size(), CV_8U);
    for(int i = 0; i < original.rows; i++){
        for(int j = 0; j < original.cols; j++){
            int rando = rand() % 100;
            if(rando < howSalty){
                salt.at<char>(i, j) = 0XFF;
            }
        }
    }
    return salt;
}

Mat createPepper(Mat original, int howPeppery){
    Mat pepper = Mat(original.size(), CV_8U);
    pepper.setTo(0XFF);
    for(int i = 0; i < original.rows; i++){
        for(int j = 0; j < original.cols; j++){
            int rando = rand() % 100;
            if(rando < howPeppery){
                pepper.at<char>(i, j) = 0X00;
            }
        }
    }
    return pepper;
}

Mat adaptiveMedianFilter(Mat noisyImage, int maxKernel){
    int pad = maxKernel / 2;
    Mat paddedImage;
    Mat cleanImage = noisyImage.clone();
    copyMakeBorder(noisyImage, paddedImage, pad, pad, pad, pad, BORDER_CONSTANT);
    for(int i = pad; i < paddedImage.rows - pad; i++){
        for(int j = pad; j < paddedImage.cols - pad; j++){
            char med;
            char max;
            char min;
            for(int k = 2; k <= maxKernel; k++) {
                int currentPad = k/2;
                Mat kernel = paddedImage(Rect(j - currentPad, i - currentPad, k, k));
                vector<char> pixels;
                for(int kRows = 0; kRows < kernel.rows; kRows++){
                    for(int kCols = 0; kCols < kernel.cols; kCols++){
                        pixels.push_back(kernel.at<char>(kRows, kCols));
                    }
                }
                size_t n = pixels.size() / 2;
                nth_element(pixels.begin(), pixels.begin()+n, pixels.end());
                med = pixels[n];
                max = * max_element(pixels.begin(), pixels.end());
                min = * min_element(pixels.begin(), pixels.end());
                if(min >= med || med >= max){
                  k++;
                }
                else{
                  if((min < paddedImage.at<char>(i, j)) && (paddedImage.at<char>(i,j) < max)){
                      cleanImage.at<char>(i - pad,j - pad) = paddedImage.at<char>(i,j);
                  }
                  else{
                      cleanImage.at<char>(i - pad,j - pad) = med;
                  }
                }
            }
                cleanImage.at<char>(i - pad,j - pad) = med;
        }
    }
    imshow("Output Median", cleanImage);
    waitKey();
    return cleanImage;
}

Mat adaptiveMeanFilter(Mat noisyImage){
    Mat cleanImage = noisyImage.clone();
    int kernelSize = 4;
    int pad = kernelSize / 2;
    Mat paddedImage;
    copyMakeBorder(noisyImage, paddedImage, pad, pad, pad, pad, BORDER_CONSTANT);
    Scalar globalMean, globalStddev;
    meanStdDev(noisyImage, globalMean, globalStddev);
    double globalVariance = pow(globalStddev[0], 2);
    for (int i = 0; i < noisyImage.rows; i++) {
        for (int j = 0; j < noisyImage.cols; j++) {

            Mat kernel = paddedImage(Rect(j, i, kernelSize, kernelSize));

            Scalar localMean, localStddev;
            meanStdDev(kernel, localMean, localStddev);
            float localVariance = pow(localStddev[0], 2);
            double varianceRatio;
            if(globalVariance > localVariance){
                varianceRatio = 1;
            }
            else{
                varianceRatio = globalVariance/localVariance;
            }
            cleanImage.at<char>(i, j) = noisyImage.at<char>(i, j) - ((varianceRatio) * (noisyImage.at<char>(i, j) - localMean[0]));
        }
    }
    imshow("Output Mean", cleanImage);
    waitKey();
    return cleanImage;
}


static int parseParameters(int argc, char** argv, String* imageFile1, String* imageFile2, String* imageFile3, int* howNoisy)
{
    String keys =
            {
                    "{help h usage ? |                            | print this message   }"
                    "{n      || how noisy the image should be	}"
                    "{@imageFile1      || image you want to filter	}"
                    "{@imageFile2      || filename you want to save adaptive mean result as	}"
                    "{@imageFile3      || filename you want to save adaptive median result as	}"
            };

    // Get required parameters. If any are left blank, defaults are set based on the above table
    // If no image file is passed in, or if the user passes in a help param, usage info is printed
    CommandLineParser parser(argc, argv, keys);
    parser.about("adaptive v1.0");

    if (parser.has("help"))
    {
        parser.printMessage();
        return(0);
    }
    if (parser.has("n"))
    {
        *howNoisy = parser.get<int>("n");
        if(*howNoisy > 90 || *howNoisy < 1){
            cout << "Must select number between 1 and 90 for noise level" << endl;
            return 0;
        }
    }
    else{
        *howNoisy = 10;
    }

    if (parser.has("@imageFile1"))
    {
        *imageFile1 = parser.get<String>("@imageFile1");
    }
    else
    {
        parser.printMessage();
        return(0);
    }

    if (parser.has("@imageFile2"))
    {
        *imageFile2 = parser.get<String>("@imageFile2");
    }
    else
    {
        *imageFile2 = "outputMean.jpg";
    }

    if (parser.has("@imageFile3"))
    {
        *imageFile3 = parser.get<String>("@imageFile3");
    }
    else
    {
        *imageFile3 = "outputMedian.jpg";
    }

    return(1);
}
