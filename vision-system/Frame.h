#pragma once
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

// Global Variables:
extern const float singleMarkerSideLength;
extern Mat cameraIntrinsics;
extern Mat distortionCoeffs;

class Frame
{
public:
	Frame(Mat im);

	// Getters:
	Mat getImCap()
	{
		return imCap;
	}
	Mat getImGray()
	{
		return imGray;
	}
	Mat getImBW()
	{
		return imBW;
	}
	Mat getImScan()
	{
		return imScan;
	}
	Mat getImPose()
	{
		return imPose;
	}

private:
	// Member Functions:
	void scanSingleMarkers(Mat im);

	void displayPose();

	void computeEuclidianDistances();
	
	// Images:
	Mat imCap;
	Mat imGray;
	Mat imBW;
	Mat imScan;
	Mat imPose;
	
	// Marker Information:
	vector<int> singleMarkerIDs;
	vector<vector<Point2f>> singleMarkerCorners, singleMarkerRejects;
	vector<Vec3d> rVecs, tVecs;
	vector<Mat> R;
	vector<Vec3d> PYR;
	vector<double> euclidianDistances;
};