// p3project.cpp : Defines the entry point for the console application.
// penis

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Statscreen.h"
#include "Settings.h"
#include "Homescreen.h"


using namespace std;
using namespace cv;

string current_screen = "profile";
string winName = "P3 Project";

Mat segmentation(Mat src) {
	for (int x = 0; x < src.rows; x++) {
		for (int y = 0; y < src.cols; y++) {
			if (src.at<Vec3b>(x, y) == Vec3b(255, 255, 255)) {
				src.at<Vec3b>(x, y)[0] = 0;
				src.at<Vec3b>(x, y)[1] = 0;
				src.at<Vec3b>(x, y)[2] = 0;
			}
		}
	}
	// Create a kernel that we will use for accuting/sharpening our image
	Mat kernel = (Mat_<float>(3, 3) <<
		1, 1, 1,
		1, -8, 1,
		1, 1, 1); // an approximation of second derivative, a quite strong kernel
				  // do the laplacian filtering as it is
				  // well, we need to convert everything in something more deeper then CV_8U
				  // because the kernel has some negative values,
				  // and we can expect in general to have a Laplacian image with negative values
				  // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
				  // so the possible negative number will be truncated
//	Mat imgLaplacian;
	//Mat sharp = src; // copy source image to another temporary one
	//filter2D(sharp, imgLaplacian, CV_32F, kernel);
	//src.convertTo(sharp, CV_32F);
	//Mat imgResult = sharp - imgLaplacian;
	// convert back to 8bits gray scale
	//imgResult.convertTo(imgResult, CV_8UC3);
	//imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
	//src = imgResult; // copy back
	// Create binary image from source image
	Mat blured;
	medianBlur(src, blured, 5);
	Mat bw;
	cvtColor(blured, bw, CV_BGR2GRAY);
	Mat tresh;
	threshold(bw, tresh, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	// Perform the distance transform algorithm
	/*Mat dist;
	distanceTransform(bw, dist, CV_DIST_L2, 3);
	// Normalize the distance image for range = {0.0, 1.0}
	// so we can visualize and threshold it
	normalize(dist, dist, 0, 1., NORM_MINMAX);
	// Threshold to obtain the peaks
	// This will be the markers for the foreground objects
	threshold(dist, dist, .4, 1., CV_THRESH_BINARY);
	// Dilate a bit the dist image
	Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	dilate(dist, dist, kernel1);
	// Create the CV_8U version of the distance image
	// It is needed for findContours()
	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);
	// Find total markers
	vector<vector<Point> > contours;
	findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	// Create the marker image for the watershed algorithm
	Mat markers = Mat::zeros(dist.size(), CV_32SC1);
	// Draw the foreground markers
	for (size_t i = 0; i < contours.size(); i++)
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1);
	// Draw the background marker
	circle(markers, Point(5, 5), 3, CV_RGB(255, 255, 255), -1);
	// Perform the watershed algorithm
	watershed(src, markers);
	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	bitwise_not(mark, mark);
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	// Create the result image
	Mat dst = Mat::zeros(markers.size(), CV_8UC3);
	// Fill labeled objects with random colors
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index > 0 && index <= static_cast<int>(contours.size()))
				dst.at<Vec3b>(i, j) = colors[index - 1];
			else
				dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
		}
	}*/
	src = tresh;
	return src;
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		//Anywhere
		if (x > 106 && x < 190)
		{
			if (y > 71 && y < 167)
			{
				current_screen = "home";
				cout << "home" << endl;
			}
			else if (y > 167 && y < 272)
			{
				current_screen = "profile";
				cout << "profile" << endl;

			}
			else if (y > 167 && y < 372)
			{
				current_screen = "settings";
				cout << "settings" << endl;

			}
			else if (y > 372 && y < 472)
			{
				current_screen = "quit";
				cout << "quit" << endl;

			}
		}
		//Within profile
		if (current_screen == "profile")
		{
			
		}
		//cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		//cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		//cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

	}
}

int main()
{

/*	namedWindow(winName, CV_WINDOW_AUTOSIZE);

	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	Mat dispImage;
	while (true) {
		if (!dispImage.empty()) { dispImage.release(); }
		setMouseCallback(winName, CallBackFunc, NULL);
		if (current_screen == "home")
		{
			dispImage = homescreen->run();
		} else if (current_screen == "profile")
		{
			dispImage = test->run();
			cout << "Profile: " << dispImage.rows << endl;
		} else if (current_screen == "settings")
		{
			
			dispImage = settingsscreen->run();
			cout << "Settings: " << dispImage.rows << endl;
		} else if (current_screen == "quit")
		{
	Mat img = imread("home.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file
	waitKey(1);
	char* source_image = "Source image";
	namedWindow(source_image, CV_WINDOW_AUTOSIZE);
	imshow(source_image, img);
	resizeWindow(source_image, 800, 600);
	cout << "testing" << endl;
	waitKey(0);
	//stat_screen();*/

	///  VVV is the camera loop VVV 
	Mat testing1;
	Mat testing2;
	Mat image = imread("img2.jpg");
	
	VideoCapture stream1(0); //change 0 to 1 to get from the plugged in camera instead of webcam.
	if (!stream1.isOpened())
	{
		cout << "Cannot open camera";
	}
	while (true)
	{
		stream1.read(testing1);

		testing2 = segmentation(image);
		
		imshow("cam", testing2);
		if ((waitKey(30) >= 0))
			break;
	}

	getchar();
    return 0;
}

