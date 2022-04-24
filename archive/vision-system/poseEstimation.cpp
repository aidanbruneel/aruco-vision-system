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


// setting personal constants for my chessboard and markers
const float arucoSquareDimension = 0.1895f; //meters

// get absolute distance from camera to marker in question
float getEuclidDistance(vector<Vec3d> tvec, int poi)
{
	float output;

	// method for getting Euclidian distance
	output = pow(tvec[poi][0], 2) + pow(tvec[poi][1], 2) + pow(tvec[poi][2], 2);
	output = sqrt(output);

	return output;
}

// monitor live webcam for markers
int startWebcamMonitoring(const Mat& cameraMatrix, const Mat& distanceCoeffs, float arucoSquareDimensions)
{
	// initialize where frames from camera go
	Mat frame;

	// initialize vars for Ids, marker corners, etc.
	vector<int> markerIds;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;
	aruco::DetectorParameters params;

	// getting dictionary
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	// opening video feed
	VideoCapture vid(0);

	// if cannot open, return failure
	if (!vid.isOpened())
	{
		return -1;
	}

	// open live feed in window "Webcam"
	namedWindow("Webcam", WINDOW_AUTOSIZE);

	// readout success, camera matrix and distance coeffs
	cout << "Opened Cam Okay" << endl;
	cout << "camMatrix: " << cameraMatrix << endl;
	cout << "distanceCoeffs: " << distanceCoeffs << endl;

	// create vectors for rotation (roll, pitch, yaw) (rads) and translation (m)
	vector<Vec3d> rVecs, tVecs;

	// loop where look for markers in frames from webcam
	while (true)
	{
		// if no frame returned from webcam (unable to get info from webcam)
		if (!vid.read(frame))
			break;

		// detect marker(s) + estimate their pose(s)
		aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds);
		aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDimension, cameraMatrix, distanceCoeffs, rVecs, tVecs);

		// for each marker, draw their axis on the output frame. if Id = 6, read out rotations, translations, and euclidean distance
		system("CLS");
		for (int i = 0; i < markerIds.size(); i++)
		{
			aruco::drawAxis(frame, cameraMatrix, distanceCoeffs, rVecs[i], tVecs[i], 0.1f);
			if (markerIds[i] == 0)
			{
				cout << " ID: 0" << endl;
				cout << " Rot Vec:   " << rVecs[i] << endl;
				cout << " Trans Vec: " << tVecs[i] << endl;
				cout << " Euclidian:  " << getEuclidDistance(tVecs, i) << endl;
			}
			else if (markerIds[i] == 1)
			{
				cout << " ID: 1" << endl;
				cout << " Rot Vec:   " << rVecs[i] << endl;
				cout << " Trans Vec: " << tVecs[i] << endl;
				cout << " Euclidian:  " << getEuclidDistance(tVecs, i) << endl;
			}
			else if (markerIds[i] == 2)
			{
				cout << " ID: 2" << endl;
				cout << " Rot Vec:   " << rVecs[i] << endl;
				cout << " Trans Vec: " << tVecs[i] << endl;
				cout << " Euclidian:  " << getEuclidDistance(tVecs, i) << endl;
			}
			else if (markerIds[i] == 3)
			{
				cout << " ID: 3" << endl;
				cout << " Rot Vec:   " << rVecs[i] << endl;
				cout << " Trans Vec: " << tVecs[i] << endl;
				cout << " Euclidian:  " << getEuclidDistance(tVecs, i) << endl;
			}
		}
		aruco::drawDetectedMarkers(frame, markerCorners, markerIds);

		// print output frame to window "Webcam"
		imshow("Webcam", frame);
		if (waitKey(30) >= 0) break;
	}

	return 1;
}

// load camera calibration parameters from file created from calibration script
bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoeffs)
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

		cameraMatrix = Mat(Size(columns, rows), CV_64F);

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double read = 0.0f;
				inStream >> read;
				cameraMatrix.at<double>(r, c) = read;
				cout << cameraMatrix.at<double>(r, c) << "\n";
			}
		}


		//Distance Coeffs
		inStream >> rows;
		inStream >> columns;

		distanceCoeffs = Mat::zeros(rows, columns, CV_64F);

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double read = 0.0f;
				inStream >> read;
				distanceCoeffs.at<double>(r, c) = read;
				cout << distanceCoeffs.at<double>(r, c) << "\n";
			}
		}

		// close input stream
		inStream.close();

		// return successfully completed task
		return true;
	}

	return false;
}

int main() {

	// initialize camera matrix
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);

	// initialize distance coeffs
	Mat distanceCoeffs;

	// load calibration values
	loadCameraCalibration("CharCaliOut", cameraMatrix, distanceCoeffs);
	cout << "Coeffs Loaded" << endl;

	// start monitoring webcam
	startWebcamMonitoring(cameraMatrix, distanceCoeffs, arucoSquareDimension);

	return 0;
}
