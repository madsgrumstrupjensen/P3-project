// p3project.cpp : Defines the entry point for the console application.
// penis

#include "stdafx.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <iostream>


using namespace std;
using namespace cv;

void stat_screen()
{
	//test
	Mat img = imread("home.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file


	waitKey(1);
	char* source_image = "Source image";
	namedWindow(source_image, CV_WINDOW_AUTOSIZE);
	imshow(source_image, img);
	getchar();
}

int main()
{
	Mat img = imread("home.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file
	waitKey(1);
	char* source_image = "Source image";
	namedWindow(source_image, CV_WINDOW_AUTOSIZE);
	imshow(source_image, img);
	resizeWindow(source_image, 800, 600);
	cout << "testing" << endl;
	waitKey(0);
	//stat_screen();
	Mat testing;

	getchar();
    return 0;
}

