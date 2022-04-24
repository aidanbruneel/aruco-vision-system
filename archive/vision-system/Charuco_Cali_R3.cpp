#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>
#include <ctime>

#include <sstream>
#include <fstream>

#pragma warning(disable : 4996)

using namespace std;
using namespace cv;

// Create variables for collecting info from frames
vector<vector<vector<Point2f>>> allCorners, tempCorners;
vector<vector<int>> allIds, tempIds;
vector<Mat> allImgs, tempImages;
Size imgSize;

// Calibration Board settings
int squaresX = 5;
int squaresY = 7;
double squareLength = 0.036; //need this from the user
double markerLength = 0.021; //also this
int dictionaryId = 10;

// Calibration program settings
string outputFile = "CharCaliOut";
bool showChessboardCorners = true;
int calibrationFlags = 0, totalImageCollect=60;
float aspectRatio = 1;

// Create ArUco parameters
Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create();
Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
Ptr<aruco::CharucoBoard> charucoboard = aruco::CharucoBoard::create(squaresX, squaresY, squareLength, markerLength, dictionary);
Ptr<aruco::Board> board = charucoboard.staticCast<aruco::Board>();

Mat cameraIntrinsics, distortionCoeffs;
vector<Mat> rvecs, tvecs;

vector<Mat> allCharucoCorners;
vector<Mat> allCharucoIds;
vector<Mat> filteredImages;

// saveCameraCalibration() function for saving calibrated camera intrinsics and distortion coefficients to a readable file
bool saveCameraCalibration(string name, Mat cameraIntrinsics, Mat distortionCoeffs)
{
    cout << "Writing Calibration File..." << endl;
    ofstream outStream(name);
    if (outStream)
    {
        uint16_t rows = cameraIntrinsics.rows;
        uint16_t columns = cameraIntrinsics.cols;

        outStream << rows << endl;
        outStream << columns << endl;

        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < columns; c++)
            {
                double value = cameraIntrinsics.at<double>(r, c);
                outStream << value << endl;
            }
        }

        rows = distortionCoeffs.rows;
        columns = distortionCoeffs.cols;

        outStream << rows << endl;
        outStream << columns << endl;

        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < columns; c++)
            {
                double value = distortionCoeffs.at<double>(r, c);
                outStream << value << endl;
            }
        }

        outStream.close();
        cout << "Calibration File Saved!" << endl;
        return true;

    }

    return false;
}

// collectImages() function for automatically capturing images when sufficient corners of calibration board are detected
bool collectImages(VideoCapture vid, int framesPerSecond, int imageCollect, int numMin)
{
    for (int i = 0; i < (framesPerSecond * numMin * 60); i++) {
        //int everyOther = floor((framesPerSecond * numMin * 60) / imageCollect);
        int everyOther = 8;
        Mat image, imageCopy, imageCopy_out;
        string cont;

        if (!vid.read(image))
            break;

        vector<int> ids;
        vector< vector<Point2f>> corners, rejected;

        // Detect markers
        aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);

        // Interpolate charuco corners
        Mat currentCharucoCorners, currentCharucoIds;
        if (ids.size() > 0)
            aruco::interpolateCornersCharuco(corners, ids, image, charucoboard, currentCharucoCorners,
                currentCharucoIds);

        // Draw detected markers
        image.copyTo(imageCopy);
        if (ids.size() > 0) aruco::drawDetectedMarkers(imageCopy, corners);

        if (currentCharucoCorners.total() > 0)
            aruco::drawDetectedCornersCharuco(imageCopy, currentCharucoCorners, currentCharucoIds);


        flip(imageCopy, imageCopy_out, 1);
        imshow("Webcam", imageCopy_out);

        char character = waitKey(1000 / framesPerSecond); // need to find another way to do this

        switch (character)
        {
        case 27:
            //exit
            return 0;
            break;
        default:
            // Automatically collect images
            if (!(i%everyOther) && ids.size() > 0 && currentCharucoCorners.total()>4) {
                tempCorners.push_back(corners);
                tempIds.push_back(ids);
                tempImages.push_back(image);
                imgSize = image.size();
                cout << "Saved Image!" << "   Image #: " << tempImages.size() << endl;
                cout << "Num of Corners: " << currentCharucoCorners.total() << endl;
            }
            cout << "Time Left: " << ((numMin * 60) - (floor(i/framesPerSecond))) << endl;
        }

        if (tempImages.size() == imageCollect) {
            bool approve=false;

            while (!approve) {
                cout << "Images collected so far: " << allImgs.size() << endl;
                cout << "Ready to start next stage of calibration? (Y/N)" << endl;
                cin >> cont;

                if (cont == "Y") {
                    approve = true;
                    for (int j = 0; j < tempIds.size(); j++) {
                        allCorners.push_back(tempCorners.at(j));
                        allIds.push_back(tempIds.at(j));
                        allImgs.push_back(tempImages.at(j));
                    }
                    tempCorners.clear();
                    tempIds.clear();
                    tempImages.clear();
                    return true;
                }
                else if (cont == "N") {
                    i = 0;
                    tempCorners.clear();
                    tempIds.clear();
                    tempImages.clear();
                    approve = true;
                }
                else {
                    cout << "That is not a valid input. Please try again." << endl;
                }
            }
        }

        if (allImgs.size() == totalImageCollect) {
            break;
        }
    }

    return true;
    
}

// Function for converting collected images and calibration board points into 
bool completeCalibration()
{
    // Initialize calibration variables
    bool CaliSuccess, saveOk;

    vector<vector<Point2f>> allCornersConcatenated;
    vector<int> allIdsConcatenated;
    vector<int> markerCounterPerFrame;
    markerCounterPerFrame.reserve(allCorners.size());

    for (unsigned int i = 0; i < allCorners.size(); i++) {
        markerCounterPerFrame.push_back((int)allCorners[i].size());
        for (unsigned int j = 0; j < allCorners[i].size(); j++) {
            allCornersConcatenated.push_back(allCorners[i][j]);
            allIdsConcatenated.push_back(allIds[i][j]);
        }
    }

    int nFrames = (int)allCorners.size();
    allCharucoCorners.reserve(nFrames);
    allCharucoIds.reserve(nFrames);

    for (int i = 0; i < nFrames; i++) {
        // Interpolate using camera parameters
        Mat currentCharucoCorners, currentCharucoIds;
        aruco::interpolateCornersCharuco(allCorners[i], allIds[i], allImgs[i], charucoboard, currentCharucoCorners, currentCharucoIds, cameraIntrinsics, distortionCoeffs);

        allCharucoCorners.push_back(currentCharucoCorners);
        allCharucoIds.push_back(currentCharucoIds);
        filteredImages.push_back(allImgs[i]);
    }

    if (allCharucoCorners.size() < 4) {
        cerr << "Not enough corners for calibration" << endl;
        return 0;
    }

    // Calibrate camera using charuco
    CaliSuccess = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, charucoboard, imgSize, cameraIntrinsics, distortionCoeffs, rvecs, tvecs, calibrationFlags);

    // Check calibration success and call saveCameraCalibration() if successful to write calibration file
    if (CaliSuccess) {
        cout << "Calibration Successful!" << endl;
        saveOk = saveCameraCalibration(outputFile, cameraIntrinsics, distortionCoeffs);
    }
    else {
        cout << "Calibration Failed!" << endl;
        saveOk = false;
    }

    if (!saveOk) {
        cerr << "Cannot Write Calibration File!" << endl;
        return 0;
    }

    return CaliSuccess;
}

int main()
{
    // Open video capture device
    VideoCapture vid(0);
    if (!vid.isOpened())
    {
        return 0;
    }
    namedWindow("Webcam", WINDOW_AUTOSIZE);
    
    // collectImages() settings
    int framesPerSecond = 20, numMin = 1, imageCollect = 10;
    

    // Begin calibration phases
    for (int k = 0; k < 7; k++) {
        cout << "Begin phase " << k + 1 << " of calibration. " << endl;
        collectImages(vid, framesPerSecond, imageCollect, numMin);
    }

    cout << "Finished Collecting Images!!" << endl;

    // Call completeCalibration() function to calculate camera's intrinsic parameters
    completeCalibration();

	return 0;
}
