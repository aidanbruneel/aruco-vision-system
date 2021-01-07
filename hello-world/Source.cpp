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

const float calibrationSquareDimension = 0.02425f; //mm
const float arucoSquareDimension = 0.2000f; // NOT MEASURED

void createArucoMarkers()
{
	Mat outputMarker;

	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	for (int i = 0; i < 50; i++)
	{
		aruco::drawMarker(markerDictionary, i, 500, outputMarker, 1);
		ostringstream convert;
		string imageName = "4x4Marker_";
		convert << imageName << i << ".jpg";
		imwrite(convert.str(), outputMarker);
	}

}

VideoCapture cameraSetup()
{
	VideoCapture cap(0, CAP_DSHOW);
	/*
	if(!cap.isOpened())
	{
		return -1;
	}
	*/
	return cap;

}

int scanCamera()
{
	Mat frame;

	vector<int> markerIds;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;
	aruco::DetectorParameters parameters;

	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	VideoCapture cap = cameraSetup();
	namedWindow("Webcam", WINDOW_AUTOSIZE);

	while (true)
	{
		if (!cap.read(frame)) break;

		aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds);

		aruco::drawDetectedMarkers(frame, markerCorners, markerIds);

		imshow("Webcam", frame);
		if (waitKey(1000/30) >= 0) break;
	}

	return 1;

}

int main(int argv, char* argc)
{
	scanCamera();
}