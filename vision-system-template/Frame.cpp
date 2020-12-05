#include "Frame.h"
#include <iostream>
using namespace std;

void Frame::scanIm() {
	// Simulation code:
	if (getIm() > 5) numMarkers = getIm() / 2;
	else numMarkers = getIm();
	for (int i = numMarkers; i > 0; i--) {
		if (i % 10) VecAnchor.push_back(i);
		else VecPoi.push_back(i);
	}
	/*
	Final Form:
	scanIm() should take an image and scan it for markers. Once it has the array of corner points and id #s,
	it creates a Marker (Poi/Anchor) object for each id, sets its corner points, calculates its own center point,
	and (eventually) estimates the camera's pose from it (UNLESS THIS IS TAXING ON THE COMPUTATION TO DO FOR EVERY MARKER).
	Each Marker object is stored in VecPoi or VecAnchor, since that is where they are constructed (no Marker object construction in main() ).

	*/
}

void Frame::drawIm() {
	ImDrawn = Im + numMarkers;
	// cout << ImDrawn << endl;
}

Frame::Frame(int im) {
	setIm(im);
	setImGray(im + 1);		//UNTESTED:	//setImGray(cv2::cvtColor(im, cv2::COLOR_BGR2GRAY))
	setImThres(im + 2);		//UNTESTED:	//setImThres()
										//cv2::detectMarkers()
	scanIm();

	drawIm();

	// Display:
	cout << "Im: " << getIm() << ", ImGray: " << getImGray() << ", ImThres: " << getImThres() << ", ImDrawn: " << getImDrawn() << ", numMarkers: " << numMarkers << endl;
	cout << "VecPoi: ";
	std::copy(VecPoi.begin(), VecPoi.end(), std::ostream_iterator<int>(std::cout, " "));
	cout << endl;
	cout << "VecAnchor: ";
	std::copy(VecAnchor.begin(), VecAnchor.end(), std::ostream_iterator<int>(std::cout, " "));
	cout << endl;
}

