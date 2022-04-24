import cv2 as cv
import numpy as np


def gray_and_thresh(im, global_threshold=127, adaptive_mode=0):
    """Return gray and threshold-binary versions of im"""
    im_gray = cv.cvtColor(im, cv.COLOR_BGR2GRAY)
    # Adaptive Thresholding
    if adaptive_mode == 1:
        im_thresh = cv.adaptiveThreshold(im, 255, cv.ADAPTIVE_THRESH_MEAN_C, cv.THRESH_BINARY, 11, 2)
    elif adaptive_mode == 2:
        im_thresh = cv.adaptiveThreshold(im, 255, cv.CALIB_CB_ADAPTIVE_THRESH, cv.THRESH_BINARY, 11, 2)
    elif adaptive_mode == 3:
        im_thresh = cv.adaptiveThreshold(im, 255, cv.ADAPTIVE_THRESH_GAUSSIAN_C, cv.THRESH_BINARY, 11, 2)
    # Global Thresholding
    else:
        ret, im_thresh = cv.threshold(im_gray, global_threshold, 255, cv.THRESH_BINARY)
    return im_gray, im_thresh


def draw_crosshairs(im, colour=(0, 0, 0)):
    """Draw crosshairs at center of im"""
    im_height, im_width, ret = im.shape
    cv.line(im, (0, round(im_height / 2)), (im_width, round(im_height / 2)), color=colour, thickness=1)
    cv.line(im, (round(im_width / 2), 0), (round(im_width / 2), im_height), color=colour, thickness=1)
    return im


def initialize_frame(cap, mirror=True):
    ret, frame = cap.read()
    gray, thresh = gray_and_thresh(frame)
    if mirror:
        frame = cv.flip(frame, 1)
        gray = cv.flip(gray, 1)
        thresh = cv.flip(thresh, 1)
    return frame, gray, thresh


def draw_point(im, point, radius=1, colour=(0, 0, 255)):
    """Draw a single point on im"""
    cv.circle(im, point, radius=radius, color=colour, thickness=-1)
    return im


def draw_marker(im, corner_points, center_point, corner_colour=(0, 255, 0), center_colour=(255, 0, 0),
                outline_colour=(255, 0, 255)):
    """Draw a marker's corner points, edges, and center point on im"""
    # Draw polygon
    marker_polygon = np.array([[corner_points[0][0], corner_points[0][1]], [corner_points[1][0], corner_points[1][1]],
                               [corner_points[2][0], corner_points[2][1]], [corner_points[3][0], corner_points[3][1]]],
                              dtype=np.int32)
    cv.polylines(im, [marker_polygon], True, outline_colour, 1)
    # Draw and label corner points
    for i in range(0, 4):
        draw_point(im, corner_points[i], corner_colour)
        cv.putText(im, "P{}".format(i + 1), corner_points[i], cv.FONT_HERSHEY_SIMPLEX, 0.5, corner_colour, 1)
    # Draw center point
    draw_point(im, center_point, center_colour)


def draw_vector(im, point, colour=(0, 0, 255)):
    """Draw a vector from the center of im and a specified point"""
    im_height, im_width, ret = im.shape
    cv.arrowedLine(im, (round(im_width / 2), round(im_height / 2)), point, color=colour, thickness=2)

