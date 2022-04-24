#pragma once
#include <iostream>
#include <vector>
using namespace std;

class Frame {

public:
	// Constructor
	Frame(int im);
	/*
	* Performs grayscale and threshold operations on im, scans im for Marker Objects, and populates the Poi and Anchor vectors with all created Marker Objects.
	* Once the Markers are recognized, it draws the Markers on the original image (on ImDraw).
	*/
	void scanIm();
	void drawIm();
	// Setters:
	void setIm(int im) {
		Im = im;
	}
	void setImGray(int imGray) {
		ImGray = imGray;
	}
	void setImThres(int imThres) {
		ImThres = imThres;
	}
	// Pushbacks:
	void pushBackVecPoi(int i) {
		VecPoi.push_back(i);
	}
	void pushBackVecAnchor(int i) {
		VecAnchor.push_back(i);
	}
	// Getters:
	int getIm() {
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
	// Source Functions (see Frame.cpp for implementation):

private:
	// Image Variables:
	int Im = 0;
	int ImGray = 0;
	int ImThres = 0;
	int ImDrawn = 0;
	// Marker Variables:
	int numMarkers = 0;
	vector<int> VecPoi;
	vector<int> VecAnchor;
};
