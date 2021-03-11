#pragma once
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/aruco/charuco.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;
using namespace cv;

// Global Variables:
extern const float singleMarkerSideLength; //meters
extern const float diamondMarkerSideLength; //meters
extern const float diamondMarkerSquareLength;
extern const float diamondMarkerArucoSquareRatio;
extern const float diamondMarkerArucoLength;
extern Mat cameraIntrinsics;
extern Mat distortionCoeffs;

class Frame
{
public:
	Frame(Mat im, Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50));

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
	Mat getImDiamond()
	{
		return imDiamond;
	}
	Mat getImPose()
	{
		return imPose;
	}
	double getPitch(int i)
	{
		return -PYR[i][0];
	}
	double getYaw(int i)
	{
		return -PYR[i][1];
	}
	double getRoll(int i)
	{
		return -PYR[i][2];
	}

private:
	// Member Functions:
	void scanMarkers(Mat im);

	void displayPose();

	void computeEuclidianDistances();
	
	// Images:
	Mat imCap;
	Mat imGray;
	Mat imBW;
	Mat imScan;
	Mat imDiamond;
	Mat imPose;
	
	// Marker Information:
	Ptr<aruco::Dictionary> markerDictionary;
	vector<int> singleMarkerIDs;
	vector<Vec4i> diamondMarkerIDs;
	vector<vector<Point2f>> singleMarkerCorners, singleMarkerRejects, diamondMarkerCorners, diamondMarkerRejects;
	vector<Vec3d> rVecs, tVecs, rVecsDiamond, tVecsDiamond;
	vector<Mat> R, RDiamond;
	vector<Vec3d> PYR, PYRDiamond;
	vector<double> euclidianDistances, euclideanDistancesDiamond;
};