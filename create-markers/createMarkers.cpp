#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/aruco/charuco.hpp"
#include "opencv2/calib3d.hpp"


#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;
using namespace cv;

void createArucoMarkers(string imageName, Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_1000))
{
	Mat outputMarker;

	for (int i = 0; i < 1024; i++)
	{
		aruco::drawMarker(markerDictionary, i, 1000, outputMarker, 1);
		ostringstream convert;
		convert << imageName << i << ".png";
		imwrite(convert.str(), outputMarker);
	}
}

void createDiamondMarkers(string imageName, Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50))
{
	Mat outputMarker;

	Vec4i currentIDs;
	int dictionarySize = 50;
	int i, j, k, l;

	for (i = 0; i < dictionarySize; i += 5)
	{
		for (j = 0; j < dictionarySize; j += 5)
		{
			for (k = 0; k < dictionarySize; k += 5)
			{
				for (l = 0; l < dictionarySize; l += 5)
				{
					i++;
					currentIDs = Vec4i(i, j, k, l);
					aruco::drawCharucoDiamond(markerDictionary, currentIDs, 1000, 540, outputMarker);
					ostringstream convert;
					string imageName = "4X4_50_DIAMOND_";
					convert << imageName << i << "_" << j << "_" << k << "_" << l << ".png";
					imwrite(convert.str(), outputMarker);

				}
			}
		}

	}
}

int main()
{
	// Parameters
	Ptr<aruco::Dictionary> arucoDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_1000);
	string arucoName = "4x4_1000_";
	Ptr<aruco::Dictionary> diamondDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
	string diamondName = "4X4_50_DIAMOND_";

	// Generate Images of ArUco Single Marker Dictionary
	createArucoMarkers(arucoName, arucoDictionary);

	// Generate Images of ChArUco Diamond Marker Dictionary
	//createDiamondMarkers(diamondName, diamondDictionary)
	
	return 1;
}