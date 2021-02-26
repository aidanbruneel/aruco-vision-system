#include "Frame.h"

using namespace std;
using namespace cv;

Frame::Frame(Mat im)
{
	imCap = im;
	cvtColor(im, imGray, COLOR_BGR2GRAY);
	//adaptiveThreshold(imGray, imBW, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
	cvtColor(imGray, imScan, COLOR_GRAY2BGR);
	cvtColor(imGray, imPose, COLOR_GRAY2BGR);
	scanSingleMarkers(imGray);
	displayPose();
}

void Frame::scanSingleMarkers(Mat im)
{
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
	aruco::detectMarkers(im, markerDictionary, singleMarkerCorners, singleMarkerIDs);
	aruco::drawDetectedMarkers(imScan, singleMarkerCorners, singleMarkerIDs);
	aruco::estimatePoseSingleMarkers(singleMarkerCorners, singleMarkerSideLength, cameraIntrinsics, distortionCoeffs, rVecs, tVecs);
	computeEuclidianDistances();
}

void Frame::displayPose()
{
	system("CLS");
	cout << "--------------------------------------------------" << endl;
	cout << "\tDetected Markers = " << singleMarkerIDs.size() << endl;
	cout << "--------------------------------------------------" << endl;
	// Display Pose Estimation Values and Axes
	for (int i = 0; i < singleMarkerIDs.size(); i++)
	{
		aruco::drawAxis(imPose, cameraIntrinsics, distortionCoeffs, rVecs[i], tVecs[i], 0.1f);
		cout << " ID = " << singleMarkerIDs[i] << endl;
		cout << " Rot Vec   = " << rVecs[i] << endl;
		cout << " Trans Vec = " << tVecs[i] << endl;
		cout << " Euclidian = " << euclidianDistances[i] << endl;
		cout << "--------------------------------------------------" << endl;
	}
}

void Frame::computeEuclidianDistances()
{
	for (int i = 0; i < singleMarkerIDs.size(); i++)
	{
		euclidianDistances.push_back(sqrt(pow(tVecs[i][0], 2) + pow(tVecs[i][1], 2) + pow(tVecs[i][2], 2)));
	}

}