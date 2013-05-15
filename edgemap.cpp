/*
 * edgemap.cpp
 *
 *  Created on: 2012-10-20
 *      Author: Tai
 */

#include "edgemap.h"

const int HOUGH_LINES_PROB = 25;	//	Probability of straight lines in Hough lines detection, lower means more detected lines
const double KERNEL_LENGTH = 0.025;	//	Fraction of height of image taken as smallest detectable straight line
const int SLOPE_ERROR = 25;			//	% error allowed for matching slopes, SLOPE_ERROR / SLOPE_ROUND
int SLOPE_ROUND = 1000;		//	Range of accuracy for slope, 100 = 2 decimal places, 1000 = 3 etc.
const int PARALLEL_DIST = 2;		//	Distance allowed for parallel lines to be considered close enough
const double CANNY_CALIBRATION = 0.66;		//	Factor to calibrate Canny threshold, THRESHOLD * CANNY_CALIBRATION
const int CORNER_DISTANCE = 10;		//	If the intersection of two edges are within this range from their corners, connect them
const int GAUSSIAN_KERNEL = 5;		//	Gaussan blur kernel determines how much less noise is in the image, higher = less noise, this number can only be odd

int imgHeight = -1;
int imgWidth = -1;

int distPoints( int x1, int y1, int x2, int y2 )
{
	return ( abs( x1 - x2 ) + abs( y1 - y2 ) );
}

//	Written by Taining Liu
//		Sept 20, 2012
//
//	Calculates the line intersection of two given edges
//	Takes in the two slopes, the coordinates of the slopes, two reference variables
//	Calculates the solution to the the formulas:
//		y = ax + b
//		y = cx + d
//	Outputs the results in the reference variables, xCoord and yCoord
void lineIntersect( Vec4i edge, Vec4i edgeCompare, int &xCoord, int &yCoord )
{
	double slopeA = 0;
	double slopeB = 0;

	Scalar slope = calcSlope( edge );
	Scalar slopeCompare = calcSlope( edgeCompare );

	bool verticalA = false;
	bool verticalB = false;

	if ( slope[0] != 0 )
	{
		slopeA = slope[0] / double( SLOPE_ROUND );
	}

	if ( slope[1] != 0 )
	{
		slopeA = double( SLOPE_ROUND ) / slope[1];
	}

	if ( slopeCompare[0] != 0 )
	{
		slopeB = slopeCompare[0] / double( SLOPE_ROUND );
	}

	if ( slopeCompare[1] != 0 )
	{
		slopeB = double( SLOPE_ROUND ) / slopeCompare[1];
	}

	if ( slope[2] != 0 )	//	If vertical set flag
	{
		verticalA = true;
	}

	if ( slope[3] != 0 )	//	If horizontal slope = 0;
	{
		slopeA = 0;
	}

	if ( slopeCompare[2] != 0 )
	{
		verticalB = true;
	}

	if ( slopeCompare[3] != 0 )
	{
		slopeB = 0;
	}

	double b = 0;	//	b
	double d = 0;	//	d
	double a = 0;	//	( a - c )

	if ( !verticalA )
	{
		b = edge[1] - slopeA * edge[0];
	}

	if ( !verticalB )
	{
		d = edgeCompare[1] - slopeB * edgeCompare[0];
	}

	if ( !verticalA && !verticalB )
	{
		a = slopeA - slopeB;
	}


	//cout << b << " " << d << " " << a << " " << lines[i][2] - lines[i][0] <<endl;

	if ( !( verticalA && verticalB ) || !( a == 0 ) )
	{
		if ( verticalA )	//	If either are vertical, x coordinate has to be set to x coordinate of vertical line
		{
			xCoord = edge[0];
			yCoord = round( ( slopeB * xCoord ) + d );
		}
		else if ( verticalB )
		{
			xCoord = edgeCompare[0];
			yCoord = round( ( slopeA * xCoord ) + b );
		}
		else
		{
			xCoord = round( ( d - b ) / a );
			yCoord = round( ( slopeA * xCoord ) + b );
		}
	}

	//cout << "Xcoord: " << xCoord << " Ycoord: " << yCoord << endl << endl;
	//cout << "slopeA: " << slopeA << " slopeB: " << slopeB << endl << endl;
}

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
Scalar calcSlope( Vec4i edge )
{
	Scalar slope;
	slope[0] = 0;
	slope[1] = 0;
	slope[2] = 0;
	slope[3] = 0;
	double y_dif = edge[1] - edge[3];
	double x_dif = edge[0] - edge[2];
	if ( ( abs( x_dif ) ) >= ( abs( y_dif ) ) )
	{
		slope[0] = round( ( y_dif / x_dif ) * SLOPE_ROUND );
	}
	else
	{
		slope[1] = round( ( x_dif / y_dif ) * SLOPE_ROUND );
	}
	if ( abs( x_dif ) <= PARALLEL_DIST )
	{
		slope[2] = 1;
		slope[0] = 0;
		slope[1] = 0;
	}
	if ( abs( y_dif ) <= PARALLEL_DIST )
	{
		slope[3] = 1;
		slope[0] = 0;
		slope[1] = 0;
	}
	return slope;
}

//	Written by Taining Liu
//		Sept 20, 2012
//	Takes in the directory of an image and returns
//	the coordinates of the detected edges in
//	vector<Vec4i> format.
//	Unbound array of Vec4i which is an array of 4 integers
//		Oct 20, 2012
//	Adjusted to better connect corners
//	Ignored case for connecting different shapes
vector<Vec4i> edgeMap( String directory )
{
	//	Load image as grayscale
	Mat img = imread( directory , 0);

	//	blur image to reduce noise
	GaussianBlur( img, img, Size( GAUSSIAN_KERNEL, GAUSSIAN_KERNEL ), 0, 0 );

	//	equalise colour histogram [ Not implemented ]
	//	equalizeHist( img, img );

	//	set the Canny threshold to be mean of colours
	//	gives threshold for each colour
	Scalar thresholdVector = mean(img);

	//	convert threshold vector into single value by averaging colours
	double threshold = thresholdVector[0] * CANNY_CALIBRATION;

	//	use Canny edge detector to detect edges on img and place results in img
	Canny( img, img, threshold*0.66, threshold*0.133, 3 );

	//	Get width, height of image
	imgWidth = img.cols;
	imgHeight = img.rows;

	//	Minimum straight line length detected [ Set to image width * KERNEL_LENGTH ]
	int houghLength = round( imgWidth * KERNEL_LENGTH );

	//	HoughLinesP calculates straight lines in edges detected
	//	Outputs results as Vector [ array of arrays of four integers ]
	//	lines[0] = first line
	//	lines[0][0] = x coord of start point
	//	lines[0][1] = y coord of start point
	//	lines[0][2] = x coord of end point
	//	lines[0][3] = y coord of end point
	vector<Vec4i> lines;
	HoughLinesP( img, lines, 1, CV_PI/360, HOUGH_LINES_PROB, houghLength , houghLength );

	//	Debug output
//
//	cvtColor(img, img, CV_GRAY2BGR);
//
//    for( size_t i = 0; i < lines.size(); i++ )
//    {
//    	Scalar color;
//
//    		color[0] = 255;
//    		color[1] = 0;
//    		color[2] = 0;
//
//		Vec4i l = lines[i];
//				line( img, Point(l[0], l[1]), Point(l[2], l[3]), color, 1, CV_AA);
//				imshow( "output" , img );
//    }

	//	Loop to help remove unnecessary edges
for( int k = 0; k < 5; k++ )
{
	for( size_t i = 0; i < lines.size(); i++ )
	{
		// If beginning or end of line are on edge of frame then remove line
//		if ( ( lines[i][0] <= CORNER_DISTANCE )
//			|| ( lines[i][0] >= imgWidth - CORNER_DISTANCE )
//			|| ( lines[i][1] <= CORNER_DISTANCE )
//			|| ( lines[i][1] >= imgHeight - CORNER_DISTANCE )
//			|| ( lines[i][2] <= CORNER_DISTANCE )
//			|| ( lines[i][2] >= imgWidth - CORNER_DISTANCE )
//			|| ( lines[i][3] <= CORNER_DISTANCE )
//			|| ( lines[i][3] >= imgHeight - CORNER_DISTANCE ) )
//		{
//			lines[i][0] = -1;
//			lines[i][1] = -1;
//			lines[i][2] = -1;
//			lines[i][3] = -1;
//		}

		//	Loop that cycles through remaining edges comparing with the first edge
		if ( lines[i][0] != -1 )
		{
			//	Use calcSlope to get slope of line
			Scalar slope = calcSlope( lines[i] );
			for( size_t j = 0; j < lines.size(); j++ )
			{
				//	Edge that you will be comparing every other edge to
				Vec4i edgeCompare = lines[j];
				if ( ( edgeCompare[0] != -1 ) && ( i != j ) )
				{
					//	Use calcSlope to get slope of comparison line
					Scalar slopeCompare = calcSlope( edgeCompare );

					//	Calculate slope of start point of first edge to end point of second edge to see if they
					//	are on the same line
					Vec4i lineCompare;
					lineCompare[0] = lines[i][0];
					lineCompare[1] = lines[i][1];
					lineCompare[2] = edgeCompare[2];
					lineCompare[3] = edgeCompare[3];
					Scalar slopeLine = calcSlope( lineCompare );

					//	If slopes in x and y slopes in edge and edgeCompare are non-zero assign them to slopeA and slopeB
					//	Make sure same slope value taken from lineSlope for later checking
					int slopeA = 0;
					int slopeB = 0;
					int slopeC = 0;
					if ( ( slope[0] != 0 ) || ( slopeCompare[0] != 0 ) || ( slope[1] != 0 ) || ( slopeCompare[1] != 0 ) || ( slopeLine[0] != 0 ) || ( slopeLine[1] != 0 ) )
					{
						if ( ( slope[0] != 0 ) && ( slopeCompare[0] != 0 ) && ( slopeLine[0] != 0 ) )
						{
							slopeA = slope[0];
							slopeB = slopeCompare[0];
							slopeC = slopeLine[0];
						}
						if ( ( slope[1] != 0 ) && ( slopeCompare[1] != 0 ) && ( slopeLine[1] != 0 ) )
						{
							slopeA = slope[1];
							slopeB = slopeCompare[1];
							slopeC = slopeLine[1];
						}
					}

					//	Boolean variables to determine if vertical or horizontal flags are set on both edges
					bool areVertical = ( ( slope[2] == 1 ) && ( slopeCompare[2] == 1 ) );
					bool areHorizontal = ( ( slope[3] == 1 ) && ( slopeCompare[3] == 1 ) );

					bool sameLine = false;
					bool sameSlope = false;

					//	If the x and y slopes on both edges are 0 and they are not parallel either vertically or horizontally
					//	ensure edges cannot be adjusted
					if ( slopeA != 0 || areVertical || areHorizontal )
					{
						int slopeAvg = round( ( slopeA + slopeB ) / 2.0 );

						//	Boolean variable determining whether both edges have same slope
						sameSlope = ( ( ( slopeA <= ( slopeB + SLOPE_ERROR ) ) && ( slopeA >= ( slopeB - SLOPE_ERROR ) ) ) || areVertical || areHorizontal );

						//	Boolean variable determining whether both edges are on same line
						sameLine = ( ( ( slopeC <= ( slopeAvg + SLOPE_ERROR ) ) && ( slopeC >= ( slopeAvg - SLOPE_ERROR ) ) ) || ( areVertical && ( slopeLine[2] == 1 ) ) || ( areHorizontal && ( slopeLine[3] == 1 ) ) );
					}

					//	If slopes of both edges are similar enough and both edges are close enough to same line
					//	combine coordinates to form longer line and remove second coordinate
					if ( sameSlope
						&& sameLine
						&& !( areVertical && ( abs( lines[i][0] - edgeCompare[0] ) > PARALLEL_DIST ) )
						&& !( areHorizontal && ( abs( lines[i][1] - edgeCompare[1] ) > PARALLEL_DIST ) )
						&& ( ( ( abs( lines[i][0] - edgeCompare[0] ) <= CORNER_DISTANCE * 2 )	&& ( abs( lines[i][1] - edgeCompare[1] ) <= CORNER_DISTANCE * 2 ) )
						|| ( ( abs( lines[i][2] - edgeCompare[2] ) <= CORNER_DISTANCE * 2 ) && ( abs( lines[i][3] - edgeCompare[3] ) <= CORNER_DISTANCE * 2 ) ) ) )
					{
//						cout << "Slope: " << slopeA << " " << slopeB << " " << slopeC << " " << endl;
//						cout << "Edge: 		" << lines[i][0] << " " << lines[i][1] << " " << lines[i][2] << " " << lines[i][3] << endl;
//						cout << "Edge Compare: 	" << edgeCompare[0] << " " << edgeCompare[1] << " " << edgeCompare[2] << " " << edgeCompare[3] << endl;

						//	If start point of second edge farther left than edge, assign start point
						//	of second edge to first edge
						if ( ( lines[i][0] - edgeCompare[0] ) > 0 )
						{
							lines[i][0] = edgeCompare[0];
							lines[i][1] = edgeCompare[1];
						}

//						if ( ( ( lines[i][1] - edgeCompare[1] ) > 0 ) && ( lines[i][0] < ( imgWidth / 2 ) ) && ( edgeCompare[0] < ( imgWidth / 2 ) ) )
//						{
//							lines[i][1] = edgeCompare[1];
//						}
						//	If end point of second edge farther left than edge, assign end point
						//	of second edge to first edge
						if ( ( edgeCompare[2] - lines[i][2] ) > 0 )
						{
							lines[i][2] = edgeCompare[2];
							lines[i][3] = edgeCompare[3];
						}

//						if ( ( ( edgeCompare[3] - lines[i][3] ) > 0 ) && ( lines[i][0] < ( imgWidth / 2 ) ) && ( edgeCompare[0] < ( imgWidth / 2 ) ) )
//						{
//							lines[i][3] = edgeCompare[3];
//						}


						// If lines are vertical, ensure the longest vertical line possible of
						// given edges.
						if ( areVertical )
						{
							int temp1, temp2;
							temp1 = min( min( edgeCompare[1], edgeCompare[3] ), min( lines[i][1], lines[i][3] ) );
							temp2 = max( max( edgeCompare[1], edgeCompare[3] ), max( lines[i][1], lines[i][3] ) );

							lines[i][1] = temp1;
							lines[i][3] = temp2;
						}

						// If lines are horizontal, ensure the horizontal vertical line possible
						// of given edges.
						if ( areHorizontal )
						{
							int temp1, temp2;
							temp1 = min( min( edgeCompare[0], edgeCompare[2] ), min( lines[i][0], lines[i][2] ) );
							temp2 = max( max( edgeCompare[0], edgeCompare[2] ), max( lines[i][0], lines[i][2] ) );

							lines[i][0] = temp1;
							lines[i][2] = temp2;
						}

//						cout << "Final Edge: 	" << lines[i][0] << " " << lines[i][1] << " " << lines[i][2] << " " << lines[i][3] << endl << endl;

						//	Set second edge to be non existent on img
						lines[j][0] = -1;
						lines[j][1] = -1;
						lines[j][2] = -1;
						lines[j][3] = -1;
					}
				}
			}
		}
	}
}

	//	Loop that detects closest end point of other line then connects the two points
	for( int k = 0; k < 5; k++ )
	{
		for( size_t i = 0; i < lines.size(); i++ )
		{
			int distStart = imgWidth;			//	Closest distance to start point
			int distEnd = imgWidth;				//	Closest distance to end point
			int closeStartx = -1;
			int closeStarty = -1;
			int closeEndx = -1;
			int closeEndy = -1;
			int closeLine = -1;

			//	Loop that cycles through remaining edges comparing with the first edge
			if ( lines[i][0] != -1 )
			{
				//	Use calcSlope to get slope of line
				Scalar slope = calcSlope( lines[i] );
				for( size_t j = 0; j < lines.size(); j++ )
				{
					Vec4i edgeCompare = lines[j];
					if ( ( edgeCompare[0] != -1 ) && ( i != j ) )
					{
						//	Use calcSlope to get slope of comparison line
						Scalar slopeCompare = calcSlope( edgeCompare );

						//	Determine intersecting points of both lines
						//	solve for two equations
						//	y = ax + b
						//	y = cx + d
						//	=> 0 = ( a - c ) + b - d
						//	=> x = ( d - b ) / a - c

						int xCoord = -1;
						int yCoord = -1;

						lineIntersect( lines[i], edgeCompare, xCoord, yCoord );

						int distStartcompare = imgWidth;
						int distEndcompare = imgWidth;

						//	Determine shortest distance between start point and intersect of both lines
						distStartcompare = distPoints( lines[i][0], lines[i][1], xCoord, yCoord );

						//	Determine shortest distance between end point and intersect of both lines
						distEndcompare = distPoints( lines[i][2], lines[i][3], xCoord, yCoord );

						//	Calculated distances smaller than existing closest distance, reassign closest distance
						//	assign j line number as closest line
						if ( ( distStartcompare < distStart ) && ( distStartcompare != 0 ) )
						{
							distStart = distStartcompare;
							closeStartx = xCoord;
							closeStarty = yCoord;
							closeLine = j;

							//cout << "distStart: " << distStart << " closeStart: " << j << endl;
							//cout << "distStartx: " << closeStartx << " closeStarty: " << closeStarty << endl;
						}
						if ( ( distEndcompare < distEnd ) && ( distEndcompare != 0 ) )
						{
							distEnd = distEndcompare;
							closeEndx = xCoord;
							closeEndy = yCoord;
							closeLine = j;

							//cout << "distEnd: " << distEnd << " closeEnd: " << j << endl;
							//cout << "distEndx: " << closeEndx << " distEndy: " << closeEndy << endl;
						}
					}

					//	If intersection is within CORNER_DISTANCE of a corner of the line being tested
					//	and the line being compared to, set both to be the intersection
					if ( ( abs( lines[i][0] - closeStartx ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][0] - closeStartx ) <= CORNER_DISTANCE )
					  && ( abs( lines[i][1] - closeStarty ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][1] - closeStarty ) <= CORNER_DISTANCE ) )
					{
						//cout << "Adjust start point " << closeStartx << " " << closeStarty << endl;
						lines[i][0] = closeStartx;
						lines[closeLine][0] = closeStartx;
						lines[i][1] = closeStarty;
						lines[closeLine][1] = closeStarty;
					}
					if ( ( abs( lines[i][0] - closeStartx ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][2] - closeStartx ) <= CORNER_DISTANCE )
					  && ( abs( lines[i][1] - closeStarty ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][3] - closeStarty ) <= CORNER_DISTANCE ) )
					{
						//cout << "Adjust start point " << closeStartx << " " << closeStarty << endl;
						lines[i][0] = closeStartx;
						lines[closeLine][2] = closeStartx;
						lines[i][1] = closeStarty;
						lines[closeLine][3] = closeStarty;
					}
					if ( ( abs( lines[i][2] - closeEndx ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][0] - closeEndx ) <= CORNER_DISTANCE )
					  && ( abs( lines[i][3] - closeEndy ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][1] - closeEndy ) <= CORNER_DISTANCE ) )
					{
						//cout << "Adjust end point " << closeEndx << " " << closeEndy << endl;
						lines[i][2] = closeEndx;
						lines[closeLine][0] = closeEndx;
						lines[i][3] = closeEndy;
						lines[closeLine][1] = closeEndy;
					}
					if ( ( abs( lines[i][2] - closeEndx ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][2] - closeEndx ) <= CORNER_DISTANCE )
					  && ( abs( lines[i][3] - closeEndy ) <= CORNER_DISTANCE )
					  && ( abs( lines[closeLine][3] - closeEndy ) <= CORNER_DISTANCE ) )
					{
						//cout << "Adjust end point " << closeEndx << " " << closeEndy << endl;
						lines[i][2] = closeEndx;
						lines[closeLine][2] = closeEndx;
						lines[i][3] = closeEndy;
						lines[closeLine][3] = closeEndy;
					}
				}
			}

		}
	}
		//Loop goes over all existing corners and connects them if they are within CORNER_DISTANCE
		for (size_t i = 0; i < lines.size(); i++) {
			for (size_t j = 0; j < lines.size(); j++) {
				if ((abs(lines[i][0] - lines[j][0]) <= CORNER_DISTANCE) && (abs(lines[i][1] - lines[j][1]) <= CORNER_DISTANCE)) {
					lines[i][0] = round(float(lines[i][0] + lines[j][0]) / 2);
					lines[j][0] = lines[i][0];
					lines[i][1] = round(float(lines[i][1] + lines[j][1]) / 2);
					lines[j][1] = lines[i][1];
				}
				if ((abs(lines[i][0] - lines[j][2]) <= CORNER_DISTANCE) && (abs(lines[i][1] - lines[j][3]) <= CORNER_DISTANCE)) {
					lines[i][0] = round(float(lines[i][0] + lines[j][2]) / 2);
					lines[j][2] = lines[i][0];
					lines[i][1] = round(float(lines[i][1] + lines[j][3]) / 2);
					lines[j][3] = lines[i][1];
				}
				if ((abs(lines[i][2] - lines[j][0]) <= CORNER_DISTANCE) && (abs(lines[i][3] - lines[j][1]) <= CORNER_DISTANCE)) {
					lines[i][2] = round(float(lines[i][2] + lines[j][0]) / 2);
					lines[j][0] = lines[i][2];
					lines[i][3] = round(float(lines[i][3] + lines[j][1]) / 2);
					lines[j][1] = lines[i][3];
				}
				if ((abs(lines[i][2] - lines[j][2]) <= CORNER_DISTANCE) && (abs(lines[i][3] - lines[j][3]) <= CORNER_DISTANCE)) {
					lines[i][2] = round(float(lines[i][2] + lines[j][2]) / 2);
					lines[j][2] = lines[i][2];
					lines[i][3] = round(float(lines[i][3] + lines[j][3]) / 2);
					lines[j][3] = lines[i][3];
				}
			}
		}

	return lines;
}


