/*
 * edgemap.h
 *
 *  Created on: 2012-10-20
 *      Author: Tai
 */

#ifndef EDGEMAP_H_
#define EDGEMAP_H_

#include "opencv2/opencv.hpp"
#include "stdio.h"
#include <iostream>

using namespace cv;
using namespace std;

int distPoints( int x1, int y1, int x2, int y2 );

//	Written by Taining Liu
//		Sept 20, 2012
//
//	Calculates the line intersection of two given edges
//	Takes in the two slopes, the coordinates of the slopes, two reference variables
//	Calculates the solution to the the formulas:
//		y = ax + b
//		y = cx + d
//	Outputs the results in the reference variables, xCoord and yCoord
void lineIntersect( Vec4i edge, Vec4i edgeCompare, int &xCoord, int &yCoord );

//	Written by Taining Liu
//		Sept 20, 2012
//
//	Calculates slope and returns Scalar representing slope
//	Takes in Vec4i variable called edge, calculates difference between x and y points in edge
//	If x difference larger than y, divide y dif by x dif
//	If y difference larger than x, divide x dif by y dif
//	Multiply by SLOPE_ROUND
//	If x dif is 0 set flag for vertical line
//	If y dif is 0 set flag for horizontal line
//	To ensure range:
//		Scalar[0] is slope from 0 to 1 from 1 to SLOPE_ROUND
//		Scalar[1] is slope from infinity to 1 from 1 to SLOPE_ROUND
//		Scalar[2] is flag for vertical line
//		Scalar[3] is flag for horizontal line
Scalar calcSlope( Vec4i edge );

//	Written by Taining Liu
//		Sept 20, 2012
//	Takes in the directory of an image and returns
//	the coordinates of the detected edges in
//	vector<Vec4i> format.
//	Unbound array of Vec4i which is an array of 4 integers
//		Oct 20, 2012
//	Adjusted to better connect corners
//	Ignored case for connecting different shapes
vector<Vec4i> edgeMap( String directory );

#endif /* EDGEMAP_H_ */
