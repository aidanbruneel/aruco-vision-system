#include "Frame.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <queue>

using namespace std;

class Marker {
public:
	int i = 1;

	int ** getCornerPixels() {
		// tbh i kinda wanna use a 2D array
	}

private:
	int cornerPixels[4][2];

};
/*
class Frame {

public:
	Frame(int im) {
		setIm(im);
		setImGray(im + 1);		//UNTESTED:	//setImGray(cv2::cvtColor(im, cv2::COLOR_BGR2GRAY))
		setImThres(im + 2);		//UNTESTED:	//setImThres()
											//cv2::detectMarkers()
		scanIm();
		drawIm();
		
		cout << "Im: " << getIm() << ", ImGray: " << getImGray() << ", ImThres: " << getImThres() << ", ImDrawn: " << getImDrawn() << ", numMarkers: " << numMarkers << endl;
		cout << "VecPoi: ";
		std::copy(VecPoi.begin(), VecPoi.end(), std::ostream_iterator<int>(std::cout, " "));
		cout << endl;
		cout << "VecAnchor: ";
		std::copy(VecAnchor.begin(), VecAnchor.end(), std::ostream_iterator<int>(std::cout, " "));
		cout << endl;
	}
	void setIm(int im) {
		Im = im;
	}
	void setImGray(int imGray) {
		ImGray = imGray;
	}
	void setImThres(int imThres) {
		ImThres = imThres;
	}
	void pushBackVecPoi(int i) {
		VecPoi.push_back(i);
	}
	void pushBackVecAnchor(int i) {
		VecAnchor.push_back(i);
	}
	int getIm(){
		return Im;
	}
	int getImGray() {
		return ImGray;
	}
	int getImThres() {
		return ImThres;
	}
	int getImDrawn() {
		return ImDrawn;
	}
	vector<int> getVecPoi() {
		return VecPoi;
	}
	vector<int> getVecAnchor() {
		return VecAnchor;
	}
	void scanIm() {
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
/*
	}
	void drawIm() {
		ImDrawn = Im + numMarkers;
	}
private:
	int Im = 0;
	int ImGray = 0;
	int ImThres = 0;
	int ImDrawn = 0;

	int numMarkers = 0;

	vector<int> VecPoi;
	vector<int> VecAnchor;

};
*/

int main() {
	
	while (1) {
		int frameCap = 1;
		Frame currentFrame(frameCap);
	}
	return 69;
}
