
import cv2
import numpy as np
from pyzbar.pyzbar import decode
from locatingFunctions import centerPoint

## Parameters
lk_params = dict(winSize = (40,40),maxLevel = 4,criteria = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03)) # adjust these last 2 values. EPS is for speed, COUNT is for accuracy
cap_height = 480
cap_width = 620

## Video Capture
cap = cv2.VideoCapture(0)
cap.set(3,cap_width)
cap.set(4,cap_height)

## Setup previous frame
ret, prev_frame = cap.read()
prev_frame = cv2.flip(prev_frame,1)
prev_gray = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)
# prev_threshold = cv2.adaptiveThreshold(prev_gray, 255, cv2.CALIB_CB_ADAPTIVE_THRESH, cv2.THRESH_BINARY, 11, 2)
prev_threshold = cv2.threshold(prev_gray, 127, 255, cv2.THRESH_BINARY)

## Initial Optical Flow
prevPts = np.array([[[100,100]],[[200,200]],[[300,300]],[[400,400]]],dtype=np.float32)
mask_constant = np.zeros_like(prev_frame)
barcodeExists = False

while True:
    ## Set up current frame
    ret, frame = cap.read()
    frame = cv2.flip(frame,1)
    frame_gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    ret, frame_threshold = cv2.threshold(frame_gray, 127, 255, cv2.THRESH_BINARY)
    # frame_threshold = cv2.adaptiveThreshold(frame_gray, 255, cv2.CALIB_CB_ADAPTIVE_THRESH, cv2.THRESH_BINARY, 11, 2)

    ## Decode current frame for zbar Barcode
    mask_decode = np.zeros_like(frame)
    barcodeExists = False
    for barcode in decode(frame):
        barcodeExists = True
        barcodeData = barcode.data.decode('utf-8')
        #print(myData)
        #print(barcode.polygon)
        #print(barcode.polygon[1][1],barcode.polygon[1][2])

        ## Polygon Points
        pts = np.array([barcode.polygon],np.int32)
        pts = pts.reshape(-1,1,2)
        pt1x = pts[0][0][0]
        pt1y = pts[0][0][1]
        pt1 = (pt1x,pt1y)
        pt2x = pts[1][0][0]
        pt2y = pts[1][0][1]
        pt2 = (pt2x,pt2y)
        pt3x = pts[2][0][0]
        pt3y = pts[2][0][1]
        pt3 = (pt3x,pt3y)
        pt4x = pts[3][0][0]
        pt4y = pts[3][0][1]
        pt4 = (pt4x,pt4y)
        corner_pts = np.array([[[pt1x,pt1y]],[[pt2x,pt2y]],[[pt3x,pt3y]],[[pt4x,pt4y]]],dtype=np.float32)
        cv2.polylines(mask_decode, [pts], True, (0, 0, 255), 1)

        ## Center Point
        center = centerPoint(pt1, pt2, pt3, pt4)
        # print('Center: ', center[0], center[1])
        cv2.circle(mask_decode, center, 4, (0, 255, 0), 4)
        cv2.arrowedLine(mask_decode, (round(cap_width / 2), round(cap_height / 2)), center, (0, 0, 255), 2)
        # centerNormalized = (round(((center[0]-cap_width/2)/(cap_width/2)),3), round(((cap_height-center[1]-cap_height/2)/(cap_width/2)),3)) # Normalized value from 0.0 to 1.0
        # print('x =', centerOffset[0], ',', 'y =', centerOffset[1])

        ## Rectangle Points and Data Label
        ptsRect = barcode.rect
        cv2.putText(mask_decode, barcodeData, (ptsRect[0], ptsRect[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 255, 0), 2)
        cv2.putText(mask_decode, 'P1', (pts[0][0][0], pts[0][0][1]), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 255), 2)
        cv2.putText(mask_decode, 'P2', (pts[1][0][0], pts[1][0][1]), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 255), 2)
        cv2.putText(mask_decode, 'P3', (pts[2][0][0], pts[2][0][1]), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 255), 2)
        cv2.putText(mask_decode, 'P4', (pts[3][0][0], pts[3][0][1]), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 255), 2)

    if barcodeExists == True:
        good_old = corner_pts
        good_new = corner_pts
        mask_constant = np.zeros_like(frame)
        #nextPts, status, err = cv2.calcOpticalFlowPyrLK(prev_gray, frame_gray, prevPts, None, **lk_params)
    else:
        nextPts, status, err = cv2.calcOpticalFlowPyrLK(prev_gray, frame_gray, prevPts, None, **lk_params)
        good_new = nextPts[status==1]
        good_old = prevPts[status==1]
        #cv2.polylines(frame, nextPts, True, (0, 255, 0), 1)

    cv2.circle(frame, (prevPts[0][0][0],prevPts[0][0][1]), 4, (255, 255, 255), 4)

    for i,(new,old) in enumerate(zip(good_new,good_old)):
        a,b = new.ravel()
        c,d = old.ravel()
        cv2.line(mask_constant, (a,b),(c,d), (0,255,0), 2)
        cv2.circle(frame,(a,b),5,(0,255,0),-1)

    ## Crosshairs
    cv2.line(mask_constant, (0, round(cap_height / 2)), (cap_width, round(cap_height / 2)), (0, 0, 20), 2)
    cv2.line(mask_constant, (round(cap_width / 2), 0), (round(cap_width / 2), cap_height), (0, 0, 20), 2)

    ## Image Summation

    mask = cv2.add(mask_constant,mask_decode)
    img = cv2.add(frame, mask)
    cv2.imshow('QR Locator', img)
    cv2.imshow('Grayscale Image', frame_gray)

    ## waitKey
    k = cv2.waitKey(30) & 0xFF
    if k == 27:
        break
    ## Previous Points and Previous Frame Reassignment
    prev_gray = frame_gray.copy()
    prev_threshold = frame_threshold.copy()
    prevPts = good_new.reshape(-1, 1, 2)

cv2.destroyAllWindows()
cap.release()