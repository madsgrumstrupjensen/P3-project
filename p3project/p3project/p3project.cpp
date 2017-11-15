// p3project.cpp : Defines the entry point for the console application.
// penis

#include "stdafx.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Statscreen.h"
#include "mainscreen.h"
#include "Game.h"
#include "Settings.h"


using namespace std;
using namespace cv;

int current_screen = 3;
string winName = "P3 Project";

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		if (x > 106 && x < 190)
		{
			if (y > 71 && y < 167)
			{
				current_screen = 1;
				cout << "home" << endl;
			}
			else if (y > 167 && y < 272)
			{
				current_screen = 2;
				cout << "profile" << endl;

			}
			else if (y > 167 && y < 372)
			{
				current_screen = 3;
				cout << "settings" << endl;

			}
			else if (y > 372 && y < 472)
			{
				current_screen = 4;
				cout << "quit" << endl;

			}
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

	namedWindow(winName, CV_WINDOW_AUTOSIZE);
	setMouseCallback(winName, CallBackFunc, NULL);
	Statscreen* test = new Statscreen();
	Homescreen* test2 = new Homescreen();
	Settings* test3 = new Settings();

	while (true) {
		if (current_screen == 1)
		{
			Mat hometest2 = test2->run();
			imshow(winName, hometest2);
		} else if (current_screen == 2)
		{
			Mat statest = test->run();
			imshow(winName, statest);
		} else if (current_screen == 3)
		{
			Mat settings = test3->run();
			imshow(winName, settings);
		} else if (current_screen == 4)
		{
			
		}
		waitKey();
	}
	/*
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
	*/
    return 0;
}

