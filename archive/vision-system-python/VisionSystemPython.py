import numpy as np
import cv2
import cv2.aruco as aruco

from imageProcessing import initialize_frame, gray_and_thresh, draw_marker, draw_point, draw_vector, draw_crosshairs
from locateFunctions import marker_2_point_array, get_center_point

cap = cv2.VideoCapture(0)

while True:
    # Initialize Frame:
    # frame, frame_gray, frame_thresh = initialize_frame(cap) # Not sure why this does not work...
    ret, frame = cap.read()
    frame_gray, frame_thresh = gray_and_thresh(frame)
    # ArUco Decode:
    aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250)
    decode_parameters = aruco.DetectorParameters_create()
    marker_corners_old, marker_ids, marker_rejectedImgPoints = aruco.detectMarkers(frame, aruco_dict, parameters=decode_parameters)
    # Setup Tracked Frames:
    frame_tracked_old = frame.copy()
    frame_tracked = frame.copy()
    frame_tracked_old = draw_crosshairs(frame_tracked_old)

    if marker_corners_old.__len__() > 0:
        marker_corners = marker_2_point_array(marker_corners_old)
        # Compute and Draw Center Point and Vector:
        marker_center_point = get_center_point(marker_corners)
        # frame_tracked = draw_marker(frame_tracked, marker_corners, marker_center_point) # error: (-215:Assertion failed) size.width>0 && size.height>0 in function 'cv::imshow'
        frame_tracked_old = draw_point(frame_tracked_old, marker_center_point, radius=4)
        # frame_tracked_old = draw_vector(frame_tracked_old, marker_center_point) # error: (-215:Assertion failed) size.width>0 && size.height>0 in function 'cv::imshow'

        print(marker_ids, marker_corners_old, marker_center_point)
    
    # Draw Marker (ArUco Old Method):
    frame_tracked_old = aruco.drawDetectedMarkers(frame_tracked_old, marker_corners_old)

    # Displays:
    cv2.imshow('frame', frame)
    # cv2.imshow('frame_tracked', frame_tracked)
    cv2.imshow('frame_tracked_old', frame_tracked_old)
    # cv2.imshow('frame_gray', frame_gray)
    # cv2.imshow('frame_thresh', frame_thresh)

    # Store frame into previous frame
    prev_frame = frame.copy()
    prev_gray = frame_gray.copy()
    prev_thresh = frame_gray.copy()

    # Waitkey
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release Capture and Destroy All Windows
cap.release()
cv2.destroyAllWindows()
