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
	Ptr<aruco::DetectorParameters> detectParams = aruco::DetectorParameters::create();
	aruco::detectMarkers(im, markerDictionary, singleMarkerCorners, singleMarkerIDs, detectParams, singleMarkerRejects, cameraIntrinsics, distortionCoeffs);
	aruco::drawDetectedMarkers(imScan, singleMarkerCorners, singleMarkerIDs);
	aruco::estimatePoseSingleMarkers(singleMarkerCorners, singleMarkerSideLength, cameraIntrinsics, distortionCoeffs, rVecs, tVecs);
	
	Mat Ri = Mat::zeros(Size(3, 3), CV_16F);
	Vec3d PYRi;
	Mat mtxR = Mat::zeros(Size(3, 3), CV_16F);
	Mat mtxQ = Mat::zeros(Size(3, 3), CV_16F);
	for (int i = 0; i < singleMarkerIDs.size(); i++)
	{
		Rodrigues(rVecs[i], Ri);
		R.push_back(Ri);
		PYRi = RQDecomp3x3(Ri, mtxR, mtxQ);
		PYR.push_back(PYRi);
	}
	
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
		cout << " Rot Matrix:" << endl << R[i] << endl;
		cout << " Pitch = " << -PYR[i][0] << endl;
		cout << " Yaw   = " << -PYR[i][1] << endl;
		cout << " Roll  = " << -PYR[i][2] << endl;
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
