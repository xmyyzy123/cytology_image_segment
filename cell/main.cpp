/**
* @file main.cpp                     
* @author Laihui Ding / xmyyzy123@gmail.com
* @version 1.0
* @date 2021-4-15
* @brief Using the *Phansalkar* method to segment carbon black particles in cytology images
* @details Using the *Phansalkar* method to segment carbon black particles in cytology images:
	1. First, split original image into three channels(R, G, B).
	2. Blue Channel image which is easier to be segmented as input for  the *Phansalkar* method.
* @see  https://www.researchgate.net/publication/224226466_Adaptive_local_thresholding_for_detection_of_nuclei_in_diversity_stained_cytology_images
		https://github.com/fiji/Auto_Local_Threshold/blob/master/src/main/java/fiji/threshold/Auto_Local_Threshold.java
		https://stackoverflow.com/questions/11456565/opencv-mean-sd-filter
*/

#include <vector>
#include <cmath>
#include <iostream>
#include <time.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

/**
* This is the implementation of Phansalskar N. et al. Adaptive local thresholding for detection of nuclei in diversity stained
  cytology images.International Conference on Communications and Signal Processing (ICCSP), 2011, 
  218 - 220. 
  In this method, the threshold t = mean*(1+p*exp(-q*mean)+k*((stdev/r)-1))
  Phansalkar recommends k = 0.25, r = 0.5, p = 2 and q = 10.
* @param img: image to be processed
* @param size: window size
*/
void Phansalkar(cv::Mat& img, int size)
{
	//clock_t start_time = clock();

	int height = img.rows;
	int width = img.cols;

	// Recommended value in Phansalkar's paper
	double k_value = 0.25;
	double r_value = 0.5;
	double p_value = 2.0;
	double q_value = 10.0;

	// Normalize images to 0 - 1.0
	cv::Mat img_clone;
	img.convertTo(img_clone, CV_32FC1);
	normalize(img_clone, img_clone, 0, 1, NORM_MINMAX);

	// Median filter image
	cv::Mat median_img;
	blur(img_clone, median_img, Size(size, size));

	// Variance image
	cv::Mat var_img;
	blur(img_clone.mul(img_clone), var_img, Size(size, size));

	// E[X-u)^2]=E(X^2)-[E(X)]^2
	cv::Mat sigma;
	cv::sqrt(cv::abs(var_img - median_img.mul(median_img)), sigma);

	// threshold t = mean*(1+p*exp(-q*mean)+k*((stdev/r)-1))
	for (int i = 0; i < height; i++)
	{
		float* p = img_clone.ptr<float>(i);
		float* p_mean = median_img.ptr<float>(i);
		float* p_std = sigma.ptr<float>(i);
		for (int j = 0; j < width; j++)
		{
			double data = p[j];
			double thredshold = p_mean[j] * (1.0 + p_value * exp(-q_value * p_mean[j]) + k_value * ((p_std[j] / r_value) - 1.0));
			img.at<uchar>(i, j) = data >  thredshold ? 255 : 0;
		}
	}

	//clock_t end_time = clock();
	//cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << std::endl;
}

int main()
{
	Mat srcimg = imread("./high_density.tif", -1);

	// get blue channel image by split channels
	vector<Mat> channels;               
	split(srcimg, channels);      
	Mat blueImage = channels.at(0);     
	Mat greenImage = channels.at(1);    
	Mat redImage = channels.at(2);      

	//imshow("blueImage", blueImage);
	//imshow("greenImage", greenImage);
	//imshow("redImage", redImage);
	//waitKey(0);
	
	// 20 is window size, can be changed to other value according to images.
	Phansalkar(blueImage, 20);

	//cv::imwrite("high_density.jpg", blueImage);
	imshow("Phansalkar", blueImage);
	waitKey(0);

	return 0;
}
