#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

#include <sstream>
#include <iostream>
#include <fstream>

#include "Frame.h"

using namespace std;
using namespace cv;

// Global Variables:
const float singleMarkerSideLength = 0.15f; //meters
Mat cameraIntrinsics = Mat::eye(3, 3, CV_64F);
Mat distortionCoeffs;

// Camera Calibration
bool loadCameraCalibration(string name, Mat& cameraIntrinsics, Mat& distortionCoeffs)
{
	// initialize data stream from calibraiton file
	ifstream inStream(name);

	// if the stream opens successfully (i.e. the file is found), then unpack info for distortions and camera matrix
	if (inStream)
	{
		uint16_t rows;
		uint16_t columns;

		// Camera Matrix
		inStream >> rows;
		inStream >> columns;

		cameraIntrinsics = Mat(Size(columns, rows), CV_64F);
		
		cout << "Camera Intrinsics:" << endl;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double read = 0.0f;
				inStream >> read;
				cameraIntrinsics.at<double>(r, c) = read;
				cout << cameraIntrinsics.at<double>(r, c) << "\n";
			}
		}

		//Distortion Coeffs
		inStream >> rows;
		inStream >> columns;

		distortionCoeffs = Mat::zeros(rows, columns, CV_64F);
		
		cout << "Distortion Coefficients:" << endl;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double read = 0.0f;
				inStream >> read;
				distortionCoeffs.at<double>(r, c) = read;
				cout << distortionCoeffs.at<double>(r, c) << "\n";
			}
		}

		// close input stream
		inStream.close();

		// return successfully completed task
		return true;
	}

	return false;
}

void createArucoMarkers(Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_1000))
{
	Mat outputMarker;

	for (int i = 0; i < 1024; i++)
	{
		aruco::drawMarker(markerDictionary, i, 1000, outputMarker, 1);
		ostringstream convert;
		string imageName = "ARUCO_ORIGINAL_";
		convert << imageName << i << ".png";
		imwrite(convert.str(), outputMarker);
	}
}

// POPULATE DICTIONARY
/*
int main()
{
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_ARUCO_ORIGINAL);
	createArucoMarkers(markerDictionary);
}
*/

// RUN VISION SYSTEM

int main()
{
	// Open VideoCapture device:
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cout << "Error: Camera already opened." << endl;
		return -1;
	}

	// Open Windows:
	namedWindow("imCap", WINDOW_AUTOSIZE);
	namedWindow("imGray", WINDOW_AUTOSIZE);
	//namedWindow("imBW", WINDOW_AUTOSIZE);
	namedWindow("imScan", WINDOW_AUTOSIZE);
	namedWindow("imPose", WINDOW_AUTOSIZE);
	
	// Load Camera Parameters:
	loadCameraCalibration("CharCaliOut", cameraIntrinsics, distortionCoeffs);

	// Initialize Current Capture:
	Mat currentCap;

	while (true)
	{
		// if no frame returned from webcam (unable to get info from webcam)
		if (!cap.read(currentCap))
			break;
		
		// Create Frame object:
		Frame currentFrame(currentCap);

		// Display to Windows:
		imshow("imCap", currentFrame.getImCap());
		imshow("imGray", currentFrame.getImGray());
		//imshow("imBW", currentFrame.getImBW());
		imshow("imScan", currentFrame.getImScan());
		imshow("imPose", currentFrame.getImPose());
		
		// WaitKey:
		if (waitKey(1) >= 0) break;
	}
	
	return 1;
}

