def centerPoint(pt1, pt2, pt3, pt4):
    xdiff = pt3[0]-pt1[0], pt2[0]-pt4[0]
    ydiff = pt3[1]-pt1[1], pt2[1]-pt4[1]

    def det(a,b):
        return a[0] * b[1] - a[1] * b[0]

    div = det(xdiff,ydiff)
    if div == 0:
        raise Exception('Lines do not intersect')

    d = (det(*([pt3,pt1])),det(*([pt2,pt4])))
    x = det(d,xdiff) / div
    x = round(x)
    y = det(d,ydiff) / div
    y = round(y)
    center = (x,y)
    return center
