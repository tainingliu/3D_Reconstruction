//#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "stdio.h"
#include <iostream>

#include "edgemap.h"
#include "orthogonalize.h"
#include "coordGeneration.h"

extern const int GAUSSIAN_KERNEL = 23;

using namespace cv;
using namespace std;

double CANNY_TEST = 0.66;

int main( int argc, char** argv )
{
	if ( argc <= 1 )
	{
		cout << "No image given!" << endl;
		return 0;
	}

	string input = argv[1];

	vector<Vec4i> lines2 = edgeMap( input );

	vector<vector<Vec4i> > lines = orthogonalize( lines2 );

	vector<vector<Vec3i> > objectCoordinates = coordGenerate( lines );

	//	Load image as grayscale
	Mat img = imread( input , 0);

	//	blur image to reduce noise
	GaussianBlur( img, img, Size( GAUSSIAN_KERNEL, GAUSSIAN_KERNEL ), 0, 0 );

	//	equalise colour histogram [ Not implemented ]
	// equalizeHist( img, img );

	//	set the Canny threshold to be mean of colours
	//	gives threshold for each colour
	Scalar thresholdVector = mean(img);

	//	convert threshold vector into single value by averaging colours
	double threshold = thresholdVector[0] * CANNY_TEST;

	//	ouput img array
	Mat cdst;

	//	use Canny edge detector to detect edges on img and place results in img
	Canny( img, cdst, threshold*0.66, threshold*0.133, 3 );

	//	since imported as grayscale need to convert type to add color
	cvtColor(cdst, cdst, CV_GRAY2BGR);

    //Print out detected edges
//    int j = 0;

    Scalar color1;

    cout << "Edge map coordinates:" << endl;

    for( size_t i = 0; i < lines2.size(); i++ )
    {
    	color1[0] = 0;
    	color1[1] = 255;
    	color1[2] = 0;

		Vec4i l = lines2[i];
    	if ( l[0] != -1 )
    	{

			//if ( j == 10 )
			//{
				line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), color1, 1, CV_AA);
				cout << l[0] << " " << l[1] << " " << l[2] << " " << l[3] << endl;
				//imshow( "test" , cdst );
			//}
    	}
    }

    for( size_t j = 0; j < lines.size(); j++ )
    {
    	vector<Vec4i> tempObj = lines[j];
		for( size_t i = 0; i < tempObj.size(); i++ )
		{
			color1[0] = 0;
			color1[1] = 0;
			color1[2] = 255;

			Vec4i l = tempObj[i];
			if ( l[0] != -1 )
			{

				//cout << l[0] << " " << l[1] << " " << l[2] << " " << l[3] << endl;
				//j++;
				//cout << "line " << j << endl;
				//if ( j == 10 )
				//{
					line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), color1, 1, CV_AA);
					//imshow( "test" , cdst );
				//}
			}
		}
	}

    cout << "3D coordinates:" << endl;

    for( size_t j = 0; j < objectCoordinates.size(); j++ )
    {
    	cout << "Object: " << j << endl;
    	vector<Vec3i> coordinates = objectCoordinates[j];

		for( size_t i = 0; i < coordinates.size(); i++ )
		{
			Vec3i l = coordinates[i];

			cout << l[0] << " " << l[1] << " " << l[2] << endl;

			circle( cdst, Point( l[0], l[1] ), 3, Scalar( 255, 0, 0 ), 1 , 1 );
		}
    }

	namedWindow("result", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );

	imshow("result", cdst);

    //imwrite( "C:/test2.jpg", cdst );

    waitKey(0);

    return 0;

}
