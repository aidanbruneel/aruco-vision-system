import numpy as np


def get_center_point(corner_points):
    xdiff = corner_points[2][0] - corner_points[0][0], corner_points[1][0] - corner_points[3][0]
    ydiff = corner_points[2][1] - corner_points[0][1], corner_points[1][1] - corner_points[3][1]

    def det(a, b):
        return a[0] * b[1] - a[1] * b[0]

    div = det(xdiff, ydiff)
    if div == 0:
        raise Exception('Lines do not intersect')

    d = (det(*([corner_points[2], corner_points[0]])), det(*([corner_points[1], corner_points[3]])))
    x = det(d, xdiff) / div
    x = round(x)
    y = det(d, ydiff) / div
    y = round(y)
    center_point = (x, y)
    return center_point


def marker_2_point_array(marker_corners):
    point_array = [(0, 0), (0, 0), (0, 0), (0, 0)]
    for i in range(0, 4):
        point_array[i] = (int(round(marker_corners[0][0][i][0])), int(round(marker_corners[0][0][i][1])))
    return point_array

