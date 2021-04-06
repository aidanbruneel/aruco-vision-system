#include "Frame.h"

using namespace std;
using namespace cv;

Frame::Frame(Mat im, int id, Ptr<aruco::Dictionary> dictionary)
{
	relevantID = id;
	markerDictionary = dictionary;
	// Image Processing
	imCap = im;
	cvtColor(im, imGray, COLOR_BGR2GRAY);
	//adaptiveThreshold(imGray, imBW, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
	cvtColor(imGray, imScan, COLOR_GRAY2BGR);
	cvtColor(imGray, imDiamond, COLOR_GRAY2BGR);
	cvtColor(imGray, imPose, COLOR_GRAY2BGR);
	// Scan grayscale image for ArUco single markers and perform Pose Estimation on any detected markers
	scanMarkers(imGray);
	// Display Pose Estimation data and draw marker axes
	displayPose();
}

void Frame::scanMarkers(Mat im)
{
	//Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
	Ptr<aruco::DetectorParameters> detectParams = aruco::DetectorParameters::create();
	aruco::detectMarkers(im, markerDictionary, singleMarkerCorners, singleMarkerIDs, detectParams, singleMarkerRejects, cameraIntrinsics, distortionCoeffs);
	aruco::drawDetectedMarkers(imScan, singleMarkerCorners, singleMarkerIDs);
	
	/*
	if (singleMarkerIDs.size() > 0)
	{
		aruco::detectCharucoDiamond(im, singleMarkerCorners, singleMarkerIDs, 1/diamondMarkerArucoSquareRatio, diamondMarkerCorners, diamondMarkerIDs, cameraIntrinsics, distortionCoeffs);
		aruco::drawDetectedDiamonds(imDiamond, diamondMarkerCorners, diamondMarkerIDs);
	}
	*/

	aruco::estimatePoseSingleMarkers(singleMarkerCorners, singleMarkerSideLength, cameraIntrinsics, distortionCoeffs, rVecs, tVecs);
	
	// Convert Axis-Angle representation of pose to Rodrigues Rotation Matrix, then perform RQ Decomposition to obtain Pitch, Yaw, and Roll
	Mat Rcmi = Mat::zeros(Size(3, 3), CV_64F);
	Mat Rmci = Mat::zeros(Size(3, 3), CV_64F);
	Mat Tcmi = Mat::zeros(Size(4, 4), CV_64F);
	Mat Tmci = Mat::zeros(Size(4, 4), CV_64F);
	Vec3d PYRcmi, PYRmci;
	Mat mtxR = Mat::zeros(Size(3, 3), CV_64F);
	Mat mtxQ = Mat::zeros(Size(3, 3), CV_64F);
	for (int i = 0; i < singleMarkerIDs.size(); i++)
	{
		// Rotation Matrix (Camera->Marker)
		Rodrigues(rVecs[i], Rcmi);
		Rcm.push_back(Rcmi);
		// Homogeneous Transformation Matrix (Camera->Marker)
		Rcmi.copyTo(Tcmi(Rect(0, 0, Rcmi.cols, Rcmi.rows)));
		Tcmi.at<double>(0, 3) = tVecs[i][0];
		Tcmi.at<double>(1, 3) = tVecs[i][1];
		Tcmi.at<double>(2, 3) = tVecs[i][2];
		Tcmi.at<double>(3, 3) = 1.0000;
		Tcm.push_back(Tcmi);
		// Pitch, Yaw, and Roll (Camera->Marker)
		PYRcmi = RQDecomp3x3(Rcmi, mtxR, mtxQ);
		PYRcm.push_back(PYRcmi);
		// Homogeneous Transformation Matrix (Marker->Camera)
		Tmci = Tcmi.inv();
		Tmc.push_back(Tmci);
		// Rotation Matrix (Marker->Camera)
		Rmci = Tmci(Rect(0, 0, 3, 3));
		Rmc.push_back(Rmci);
		// Pitch, Yaw, and Roll (Marker->Camera)
		PYRmci = RQDecomp3x3(Rmci, mtxR, mtxQ);
		PYRmc.push_back(PYRmci);
	}
	
	// Calculate Euclidian distance from Translation Vector
	computeEuclidianDistances();
	computeGuidanceAngles();
	for (int i = 0; i < singleMarkerIDs.size(); i++)
	{
		writeDataPackets(euclidianDistances[i], guidanceAngles[i]);
	}
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
		if (singleMarkerIDs[i] == relevantID)
		{
			aruco::drawAxis(imPose, cameraIntrinsics, distortionCoeffs, rVecs[i], tVecs[i], 0.07f);
			cout << " ID = " << singleMarkerIDs[i] << endl;
		
			//cout << fixed << setprecision(4) << " Rot Vec   = " << rVecs[i] << endl;
			//cout << fixed << setprecision(4) << " Trans Vec = " << tVecs[i] << endl;
			cout << fixed << setprecision(4) << " Euclidian = " << euclidianDistances[i] << endl;
		
			//cout << " Rcm:" << endl;
			//printMat(Rcm[i], 4);
			cout << " Tcm:" << endl;
			printMat(Tcm[i], 4);
			cout << fixed << setprecision(4) << " PYRcm:" << endl << PYRcm[i] << endl;

			//cout << " Rmc:" << endl;
			//printMat(Rmc[i], 4);
			cout << " Tmc:" << endl;
			printMat(Tmc[i], 4);
			cout << fixed << setprecision(4) << " PYRmc:" << endl << PYRmc[i] << endl;
			//cout << fixed << setprecision(4) << " Zd:" << endl << Zd[i] << endl;
			//cout << fixed << setprecision(4) << " Zc:" << endl << Zc[i] << endl;
			cout << fixed << setprecision(4) << " Guidance Angle:" << endl << guidanceAngles[i] << endl;
			cout << " Data Packet:" << endl << dataPackets[i] << endl;
			cout << "--------------------------------------------------" << endl;
		}
		
	}
}

void Frame::computeEuclidianDistances()
{
	for (int i = 0; i < singleMarkerIDs.size(); i++)
	{
		euclidianDistances.push_back(sqrt(pow(tVecs[i][0], 2) + pow(tVecs[i][1], 2) + pow(tVecs[i][2], 2)));
	}
}

void Frame::computeGuidanceAngles()
{
	for (int i = 0; i < singleMarkerIDs.size(); i++)
	{
		double dx = Tmc[i].at<double>(0, 3);
		double dz = Tmc[i].at<double>(2, 3);
		double rd = sqrt(pow(dx, 2) + pow(dz, 2));
		Vec2d zd = Vec2d(-dx/rd, -dz/rd);
		Zd.push_back(zd);

		double ax = Tmc[i].at<double>(0, 2);
		double az = Tmc[i].at<double>(2, 2);
		double ra = sqrt(pow(ax, 2) + pow(az, 2));
		Vec2d zc = Vec2d(ax / ra, az / ra);
		Zc.push_back(zc);

		double cameraAngle = atan2(zc[0], zc[1]) * 180.0000 / 3.1415926535;
		double desiredAngle = atan2(zd[0], zd[1]) * 180.0000 / 3.1415926535;

		double guidanceAngle = desiredAngle - cameraAngle;

		guidanceAngles.push_back(guidanceAngle);
	}
}

void Frame::writeDataPackets(double dist, double ang)
{
	ostringstream streamStr;
	streamStr << fixed << setprecision(4);
	streamStr << "d" << dist << "a" << ang << "f";
	string dataPacket = streamStr.str();
	dataPackets.push_back(dataPacket);
}

void Frame::printMat(Mat mat, int precision)
{
	for (int i = 0; i < mat.size().height; i++)
	{
		cout << "[";
		for (int j = 0; j < mat.size().width; j++)
		{
			cout << setprecision(precision) << mat.at<double>(i, j);
			if (j != mat.size().width - 1)
				cout << ", ";
			else
				cout << "]" << endl;
		}
	}
}
