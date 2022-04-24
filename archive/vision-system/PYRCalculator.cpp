#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;
using namespace cv;

int main()
{
	double Rx, Ry, Rz;
	cout << " Please enter rVecs values:" << endl;
	cout << " Rx: ";
	cin >> Rx;
	cout << " Ry: ";
	cin >> Ry;
	cout << " Rz: ";
	cin >> Rz;
	Vec3d rVec = Vec3d(Rx, Ry, Rz);
	cout << " rVec = " << rVec << endl;

	Mat R = Mat::zeros(Size(3, 3), CV_16F);
	Vec3d PYR;
	Mat mtxR = Mat::zeros(Size(3, 3), CV_16F);
	Mat mtxQ = Mat::zeros(Size(3, 3), CV_16F);
	Rodrigues(rVec, R);
	cout << endl << " R Matrix =" << endl << R << endl << endl;
	PYR = RQDecomp3x3(R, mtxR, mtxQ);
	cout << " Pitch = " << -PYR[0] << endl;
	cout << " Yaw   = " << -PYR[1] << endl;
	cout << " Roll  = " << -PYR[2] << endl;
}