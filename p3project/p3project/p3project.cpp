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


using namespace std;
using namespace cv;
using namespace this_thread; // sleep_for, sleep_until
using namespace chrono; // nanoseconds, system_clock, seconds


string current_screen = "home";
string winName = "P3 Project";
/*int main(int argc, const char** argv)
{

	// Init background substractor
	Ptr<BackgroundSubtractor> bg_model = createBackgroundSubtractorMOG2().dynamicCast<BackgroundSubtractor>();

	// Create empy input img, foreground and background image and foreground mask.
	Mat img, foregroundMask, backgroundImage, foregroundImg;

	// capture video from source 0, which is web camera, If you want capture video from file just replace //by  VideoCapture cap("videoFile.mov")
	VideoCapture cap(0);

	// main loop to grab sequence of input files
	for (;;) {

		bool ok = cap.grab();

		if (ok == false) {

			std::cout << "Video Capture Fail" << std::endl;


		}
		else {

			// obtain input image from source
			cap.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
			// Just resize input image if you want
			resize(img, img, Size(640, 480));

			// create foreground mask of proper size
			if (foregroundMask.empty()) {
				foregroundMask.create(img.size(), img.type());
			}

			// compute foreground mask 8 bit image
			// -1 is parameter that chose automatically your learning rate

			bg_model->apply(img, foregroundMask, true ? -1 : 0);

			// smooth the mask to reduce noise in image
			medianBlur(foregroundMask, foregroundMask, 5);
			//make it binary
			Mat binary;

		//cvtColor(foregroundMask, foregroundMask, CV_BGR2GRAY);
				//GaussianBlur(foregroundMask, foregroundMask, Size(11, 11), 3.5, 3.5);


			//threshold mask to saturate at black and white values
			threshold(foregroundMask, foregroundMask, 10, 255, THRESH_BINARY);
			foregroundImg = Scalar::all(0);
			// Copy source image to foreground image only in area with white mask
			img.copyTo(foregroundImg, foregroundMask);

			//Get background image
bg_model->getBackgroundImage(backgroundImage);

			// Show the results
			imshow("foreground mask", foregroundMask);
			imshow("foreground image", foregroundImg);

			int key6 = waitKey(40);

			if (!backgroundImage.empty()) {

				imshow("mean background image", backgroundImage);
				int key5 = waitKey(40);

			}


		}

	}

	return EXIT_SUCCESS;
}*/




int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;
Mat src, erosion_dst, dilation_dst;
Mat bn;
Mat blured;
Mat tresh, tresh_bin;
int threshold_value = 0;
int threshold_type = 3;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

int width = 800;
int height = 600;
int stepSize = 78;
int projectorStepSize = 41;

int maxStepSize = 150;
int basex = 120;
int const maxbase = 100 * 5;
int maxbasex = basex + 5 * projectorStepSize;
int basey = 17;
int maxbasey = basey + 5 * projectorStepSize;
int gridSize = 5;
int const gridSizeMax = 15;
Mat dispImage;
Mat gridimage;
Mat grid;

clock_t beginClock;
bool clickSpawn = false;
bool checktime = false;

//Storage for shapes in projector
vector<Shape*>* shapeinput = new vector<Shape*>();
vector<Shape*>* shapeCamera = new vector<Shape*>();

// Storage for blobs
vector<KeyPoint> squarekeypoints;
vector<KeyPoint> trianglekeypoints;
vector<KeyPoint> circlekeypoints;


/** Function Headers */
void Threshold_Demo(int, void*);
void Grid_Demo(int, void*);
void Erosion(int, void*);
void Dilation(int, void*);
void GridCheckingsAndSuch();
void CameraGrid();
char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";


void segmentation()
{
	VideoCapture stream1(1); //change 0 to 1 to get from the plugged in camera instead of webcam.
	if (!stream1.isOpened())
	{
		cout << "Cannot open camera";
	}
	Mat cameraInput;
	Mat testing1;
	Mat segmentedCameraInput;
	Mat image = imread("scary2.png");
	//Mat test = imread("testing.jpg");
	Mat background = imread("background.jpg");
	Mat gridOutput;
	Mat bw;

	while (true)
	{
		stream1.read(cameraInput);
		if ((char)waitKey(1) == 32)
		{
			background = cameraInput.clone();
			cout << "Space clicked" << endl;
		}

		segmentedCameraInput = background - cameraInput;


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


		for (int x = 0; x < segmentedCameraInput.rows; x++)
		{
			for (int y = 0; y < segmentedCameraInput.cols; y++)
			{
				if (segmentedCameraInput.at<Vec3b>(x, y) == Vec3b(255, 255, 255))
				{
					segmentedCameraInput.at<Vec3b>(x, y)[0] = 0;
					segmentedCameraInput.at<Vec3b>(x, y)[1] = 0;
					segmentedCameraInput.at<Vec3b>(x, y)[2] = 0;
				}
			}
		}
		// Create a kernel that we will use for accuting/sharpening our image


		//GaussianBlur(src, blured, Size(3,3), 0, 0);




		cvtColor(segmentedCameraInput, bw, CV_BGR2GRAY);
		//GaussianBlur(bw, blured, Size(9,9),5);
		medianBlur(bw, blured, 15); // Applyng median blur

		/// Create Trackbar to choose type of Threshold
		createTrackbar(trackbar_type,
		               "thresholding", &threshold_type,
		               max_type, Threshold_Demo);

		createTrackbar(trackbar_value,
		               "thresholding", &threshold_value,
		               max_value, Threshold_Demo);

		/// Call the function to initialize
		Threshold_Demo(0, 0);
		//threshold(blured, tresh, 109, 255,  CV_THRESH_TRUNC);
		//threshold(tresh, tresh_bin, 108, 255, THRESH_BINARY);

		//bitwise_not(tresh_bin, bn); 
		//bn to erosion_dst
		Erosion(0, 0);
		imshow("erosion", erosion_dst);
		//erosion_dst to dilation_dst
		Dilation(0, 0);
		imshow("dilation", dilation_dst);


		src = bn;
		// Setup SimpleBlobDetector parameters.
		SimpleBlobDetector::Params params;
		// Filter by Area.
		params.filterByArea = false;
		params.minArea = 50;
		params.maxArea = 700;

		// filter my min distance
		//params.minDistBetweenBlobs=100;


		// Filter by Convexity
		params.filterByConvexity = true;
		params.minConvexity = 0.9;

		// Filter by Inertia
		params.filterByInertia = false;
		params.minInertiaRatio = 0.01;

		//filter by colour
		params.filterByColor = true;
		params.blobColor = 255;


		// Filter by circularity

		//Square
		params.filterByCircularity = true;
		params.minCircularity = 0.79;
		params.maxCircularity = 0.85;

		// Set up detector with params
		Ptr<SimpleBlobDetector> squaredetector = SimpleBlobDetector::create(params);

		//Circle
		params.filterByCircularity = true;
		params.minCircularity = 0.86;
		params.maxCircularity = 1;

		Ptr<SimpleBlobDetector> circledetector = SimpleBlobDetector::create(params);

		//Triangle
		params.filterByCircularity = true;
		params.minCircularity = 0.05;
		params.maxCircularity = 0.78;

		Ptr<SimpleBlobDetector> triangledetector = SimpleBlobDetector::create(params);


		// Detect blobs
		squaredetector->detect(dilation_dst, squarekeypoints);
		triangledetector->detect(dilation_dst, trianglekeypoints);
		circledetector->detect(dilation_dst, circlekeypoints);

		//the total no of blobs detected are:
		size_t squareamount = squarekeypoints.size();
		size_t triangleamount = trianglekeypoints.size();
		size_t circleamount = circlekeypoints.size();
		//cout << "total no of squares detected are:" << squareamount << endl << "total no of triangles detected are: " << triangleamount << endl;


		//location of first blob
		if (squareamount != 0)
		{
			Point2f point1 = squarekeypoints.at(0).pt;
			float x1 = point1.x;
			float y1 = point1.y;
			//cout << "location of the square blob is " << x1 << "," << y1;
		}

		if (triangleamount != 0)
		{
			Point2f point1 = trianglekeypoints.at(0).pt;
			float x1 = point1.x;
			float y1 = point1.y;
			//cout << "location of the triangle blob is " << x1 << "," << y1 << endl;
		}

		if (circleamount != 0)
		{
			Point2f point1 = circlekeypoints.at(0).pt;
			float x1 = point1.x;
			float y1 = point1.y;
		}


		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob

		Mat im_with_squarekeypoints;
		Mat im_with_trianlgekeypoints;
		Mat im_with_circlekeypoints;

		drawKeypoints(dilation_dst, squarekeypoints, im_with_squarekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(dilation_dst, trianglekeypoints, im_with_trianlgekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(dilation_dst, circlekeypoints, im_with_circlekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		//Show blobs
		Mat c = im_with_squarekeypoints + im_with_trianlgekeypoints;

		//Make camera grid
		CameraGrid();

		imshow("Squares", im_with_squarekeypoints);
		imshow("Triangle", im_with_trianlgekeypoints);
		imshow("Circle", im_with_circlekeypoints);
		imshow("original", cameraInput);
		imshow("edited", segmentedCameraInput);
		//imshow("cam", testing3);
		imshow("background", background);
	}
	//return src;
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

	if (event == EVENT_LBUTTONDOWN)
	{

		if (!clickSpawn && !checktime) { clickSpawn = true; cout << "Spawning" << endl; }
		////Anywhere



		//if (x > 106 && x < 190)
		//{
		//	if (y > 71 && y < 167)
		//	{
		//		
		//		current_screen= "home";
		//		cout << "home" << endl;
		//	}
		//	else if (y > 167 && y < 272)
		//	{
		//		current_screen = "profile";
		//		cout << "profile" << endl;

		//	}
		//	else if (y > 167 && y < 372)
		//	{
		//		current_screen = "settings";
		//		cout << "settings" << endl;

		//	}
		//	else if (y > 372 && y < 472)
		//	{
		//		current_screen = "quit";
		//		cout << "quit" << endl;

		//	}
		//}
		////Within profile
		//if (current_screen == "profile")
		//{
		//	
		//}
		////cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
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
	/*
	int width = 800;
	int height = 600;
	int stepSize = 70;
	int buffersize = 12;
	int gridS = 1000;
	int base = 100;
	int maxbase = width;
	int basey = 100;
	int maxbasey = height;
	int end = base + 5*stepSize;
	*/
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



	

	//imshow("GRID", gridimage);

	
	/*
	for (int i = 0; i < 5; i += 1) {
		for (int j = 0; j < 5; j += 1) {
			int VALUEXMAX = (((width / 5) * (i + 1)) - buffersize);
			int VALUEXMIN = (((width / 5) * i) + buffersize);
			int VALUEYMAX = (((height / 5) * (j + 1)) - buffersize);
			int VALUEYMIN = (((height / 5) * j) + buffersize);
			for (int k = 0; k < squarekeypoints.size(); k += 1) {
				if (squarekeypoints.operator[](k).pt.x - 100 < VALUEXMAX && squarekeypoints.operator[](k).pt.x - 100 > VALUEXMIN && squarekeypoints.operator[](k).pt.y - 144 < VALUEYMAX && squarekeypoints.operator[](k).pt.y - 144 > VALUEYMIN)
				{
					cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
		}
	}
	*/

	Mat triangle = imread("triangle.png");
	Mat square = imread("square.png");
	Mat circle = imread("circle.png");

	/*
	int difficulty = 4;
	//Based on difficulty (spawn number of shapes)
	vector<Shape*>* shapeinput = spawnShapes(difficulty);
	cout << "Amount of shapes shown " << shapeinput->size() << endl;
	//calculate random int(s) for the x and y axis
	
	for (int i = 0; i < shapeinput->size(); i++)
	{
		cout << "X: " << shapeinput->operator[](i)->getGridx() << ", Y: " << shapeinput->operator[](i)->getGridy() << ", Type: " << shapeinput->operator[](i)->getType() << endl;
	}


	for (int i = 0; i < shapeinput->size(); i++)
	{
		shapeinput->operator[](i)->getImage().copyTo(a(Rect((shapeinput->operator[](i)->getGridx()*50)+257, (shapeinput->operator[](i)->getGridy()*50) + 210, shapeinput->operator[](i)->getImage().cols, shapeinput->operator[](i)->getImage().rows)));
	}



	//imshow("grid", gridimage);
	*/
}


void CameraGrid()
{
	int cameraX = 150;
	int cameraY = 120;
	Mat gridimage = dilation_dst(Rect(150, 120, 280, 280));

	int width = gridimage.rows;
	int height = gridimage.cols;
	stepSize = width / gridSize;
	int buffersize = 12;

	for (int i = 0; i <= 5; i++)
	{
		line(gridimage, Point(0, i*stepSize), Point(5 * stepSize, i*stepSize), Scalar(255, 255, 255));
	}
	for (int i = 0; i <= 5; i++)
	{
		line(gridimage, Point(i*stepSize, 0), Point(i*stepSize, 5 * stepSize), Scalar(255, 255, 255));
	}

	//for (int i = 0; i < height; i += stepSize)
	//	line(gridimage, Point(0, i), Point(width, i), Scalar(255, 255, 255));
	//for (int i = 0; i < width; i += stepSize)
	//	line(gridimage, Point(i, 0), Point(i, height), Scalar(255, 255, 255));





	for (int i = 0; i < gridSize; i += 1) {
		for (int j = 0; j < gridSize; j += 1) {
			int VALUEXMAX = (((width / gridSize) * (i + 1)) - buffersize);
			int VALUEXMIN = (((width / gridSize) * i) + buffersize);
			int VALUEYMAX = (((height / gridSize) * (j + 1)) - buffersize);
			int VALUEYMIN = (((height / gridSize) * j) + buffersize);
			for (int k = 0; k < squarekeypoints.size(); k += 1) {
				if (squarekeypoints.operator[](k).pt.x - cameraX < VALUEXMAX && squarekeypoints.operator[](k).pt.x - cameraX > VALUEXMIN && squarekeypoints.operator[](k).pt.y - cameraY < VALUEYMAX && squarekeypoints.operator[](k).pt.y - cameraY > VALUEYMIN)
				{
					if (shapeCamera->operator[](i + j*gridSize) != NULL)
						shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j*gridSize) = new Shape(squarekeypoints.operator[](k).pt.x, squarekeypoints.operator[](k).pt.y, 0);
					// squarekeypoints.operator[](k).pt.x = shapeCamera->operator[](k).pt.x;

					// cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < trianglekeypoints.size(); k += 1) {
				if (trianglekeypoints.operator[](k).pt.x - cameraX < VALUEXMAX && trianglekeypoints.operator[](k).pt.x - cameraX > VALUEXMIN && trianglekeypoints.operator[](k).pt.y - cameraY < VALUEYMAX && trianglekeypoints.operator[](k).pt.y - cameraY > VALUEYMIN)
				{

					if (shapeCamera->operator[](i + j*gridSize) != NULL)
						shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j*gridSize) = new Shape(trianglekeypoints.operator[](k).pt.x, trianglekeypoints.operator[](k).pt.y, 1);

					//cout << "triangle in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < circlekeypoints.size(); k += 1) {
				if (circlekeypoints.operator[](k).pt.x - cameraX < VALUEXMAX && circlekeypoints.operator[](k).pt.x - cameraX > VALUEXMIN && circlekeypoints.operator[](k).pt.y - cameraY < VALUEYMAX && circlekeypoints.operator[](k).pt.y - cameraY > VALUEYMIN)
				{
					if (shapeCamera->operator[](i + j*gridSize) != NULL)

						shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j*gridSize) = new Shape(circlekeypoints.operator[](k).pt.x, circlekeypoints.operator[](k).pt.y, 2);
					
					//cout << "circle in grid " << i << ", " << j << " my man" << endl;

				}
			}
		}
	}
	imshow("cameragrid", gridimage);

}

vector<Shape*>* calcShapes(int difficulty)
{
	vector<Point*> previouslocations;
	vector<Shape*>* output = new vector<Shape*>();
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

			shapex = rand() % 3;
			shapey = rand() % 3;
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
		output->push_back(new Shape(shapex, shapey, shapetype));

	}

	return output;
}


void spawnShape(int difficulty, bool spawn)
{
	//Based on difficulty (spawn number of shapes)
	if (spawn == true)
	{
		//shapeinput->empty();
		shapeinput = calcShapes(difficulty);
	}
	cout << "Amount of shapes shown " << shapeinput->size() << endl;
	//calculate random int(s) for the x and y axis

	/*for (int i = 0; i < shapeinput->size(); i++)
	{
		cout << "X: " << shapeinput->operator[](i)->getGridx() << ", Y: " << shapeinput->operator[](i)->getGridy() << ", Type: " << shapeinput->operator[](i)->getType() << endl;
	}*/
	clickSpawn = false;
	beginClock = clock();
	checktime = true;
}





int main()
{
	namedWindow(winName, CV_WINDOW_AUTOSIZE);

	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	
	dispImage = homescreen->run();

	//namedWindow("GRID", 1);
	namedWindow("GRID", CV_WINDOW_AUTOSIZE);
	setMouseCallback("GRID", CallBackFunc, NULL);


	bool spawn = true;
	int difficulty = 4;
	//Creates a thread that runs the image processing part of the prototype
	thread imageprocessing(segmentation);

	//bool checktime = false;
	while (true) {
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
			thread spawnShapesThread(spawnShape, difficulty, clickSpawn);
			spawnShapesThread.join();
			//if (spawn == true) spawn = false;
		}
		for (int i = 0; i < shapeinput->size(); i++)
		{
			shapeinput->operator[](i)->getImage().copyTo(grid(Rect((shapeinput->operator[](i)->getGridx()*(projectorStepSize*2)) + basex*2 + 18, (shapeinput->operator[](i)->getGridy()*(projectorStepSize*2)) + basey*2 + 18, shapeinput->operator[](i)->getImage().cols, shapeinput->operator[](i)->getImage().rows)));
		}

		



		//Mat gridOutput;
		//gridOutput = grid;

		imshow("Grid1", dispImage);
		imshow("GRID", grid);
		//sleep_for(seconds(25));
		//shapeinput->erase(shapeinput->begin(), shapeinput->begin() + shapeinput->size());
		//GridCheckingsAndSuch();




		if (checktime)
		{
			clock_t end = clock();
			double elapsed_secs = double(end - beginClock) / CLOCKS_PER_SEC;
			cout << elapsed_secs << endl;
			
			if (elapsed_secs > 5)
			{
				beginClock = clock();
				shapeinput->erase(shapeinput->begin(), shapeinput->begin() + shapeinput->size());
				checktime = false;
			}

		}



	}



	//waitKey(0);
	//testing
}



/*
while (true) {
if (!dispImage.empty()) {
dispImage.release();
setMouseCallback(winName, CallBackFunc, NULL);

if (current_screen == "home")
{
dispImage = homescreen->run();
}
else if (current_screen == "profile")
{
dispImage = test->run();
cout << "Profile: " << dispImage.rows << endl;
}
else if (current_screen == "settings")
{

dispImage = settingsscreen->run();
cout << "Settings: " << dispImage.rows << endl;
}
else if (current_screen == "quit")
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
}
imshow("window", dispImage);
waitKey();
///  VVV is the camera loop VVV
Mat testing1;
Mat testing2;
Mat image = imread("scary2.png");
Mat test = imread("testing.jpg");
Mat background = imread("background.jpg");

/*
VideoCapture stream1(1); //change 0 to 1 to get from the plugged in camera instead of webcam.
if (!stream1.isOpened())
{
cout << "Cannot open camera";
}

*/

/*
while (true)
{
stream1.read(testing1);
if ((char)waitKey(1) == 32)
{
background = testing1.clone();
cout << "Space clicked" << endl;
}

testing2 = background - testing1;


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


Mat testing3;
//testing3 = segmentation(testing2);
//GridCheckingsAndSuch();
//imshow("original", testing1);
//imshow("edited", testing2);
//imshow("cam", testing3);
//imshow("background", background);

}

}
*/
	//	}


		//getchar();
		//return 0;
		//brain();


//	}
	//*/

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
		line(grid, Point(2*basex, 2*basey + 2*(i*projectorStepSize)), Point(2*basex + 2*(gridSize * projectorStepSize), 2*basey + 2*(i*projectorStepSize)), Scalar(0, 0, 0));
	}
	for (int i = 0; i <= gridSize; i++)
	{
		line(grid, Point(2*basex + 2*(i*projectorStepSize), 2*basey), Point(2*basex + 2*(i*projectorStepSize), 2*basey + 2*(gridSize * projectorStepSize)), Scalar(0, 0, 0));
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

