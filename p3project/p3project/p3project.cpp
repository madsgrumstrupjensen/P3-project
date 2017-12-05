// p3project.cpp : Defines the entry point for the console application.
// penis

#include "stdafx.h"
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Statscreen.h"
#include "Settings.h"
#include "Homescreen.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <datetimeapi.h>
#include "Shape.h"
#include <random>
#include <thread>
#include <chrono>
#include <Mmsystem.h>


using namespace std;
using namespace cv;
using namespace this_thread; // sleep_for, sleep_until
using namespace chrono; // nanoseconds, system_clock, seconds


string current_screen = "home";
string winName = "P3 Project";


int erosion_elem = 1;
int erosion_size = 5;
int dilation_elem = 0;
int dilation_size = 1;
int const max_elem = 2;
int const max_kernel_size = 21;
Mat src, erosion_dst, dilation_dst, fist_dst;
Mat bn;
Mat blured;
Mat tresh, tresh_bin;
Mat spawningprompt = imread("textspawn.png");
Mat timer0 = imread("Fistbar.png");
Mat timer1 = imread("fistbar2.png");
Mat timer2 = imread("fistbar3.png");
Mat timer3 = imread("fistbar4.png");
Mat timer4 = imread("fistbar5.jpg");
int threshold_value = 17;
int threshold_type = 0;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
int timer;

int width = 800;
int height = 600;
int stepSize = 78;
int projectorStepSize = 45;
int difficulty = 2;

int maxStepSize = 150;
int basex = 96;
int const maxbase = 100 * 5;
int maxbasex = basex + 5 * projectorStepSize;
int basey = 58;
int maxbasey = basey + 5 * projectorStepSize;
int gridSize = 5;
int const gridSizeMax = 15;
Mat dispImage;
Mat gridimage;
Mat grid;
int t = 0;
clock_t beginClock;
clock_t fistClock;
bool clickSpawn = false;
bool checktime = false;
bool checktimefist = false;
bool fistclicked = false;
bool win = false;
bool lose = false;
bool showshapeimgs = false;
bool spawnfeedback = false;
bool startGame = false;
double fist_secs;


//Storage for shapes in projector
vector<Shape*>* shapeinput = new vector<Shape*>(25);

vector<Shape*>* shapeCamera = new vector<Shape*>(25);
// Storage for blobs
vector<KeyPoint> squarekeypoints;
vector<KeyPoint> trianglekeypoints;
vector<KeyPoint> circlekeypoints;
vector<KeyPoint> fistkeypoints;

int comparing[5][5] = {};

/** Function Headers */
void Threshold_Demo(int, void*);
void Grid_Demo(int, void*);
void Erosion(int, void*);
void Dilation(int, void*);
void GridCheckingsAndSuch();
void CameraGrid();
void checkPlacement();
char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

Mat takeBackground(Mat input, Mat currentBackground, Mat segmentedImage)
{
	bool blank = true;
	int whitecount = 0;
	for (int x = 0; x < segmentedImage.rows; x++)
	{
		for (int y = 0; y < segmentedImage.cols; y++)
		{
			if (segmentedImage.at<uchar>(x, y) != 0)
			{
				blank = false;
				whitecount++;
			}
		}
	}
	if (blank == false && whitecount > 10)
	{
		cout << "no bg take" << endl;
		return currentBackground;
	}
	cout << "New bg taken" << endl;
	return input;
}

SimpleBlobDetector::Params find_params(string type)
{
	SimpleBlobDetector::Params output;
	//Set up default parameters

	// Filter by Area.
	output.filterByArea = false;
	output.minArea = 50;
	output.maxArea = 700;

	// Filter by Convexity
	output.filterByConvexity = true;
	output.minConvexity = 0.9;

	// Filter by Inertia
	output.filterByInertia = false;
	output.minInertiaRatio = 0.01;

	// Filter by colour
	output.filterByColor = true;
	output.blobColor = 255;

	if (type == "square")
	{
		output.filterByCircularity = true;
		output.minCircularity = 0.80;
		output.maxCircularity = 0.85;
	} else if (type == "triangle")
	{
		output.filterByCircularity = true;
		output.minCircularity = 0.05;
		output.maxCircularity = 0.79;
	}
	else if (type == "circle")
	{
		//Circle
		output.filterByCircularity = true;
		output.minCircularity = 0.86;
		output.maxCircularity = 1;
	}
	else if (type == "fist")
	{
		output.filterByArea = false;
		output.filterByCircularity = true;
		output.minCircularity = 0.05;
		output.maxCircularity = 0.78;
	}
	return output;
}

void segmentation()
{
	//Declaring variables
	bool take_background_b = true;
	Mat camera_input_mat;
	Mat background_mat = imread("background.jpg");
	Mat grid_output_mat;
	Mat greyscale_mat;
	clock_t take_background_timer_start_clock;

	VideoCapture stream(1); //change 0 to 1 to get from the plugged in camera instead of webcam.
	if (!stream.isOpened()) cout << "Cannot open camera";

	while (true)
	{
		if (take_background_b)
		{
			take_background_timer_start_clock = clock();
			take_background_b = false;
		}

		stream.read(camera_input_mat);
		flip(camera_input_mat, camera_input_mat, 1);
		if ((char)waitKey(1) == 32)
		{
			background_mat = camera_input_mat.clone();
			cout << "Space clicked" << endl;
		}

		Mat segmented_camera_input_mat = background_mat.clone() - camera_input_mat.clone();


		/// Create Erosion Trackbar
		createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "erosion",
		               &erosion_elem, max_elem,
		               Erosion);

		createTrackbar("Kernel size:\n 2n +1", "erosion",
		               &erosion_size, max_kernel_size,
		               Erosion);

		/// Create Dilation Trackbar
		createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "dilation",
		               &dilation_elem, max_elem,
		               Dilation);

		createTrackbar("Kernel size:\n 2n +1", "dilation",
		               &dilation_size, max_kernel_size,
		               Dilation);


		for (int x = 0; x < segmented_camera_input_mat.rows; x++)
		{
			for (int y = 0; y < segmented_camera_input_mat.cols; y++)
			{
				if (segmented_camera_input_mat.at<Vec3b>(x, y) == Vec3b(255, 255, 255))
				{
					segmented_camera_input_mat.at<Vec3b>(x, y)[0] = 0;
					segmented_camera_input_mat.at<Vec3b>(x, y)[1] = 0;
					segmented_camera_input_mat.at<Vec3b>(x, y)[2] = 0;
				}
			}
		}

		cvtColor(segmented_camera_input_mat, greyscale_mat, CV_BGR2GRAY);

		medianBlur(greyscale_mat, blured, 15); // Applyng median blur

		/// Create Trackbar to choose type of Threshold
		createTrackbar(trackbar_type,
		               "thresholding", &threshold_type,
		               max_type, Threshold_Demo);

		createTrackbar(trackbar_value,
		               "thresholding", &threshold_value,
		               max_value, Threshold_Demo);

		/// Call the function to initialize
		Threshold_Demo(0, 0);

		//bn to erosion_dst
		Erosion(0, 0);
		imshow("erosion", erosion_dst);
		//erosion_dst to dilation_dst
		Dilation(0, 0);
		imshow("dilation", dilation_dst);

		//fist_dst = dispImage(Rect(dilation_dst.rows - 152, dilation_dst.cols - 152, 150, 150));
		Rect camera_region_of_interest = Rect(489, 329, 150, 150);
		Mat fist_mat(camera_region_of_interest.size(), dilation_dst.type());
		dilation_dst(camera_region_of_interest).copyTo(fist_mat);
		imshow("Fist window", fist_mat);

		fist_dst = fist_mat;

		//comment
		src = bn;

		SimpleBlobDetector::Params square_params = find_params("square");
		SimpleBlobDetector::Params circle_params = find_params("circle");
		SimpleBlobDetector::Params triangle_params = find_params("triangle");
		SimpleBlobDetector::Params fist_params = find_params("fist");

		Ptr<SimpleBlobDetector> squaredetector = SimpleBlobDetector::create(square_params);
		Ptr<SimpleBlobDetector> circledetector = SimpleBlobDetector::create(circle_params);
		Ptr<SimpleBlobDetector> triangledetector = SimpleBlobDetector::create(triangle_params);
		Ptr<SimpleBlobDetector> fistdetector = SimpleBlobDetector::create(fist_params);


		// Detect blobs
		squaredetector->detect(dilation_dst, squarekeypoints);
		triangledetector->detect(dilation_dst, trianglekeypoints);
		circledetector->detect(dilation_dst, circlekeypoints);
		fistdetector->detect(fist_dst, fistkeypoints);

		//the total no of blobs detected are:
		size_t fistamount_size = fistkeypoints.size();

		clock_t fist_end_clock;
		if (fistamount_size != 0)
		{
			fistclicked = true;
			if (t == 0)
			{
				fistClock = clock();
				t = 1;
			}
		}
		else
		{
			t = 0;
			fistclicked = false;
		}
		if (fistclicked == false)
		{
			fistClock = clock();
		}

		fist_end_clock = clock();
		fist_secs = double(fist_end_clock - fistClock) / CLOCKS_PER_SEC;

		if (fist_secs > 4)
		{
			fistclicked = false;
			if (startGame == false)
			{
				startGame = true;
			}
			else if (win == false && lose == false && startGame == true)
			{
				checkPlacement();
			}
			else if (win == true || lose == true)
			{
				win = false;
				lose = false;
				shapeinput->clear();
				shapeCamera->clear();
				shapeinput = new vector<Shape*>(25);
				shapeCamera = new vector<Shape*>(25);
				clickSpawn = true;
			}
			fistClock = clock();
			t = 0;
		}

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob

		Mat im_with_squarekeypoints;
		Mat im_with_trianglekeypoints;
		Mat im_with_circlekeypoints;
		Mat im_with_fistkeypoints;

		drawKeypoints(dilation_dst, squarekeypoints, im_with_squarekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(dilation_dst, trianglekeypoints, im_with_trianglekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(dilation_dst, circlekeypoints, im_with_circlekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(fist_dst, fistkeypoints, im_with_fistkeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		//Make camera grid
		CameraGrid();

		imshow("Squares", im_with_squarekeypoints);
		imshow("Triangle", im_with_trianglekeypoints);
		imshow("Circle", im_with_circlekeypoints);
		imshow("original", camera_input_mat);
		imshow("edited", segmented_camera_input_mat);
		imshow("background", background_mat);
		imshow("Fisting", im_with_fistkeypoints);

		if (take_background_b == false)
		{
			clock_t take_background_timer_end_clock = clock();
			double elapsed_secs = double(take_background_timer_end_clock - take_background_timer_start_clock) / CLOCKS_PER_SEC;
			if (elapsed_secs > 10)
			{
				background_mat = takeBackground(camera_input_mat.clone(), background_mat.clone(), dilation_dst.clone());
				take_background_b = true;
			}
		}
	}
}

void circledetectionwithhough(Mat gray)
{
	vector<Vec3f> circles;
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
		gray.rows / 16,  // change this value to detect circles with different distances to each other
		100, 30, 1, 30 // change the last two parameters
					   // (min_radius & max_radius) to detect larger circles
	);
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		Point center = Point(c[0], c[1]);
		// circle center
		circle(src, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
		// circle outline
		int radius = c[2];
		circle(src, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
	}
	imshow("Hough Grant", gray);
}

void spawnFeedback()
{
	clock_t spawning = clock();
	while (spawnfeedback == true)
	{
		spawningprompt.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, spawningprompt.cols, spawningprompt.rows)));
		clock_t spawnEnd = clock();
		double elapsed_secs = double(spawnEnd - spawning) / CLOCKS_PER_SEC;
		imshow("GRID", grid);
		if (elapsed_secs > 3)
		{
			spawnfeedback = false;
		}
	}
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		//showloadingimage = true;
		if (!clickSpawn && !checktime)
		{
			clickSpawn = true;
			cout << "Spawning" << endl;
			//showshapeimgs = true;
		}
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


void GridCheckingsAndSuch()
{
	namedWindow("Grid1");
	int buffersize = 12;


	createTrackbar("x",
	               "Grid1", &basex,
	               maxbase, Grid_Demo);

	createTrackbar("y",
	               "Grid1", &basey,
	               maxbase, Grid_Demo);
	createTrackbar("Step size",
	               "Grid1", &projectorStepSize,
	               maxStepSize, Grid_Demo);
	grid = dispImage.clone();
	createTrackbar("Grid size",
	               "Grid1", &gridSize,
	               gridSizeMax, Grid_Demo);
	grid = dispImage.clone();


	for (int x = 0; x < grid.rows; x++)
	{
		for (int y = 0; y < grid.cols; y++)
		{
			//mat.at<uchar>(row, column, channel) = val;
			grid.at<uchar>(x, y) = 255;
			//grid.at<Vec3b>(Point(x,y)) = (255, 255, 255);
		}
	}

	Grid_Demo(0, 0);

	Mat triangle = imread("triangle.png");
	Mat square = imread("square.png");
	Mat circle = imread("circle.png");
}


void CameraGrid()
{
	Mat gridimage = dilation_dst.clone()(Rect(dilation_dst.rows - 280, 120, 280, 280));

	int width = gridimage.rows;
	int height = gridimage.cols;
	stepSize = width / gridSize;


	for (int i = 0; i <= 5; i++)
	{
		line(gridimage, Point(0, i * stepSize), Point(5 * stepSize, i * stepSize), Scalar(255, 255, 255));
	}
	for (int i = 0; i <= 5; i++)
	{
		line(gridimage, Point(i * stepSize, 0), Point(i * stepSize, 5 * stepSize), Scalar(255, 255, 255));
	}

	imshow("cameragrid", gridimage);
}


void checkPlacement()
{
	Mat gridimage = dilation_dst(Rect(dilation_dst.rows - 280, 120, 280, 280));

	int width = gridimage.rows;
	int height = gridimage.cols;

	for (int i = 0; i < gridSize; i += 1)
	{
		for (int j = 0; j < gridSize; j += 1)
		{
			int buffersize = 12;
			int VALUEXMAX = (((width / gridSize) * (i + 1)) - buffersize);
			int VALUEXMIN = (((width / gridSize) * i) + buffersize);
			int VALUEYMAX = (((height / gridSize) * (j + 1)) - buffersize);
			int VALUEYMIN = (((height / gridSize) * j) + buffersize);
			for (int k = 0; k < squarekeypoints.size(); k += 1)
			{
				if (squarekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) < VALUEXMAX && squarekeypoints.operator[](k).pt.x
					- (dilation_dst.rows - 280) > VALUEXMIN && squarekeypoints.operator[](k).pt.y - 120 < VALUEYMAX && squarekeypoints.
					operator[](k).pt.y - 120 > VALUEYMIN)
				{
					//if (shapeCamera->operator[](i + j*gridSize) != NULL)
					//	shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j * gridSize) = new Shape(squarekeypoints.operator[](k).pt.x,
					                                                      squarekeypoints.operator[](k).pt.y, 0);

					//comparing[i][j] = 1;

					cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < trianglekeypoints.size(); k += 1)
			{
				if (trianglekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) < VALUEXMAX && trianglekeypoints.operator[](k).
				                                                                                                      pt.x - (
						dilation_dst.rows - 280) > VALUEXMIN && trianglekeypoints.operator[](k).pt.y - 120 < VALUEYMAX &&
					trianglekeypoints
					.operator[](k).pt.y - 120 > VALUEYMIN)
				{
					//if (shapeCamera->operator[](i + j*gridSize) != NULL)
					//	shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j * gridSize) = new Shape(trianglekeypoints.operator[](k).pt.x,
					                                                      trianglekeypoints.operator[](k).pt.y, 1);

					//comparing[i][j] = 2;


					cout << "triangle in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < circlekeypoints.size(); k += 1)
			{
				if (circlekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) < VALUEXMAX && circlekeypoints.operator[](k).pt.x
					- (dilation_dst.rows - 280) > VALUEXMIN && circlekeypoints.operator[](k).pt.y - 120 < VALUEYMAX && circlekeypoints.
					operator[](k).pt.y - 120 > VALUEYMIN)
				{
					//if (shapeCamera->operator[](i + j*gridSize) != NULL) {
					//shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j * gridSize) = new Shape(circlekeypoints.operator[](k).pt.x,
					                                                      circlekeypoints.operator[](k).pt.y, 2);
					//}
					//comparing[i][j] = 3;


					cout << "circle in grid " << i << ", " << j << " my man" << endl;
				}
			}
		}
	}
	for (int i = 0; i < gridSize; i += 1)
	{
		for (int j = 0; j < gridSize; j += 1)
		{
			if (!shapeCamera->operator[](i + j * gridSize) == NULL)
			{
				cout << "Test " << shapeCamera->operator[](i + j * gridSize)->getType() << endl;
			}
		}
	}

	cout << "works" << endl;
	int counter = 0;
	for (int i = 0; i < gridSize; i += 1)
	{
		for (int j = 0; j < gridSize; j += 1)
		{
			if (shapeinput->operator[](i + j * gridSize) == NULL && shapeCamera->operator[](i + j * gridSize) == NULL)
			{
				cout << "both are null" << endl;
			}
			else if (shapeinput->operator[](i + j * gridSize) == NULL && !shapeCamera->operator[](i + j * gridSize) == NULL || !
				shapeinput->operator[](i + j * gridSize) == NULL && shapeCamera->operator[](i + j * gridSize) == NULL)
			{
				cout << "one is null" << endl;
				counter++;
			}
			else
			{
				if (shapeinput->operator[](i + j * gridSize)->getType() == shapeCamera->operator[](i + j * gridSize)->getType())
				{
					cout << i << " , " << j << " correct" << endl;
				}
				else
				{
					cout << i << " , " << j << " git gud scrub" << endl;
					counter++;
				}
			}
		}
	}
	cout << "change difficulty 1: " << difficulty << endl;
	cout << "count 1: " << counter << endl;
	if (counter == 0)
	{
		win = true;
		//difficulty++;
	}
	else
	{
		lose = true;
		if (difficulty != 1)
		{
			//difficulty--;
		}
	}
	cout << "change difficulty 2: " << difficulty << endl;
	cout << "count 2: " << counter << endl;
	for (int i = 0; i < shapeCamera->size(); i++)
	{
		//		cout << "Element: " << i << ", X: " << shapeCamera->operator[](i)->getGridx() << ", Y: " << shapeCamera->operator[](i)->getGridy() << ", Type: " << shapeCamera->operator[](i)->getType() << endl;
	}
}

//vector<Shape*>* 
void calcShapes()
{
	vector<Point*> previouslocations;
	shapeinput->clear();
	shapeinput = new vector<Shape*>(25);
	int shapex;
	int shapey;
	int shapetype;
	bool changed = false;
	bool different = false;
	for (int i = 0; i < difficulty; i++)
	{
		while (different == false)
		{
			srand(time(NULL));

			shapex = rand() % gridSize;
			shapey = rand() % gridSize;
			shapetype = rand() % 3;
			if (previouslocations.size() == 0)
			{
				break;
			}
			//cout << different << endl;
			for (int j = 0; j < previouslocations.size(); j++)
			{
				if (shapex == previouslocations.operator[](j)->x && shapey == previouslocations.operator[](j)->y)
				{
					changed = true;
				}
				else
				{
					different = true;
					//cout << "set to true" << endl;
				}
			}
			if (changed)
			{
				different = false;
				changed = false;
			}
		}
		previouslocations.push_back(new Point(shapex, shapey));
		different = false;
		shapeinput->operator[](shapex + shapey * gridSize) = new Shape(shapex, shapey, shapetype);
	}
	previouslocations.clear();
}


void spawnShape(bool spawn)
{
	//Based on difficulty (spawn number of shapes)
	if (spawn == true)
	{
		//shapeinput->empty();
		//shapeinput = 
		calcShapes();
		beginClock = clock();
		checktime = true;
		showshapeimgs = true;
		spawnfeedback = false;
	}
	clickSpawn = false;
}


int main()
{
	namedWindow(winName, CV_WINDOW_AUTOSIZE);

	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	Mat winscreen = imread("correct.png");
	Mat losescreen = imread("wrongicon.png");
	Mat intro = imread("introscreen.jpg");

	thread imageprocessing(segmentation);
	//namedWindow("GRID", 1);
	cvNamedWindow("GRID", CV_WINDOW_NORMAL);
	moveWindow("GRID", 1981, 0);
	cvSetWindowProperty("GRID", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	setMouseCallback("GRID", CallBackFunc, NULL);
	dispImage = homescreen->run();
	thread* spawnShapesThread = new thread();
	startGame = false;
	while (true)
	{

		if (startGame)
		{
			//Creates a thread that runs the image processing part of the prototype
			//thread spawnimagethread;

			//bool checktime = false;

			waitKey(1);
			gridimage = dispImage;
			//testing


			//GridCheckingsAndSuch();
			//cout << basex << " " << basey << endl;

			//Create grid-system on projector
			GridCheckingsAndSuch();
			//Spawn shapes in grid-system

			//Show shapes in grid
			if (clickSpawn == true)
			{
				spawnfeedback = true;
				//spawnimagethread = thread{ spawnFeedback };
				spawnShapesThread = new thread{ spawnShape, clickSpawn };

				//if (spawnShapesThread.joinable()) spawnShapesThread.join();

				//spawnimagethread.join();

				//if (spawn == true) spawn = false;
			}


			if (spawnfeedback)
			{
				spawningprompt.copyTo(grid(Rect(grid.cols / 4, grid.rows / 2, spawningprompt.cols, spawningprompt.rows)));
			}
			else
			{
				if (!spawnShapesThread == NULL)
				{
					if (spawnShapesThread->joinable())
					{
						spawnShapesThread->join();
					}
				}
			}
			if (showshapeimgs)
			{
				for (int i = 0; i < shapeinput->size(); i++)
				{
					if (!shapeinput->operator[](i) == NULL)
					{
						shapeinput->operator[](i)->getImage().copyTo(grid(Rect(
							(shapeinput->operator[](i)->getGridx() * (projectorStepSize * 2)) + basex * 2 + 18,
							(shapeinput->operator[](i)->getGridy() * (projectorStepSize * 2)) + basey * 2 + 18,
							shapeinput->operator[](i)->getImage().cols, shapeinput->operator[](i)->getImage().rows)));
					}
				}
			}


			//Mat gridOutput;
			//gridOutput = grid;

			//sleep_for(seconds(25));
			//shapeinput->erase(shapeinput->begin(), shapeinput->begin() + shapeinput->size());
			//GridCheckingsAndSuch();


			if (checktime)
			{
				clock_t end = clock();
				double elapsed_secs = double(end - beginClock) / CLOCKS_PER_SEC;
				//cout << elapsed_secs << endl;

				if (elapsed_secs > 5)
				{
					beginClock = clock();
					showshapeimgs = false;
					//shapeinput->erase(shapeinput->begin(), shapeinput->begin() + shapeinput->size());
					checktime = false;
				}
			}

			if (win == true)
			{
				winscreen.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, winscreen.cols, winscreen.rows)));
			}

			if (lose == true)
			{
				losescreen.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, losescreen.cols, losescreen.rows)));
			}

			if (fist_secs > 0 && fist_secs < 1.0)
			{
				timer0.copyTo(grid(Rect(709, 189, timer0.cols, timer0.rows)));
			}
			else if (1.0 < fist_secs && fist_secs < 2.0)
			{
				timer1.copyTo(grid(Rect(709, 189, timer1.cols, timer1.rows)));
			}
			else if (2.0 < fist_secs && fist_secs < 3.0)
			{
				timer2.copyTo(grid(Rect(709, 189, timer2.cols, timer2.rows)));
			}
			else if (3.0 < fist_secs && fist_secs < 4.0)
			{
				timer3.copyTo(grid(Rect(709, 189, timer3.cols, timer3.rows)));
			}
			else if (4.0 < fist_secs)
			{
				timer4.copyTo(grid(Rect(709, 189, timer4.cols, timer4.rows)));
			}
			//if (showloadingimage == true)
			//{
			//	spawningprompt.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, spawningprompt.cols, spawningprompt.rows)));
			//hey
			//}

			imshow("Grid1", dispImage);
			//if (spawnimagethread.joinable()) spawnimagethread.join();
			imshow("GRID", grid);
		}
		else
		{
			waitKey(1);
			grid = intro.clone();

			if (fist_secs > 0 && fist_secs < 1.0)
			{
				cout << "1: " << fist_secs << endl;

				timer0.copyTo(grid(Rect(709, 189, timer0.cols, timer0.rows)));
			}
			else if (1.0 < fist_secs && fist_secs < 2.0)
			{
				cout << "2: " << fist_secs << endl;

				timer1.copyTo(grid(Rect(709, 189, timer1.cols, timer1.rows)));
			}
			else if (2.0 < fist_secs && fist_secs < 3.0)
			{
				cout << "3: " << fist_secs << endl;

				timer2.copyTo(grid(Rect(709, 189, timer2.cols, timer2.rows)));
			}
			else if (3.0 < fist_secs && fist_secs < 4.0)
			{
				cout << "4: " << fist_secs << endl;

				timer3.copyTo(grid(Rect(709, 189, timer3.cols, timer3.rows)));
			}
			else if (4.0 < fist_secs)
			{
				cout << "5: " << fist_secs << endl;

				timer4.copyTo(grid(Rect(709, 189, timer4.cols, timer4.rows)));
			}

			imshow("GRID", grid.clone());
		}
	}
}

void Threshold_Demo(int, void*)
{
	/* 0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted
	*/

	threshold(blured, bn, threshold_value, max_BINARY_value, threshold_type);

	imshow("thresholding", bn);
}

void Grid_Demo(int, void*)
{
	for (int i = 0; i <= gridSize; i++)
	{
		line(grid, Point(2 * basex, 2 * basey + 2 * (i * projectorStepSize)),
		     Point(2 * basex + 2 * (gridSize * projectorStepSize), 2 * basey + 2 * (i * projectorStepSize)), Scalar(0, 0, 0));
	}
	for (int i = 0; i <= gridSize; i++)
	{
		line(grid, Point(2 * basex + 2 * (i * projectorStepSize), 2 * basey),
		     Point(2 * basex + 2 * (i * projectorStepSize), 2 * basey + 2 * (gridSize * projectorStepSize)), Scalar(0, 0, 0));
	}
}

/**  @function Erosion  */
void Erosion(int, void*)
{
	int erosion_type;
	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type,
	                                    Size(2 * erosion_size + 1, 2 * erosion_size + 1),
	                                    Point(erosion_size, erosion_size));

	/// Apply the erosion operation
	erode(bn, erosion_dst, element);
}

/** @function Dilation */
void Dilation(int, void*)
{
	int dilation_type;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(dilation_type,
	                                    Size(2 * dilation_size + 1, 2 * dilation_size + 1),
	                                    Point(dilation_size, dilation_size));
	/// Apply the dilation operation
	dilate(erosion_dst, dilation_dst, element);
}

