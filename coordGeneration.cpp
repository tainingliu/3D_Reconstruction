/*
 * coordGeneration.cpp
 *
 *  Created on: 2013-01-15
 *      Author: Tai
 */

#include "coordGeneration.h"
#include "edgemap.h"
#include "math.h"
#include <vector>

extern double ANGLE_OF_INCLINATION;
extern double APERTURE_WIDTH;
extern int FOCAL_LENGTH;
extern int imgWidth;
extern int imgHeight;


vector<vector<Vec3i> > coordGenerate( vector<vector<Vec4i> > orthogonalizedObjects )
{
	vector<vector<Vec3i> > objects;

	int frontObjectIndex = 0;
	int lowestPoint = 0;

	for ( size_t i = 0; i < orthogonalizedObjects.size(); i++ )
	{
		vector<Vec4i> orthogonalizedObject = orthogonalizedObjects[i];
		vector<Vec3i> coordinates;

		if ( orthogonalizedObject.size() == 9 )
		{
			Vec3i topMiddle;
			Vec3i bottomMiddle;

			if ( orthogonalizedObject[4][3] > orthogonalizedObject[4][1] )
			{
				topMiddle[0] = orthogonalizedObject[4][0];
				topMiddle[1] = orthogonalizedObject[4][1];
				topMiddle[2] = 0;
				bottomMiddle[0] = orthogonalizedObject[4][2];
				bottomMiddle[1] = orthogonalizedObject[4][3];
				bottomMiddle[2] = round( abs( orthogonalizedObject[4][3] - orthogonalizedObject[4][1] ) * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
			}
			else
			{
				topMiddle[0] = orthogonalizedObject[4][2];
				topMiddle[1] = orthogonalizedObject[4][3];
				topMiddle[2] = 0;
				bottomMiddle[0] = orthogonalizedObject[4][0];
				bottomMiddle[1] = orthogonalizedObject[4][1];
				bottomMiddle[2] = round( abs( orthogonalizedObject[4][3] - orthogonalizedObject[4][1] ) * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
			}

			coordinates.push_back( topMiddle );
			coordinates.push_back( bottomMiddle );

			Vec3i topLeft;
			Vec3i bottomLeft;

			if ( orthogonalizedObject[0][3] > orthogonalizedObject[0][1] )
			{
				topLeft[0] = orthogonalizedObject[0][0];
				topLeft[1] = orthogonalizedObject[0][1];
				topLeft[2] = round( abs( min( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[0][1] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
				bottomLeft[0] = orthogonalizedObject[0][2];
				bottomLeft[1] = orthogonalizedObject[0][3];
				bottomLeft[2] = round( abs( max( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[0][3] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) + abs( orthogonalizedObject[4][3] - orthogonalizedObject[4][1] ) * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
			}
			else
			{
				topLeft[0] = orthogonalizedObject[0][2];
				topLeft[1] = orthogonalizedObject[0][3];
				topLeft[2] = round( abs( min( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[0][3] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
				bottomLeft[0] = orthogonalizedObject[0][0];
				bottomLeft[1] = orthogonalizedObject[0][1];
				bottomLeft[2] = round( abs( max( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[0][1] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) + abs( orthogonalizedObject[4][3] - orthogonalizedObject[4][1] ) * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
			}

			coordinates.push_back( topLeft );
			coordinates.push_back( bottomLeft );

			Vec3i topRight;
			Vec3i bottomRight;

			if ( orthogonalizedObject[8][3] > orthogonalizedObject[8][1] )
			{
				topRight[0] = orthogonalizedObject[8][0];
				topRight[1] = orthogonalizedObject[8][1];
				topRight[2] = round( abs( min( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[8][1] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
				bottomRight[0] = orthogonalizedObject[8][2];
				bottomRight[1] = orthogonalizedObject[8][3];
				bottomRight[2] = round( abs( max( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[8][3] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) + abs( orthogonalizedObject[4][3] - orthogonalizedObject[4][1] ) * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
			}
			else
			{
				topRight[0] = orthogonalizedObject[8][2];
				topRight[1] = orthogonalizedObject[8][3];
				topRight[2] = round( abs( min( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[8][3] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
				bottomRight[0] = orthogonalizedObject[8][0];
				bottomRight[1] = orthogonalizedObject[8][1];
				bottomRight[2] = round( abs( max( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[8][1] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) + abs( orthogonalizedObject[4][3] - orthogonalizedObject[4][1] ) * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
			}

			coordinates.push_back( topRight );
			coordinates.push_back( bottomRight );

			Vec3i top;

			top[0] = orthogonalizedObject[1][2];
			top[1] = orthogonalizedObject[1][3];
			top[2] = round( abs( min( orthogonalizedObject[4][1], orthogonalizedObject[4][3] ) - orthogonalizedObject[1][3] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );

			coordinates.push_back( top );

			if ( coordinates[1][1] > lowestPoint )
			{
				frontObjectIndex = i;
				lowestPoint = coordinates[1][1];
			}

			objects.push_back( coordinates );
		}
	}

	for ( size_t i = 0; i < objects.size(); i++ )
	{
		if ( i == frontObjectIndex )
		{
			continue;
		}

		int z_dif = round( ( lowestPoint - objects[i][1][1] ) / tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );

		vector<Vec3i> object = objects[i];

		for ( size_t j = 0; j < object.size(); j++ )
		{
			object[j][2] = object[j][2] + z_dif;
		}

		objects[i] = object;
	}

	return objects;
}
