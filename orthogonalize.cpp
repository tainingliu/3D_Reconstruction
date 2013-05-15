/*
 * orthogonalize.cpp
 *
 *  Created on: 2012-10-21
 *      Author: Tai
 */

#include "orthogonalize.h"
#include "edgemap.h"
#include "math.h"
#include <vector>

extern int imgHeight;
extern int imgWidth;

double APERTURE_WIDTH = 32;
int FOCAL_LENGTH = 35;
double ANGLE_OF_INCLINATION = 30;
double ANGLE_ERROR = 1.2;

struct recPrism
{
	vector<Vec4i> sideEdges;
	vector<Vec2i> corners;
};

Vec4i orthogonalizeEdge( Vec4i sideEdge )
{
	int yMiddle = imgHeight / 2;

	int xMiddle = imgWidth / 2;

	int y_dif = 0;

	int x_dif = 0;

	double z_dif = 0;

	if ( sideEdge[3] >= sideEdge[1] )
	{
		// difference in x / slope = y

		// z difference equals diff in x difference divided by difference from top point to y middle
		z_dif = abs( sideEdge[2] - sideEdge[0] ) * imgWidth *  FOCAL_LENGTH / double( abs( sideEdge[0] - xMiddle ) * APERTURE_WIDTH );

		x_dif = round( z_dif * abs( sideEdge[0] - xMiddle ) / double( imgWidth ) * ( APERTURE_WIDTH / FOCAL_LENGTH ) );

		y_dif = round( z_dif * ( sideEdge[3] - yMiddle ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif ) );


//					x_dif = z_dif * abs( sideEdge[0] - xMiddle ) / double( imgWidth ) * ( APERTURE_WIDTH / FOCAL_LENGTH );
//
//					y_dif = z_dif * abs( sideEdge[1] - sideEdge[3] ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif );

		//sideEdge[3] = sideEdge[3] + ( y_dif * abs( sideEdge[3] - yMiddle ) / double( height ) );
		//sideEdge[1] = sideEdge[1] - ( y_dif * abs( sideEdge[1] - yMiddle ) / double( height ) );
		sideEdge[3] = sideEdge[3] + y_dif;
		sideEdge[2] = sideEdge[0];
	}
	else
	{
		// difference in x / slope = y
		z_dif = abs( sideEdge[2] - sideEdge[0] ) * imgWidth *  FOCAL_LENGTH / double( abs( sideEdge[2] - xMiddle ) * APERTURE_WIDTH );

		x_dif = round( z_dif * abs( sideEdge[2] - xMiddle ) / double( imgWidth ) * ( APERTURE_WIDTH / FOCAL_LENGTH ) );

		y_dif = round( z_dif * ( sideEdge[1] - yMiddle ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif ) );

//					x_dif = z_dif * abs( sideEdge[2] - xMiddle ) / double( imgWidth ) * ( APERTURE_WIDTH / FOCAL_LENGTH );
//
//					y_dif = z_dif * abs( sideEdge[1] - sideEdge[3] ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif );

		//sideEdge[3] = sideEdge[3] - ( y_dif * abs( sideEdge[3] - yMiddle ) / double( height ) );
		//sideEdge[1] = sideEdge[1] + ( y_dif * abs( sideEdge[1] - yMiddle ) / double( height ) );
		sideEdge[1] = sideEdge[1] + y_dif;
		sideEdge[0] = sideEdge[2];
	}

//	cout << "Edge shape change: z, y, x: " << z_dif << " " << y_dif << " " << x_dif << endl;

	return sideEdge;
}

Vec4i orthogonalizePosition( Vec4i sideEdge, int x_dif, int y_dif )
{
	int yMiddle = imgHeight / 2;

	int xMiddle = imgWidth / 2;

	if ( ( sideEdge[3] >= yMiddle ) && ( sideEdge[1] >= yMiddle ) )
	{
		if ( sideEdge[3] >= sideEdge[1] )
		{
			sideEdge[3] = sideEdge[3] + y_dif;
		}
		else
		{
			sideEdge[1] = sideEdge[1] + y_dif;
		}
	}
	else if ( sideEdge[3] <= yMiddle && sideEdge[1] <= yMiddle )
	{
		if ( sideEdge[3] >= sideEdge[1] )
		{
			sideEdge[1] = sideEdge[1] - y_dif;
		}
		else
		{
			sideEdge[3] = sideEdge[3] - y_dif;
		}
	}
	else
	{
		if ( sideEdge[3] >= sideEdge[1] )
		{
			sideEdge[3] = sideEdge[3] + ( y_dif * abs( sideEdge[3] - yMiddle ) / double( abs( sideEdge[3] - sideEdge[1] ) ) );
			sideEdge[1] = sideEdge[1] - ( y_dif * abs( sideEdge[1] - yMiddle ) / double( abs( sideEdge[3] - sideEdge[1] ) ) );
		}
		else
		{
			sideEdge[3] = sideEdge[3] - ( y_dif * abs( sideEdge[3] - yMiddle ) / double( abs( sideEdge[3] - sideEdge[1] ) ) );
			sideEdge[1] = sideEdge[1] + ( y_dif * abs( sideEdge[1] - yMiddle ) / double( abs( sideEdge[3] - sideEdge[1] ) ) );
		}
	}

	if ( ( sideEdge[0] > xMiddle ) && ( sideEdge[2] > xMiddle ) )
	{
		sideEdge[0] = sideEdge[0] + x_dif;
		sideEdge[2] = sideEdge[2] + x_dif;
	}
	else if ( ( sideEdge[0] <= xMiddle ) && ( sideEdge[2] <= xMiddle ) )
	{
		sideEdge[2] = sideEdge[2] - x_dif;
		sideEdge[0] = sideEdge[0] - x_dif;
	}
	else
	{
		sideEdge[2] = sideEdge[2] + ( x_dif * abs( sideEdge[2] - xMiddle ) / double( abs( sideEdge[2] - sideEdge[0] ) ) );
		sideEdge[0] = sideEdge[0] - ( x_dif * abs( sideEdge[0] - xMiddle ) / double( abs( sideEdge[2] - sideEdge[0] ) ) );
	}

	return sideEdge;
}

//	Written by Taining Liu
//		Oct 20, 2012
//	Takes array of edges that form edgemap
//	Converts edges to orthogonal view
vector<vector<Vec4i> > orthogonalize( vector<Vec4i> edgeMap )
{
	vector<Vec4i> sideEdges = edgeMap;

	vector<recPrism> objects;

	int frontObjectIndex = 0;
	int lowestPoint = 0;

	//	Loop that removes all edges that are not very close to vertical
	//	The remaining lines will correspond to lines that are vertical
	for( size_t i = 0; i < sideEdges.size(); i++ )
	{
		Scalar slope = calcSlope( sideEdges[i] );

		//	If lines have slope between 0 -> |1| remove them
		if ( ( slope[0] != 0 ) || ( slope[3] != 0 ) )
		{
			sideEdges.erase( sideEdges.begin() + (int)i );
			i--;
		}
	}

	for( size_t i = 0; i < sideEdges.size(); i++ )
	{
		Scalar slope = calcSlope( sideEdges[i] );

		// If there exists a line that connects to this edge
		// and has a slope greater than this, eliminate this edge
		// else eliminate the other edge unless the edge is above
		// this edge.
		for( size_t j = 0; j < sideEdges.size(); j++ )
		{
			if ( i != j )
			{
				Scalar slopeCompare = calcSlope( sideEdges[j] );

				//	Eliminates edges with a lesser slope
				if ( ( ( sideEdges[i][0] == sideEdges[j][0] ) && ( sideEdges[i][1] == sideEdges[j][1] ) )
					|| ( ( sideEdges[i][0] == sideEdges[j][2] ) && ( sideEdges[i][1] == sideEdges[j][3] ) )
					|| ( ( sideEdges[i][2] == sideEdges[j][0] ) && ( sideEdges[i][3] == sideEdges[j][1] ) )
					|| ( ( sideEdges[i][2] == sideEdges[j][2] ) && ( sideEdges[i][3] == sideEdges[j][3] ) ) )
				{
					if ( ( abs( slope[1] ) >= abs( slopeCompare[1] ) )
						&& ( ( ( sideEdges[j][1] >= sideEdges[i][1] ) && ( sideEdges[j][1] >= sideEdges[i][3] ) ) || ( ( sideEdges[j][3] >= sideEdges[i][1] ) && ( sideEdges[j][3] >= sideEdges[i][3] ) ) ) )
					{
						sideEdges.erase( sideEdges.begin() + (int)i );
						i--;
						break;
					}
					else
					{
						sideEdges.erase( sideEdges.begin() + (int)j );
						i--;
						j--;
						break;
					}
				}
			}
		}
	}

	// Checks if side edges have edges that connect to both
	// ends of the side edge.
	// If side edge only has one connected means that it is
	// cut off by another edge and needs to be connected.
	for ( size_t i = 0; i < sideEdges.size(); i++ )
	{
		bool oneConnected = false;
		bool twoConnected = false;
		int connectedIndex = -1;


		// Loop that checks if there is an edge from the edge map
		// that connects either end of the side edge.
		// Raises a flag for each end connected and records index of
		// lowest edge.
		for ( size_t j = 0; j < edgeMap.size(); j++ )
		{
			if ( sideEdges[i] != edgeMap[j] )
			{
				if ( ( edgeMap[j][0] == sideEdges[i][0] ) && ( edgeMap[j][1] == sideEdges[i][1] ) )
				{
					oneConnected = true;
					if ( connectedIndex < 0 )
					{
						connectedIndex = j;
					}
					else if ( edgeMap[connectedIndex][3] < edgeMap[j][3] )
					{
						connectedIndex = j;
					}
				}
				if ( ( edgeMap[j][2] == sideEdges[i][0] ) && ( edgeMap[j][3] == sideEdges[i][1] ) )
				{
					oneConnected = true;
					if ( connectedIndex < 0 )
					{
						connectedIndex = j;
					}
					else if ( edgeMap[connectedIndex][1] < edgeMap[j][1] )
					{
						connectedIndex = j;
					}
				}

				if ( ( edgeMap[j][0] == sideEdges[i][2] ) && ( edgeMap[j][1] == sideEdges[i][3] ) )
				{
					twoConnected = true;
					if ( connectedIndex < 0 )
					{
						connectedIndex = j;
					}
					else if ( edgeMap[connectedIndex][3] < edgeMap[j][3] )
					{
						connectedIndex = j;
					}
				}
				if ( ( edgeMap[j][2] == sideEdges[i][2] ) && ( edgeMap[j][3] == sideEdges[i][3] ) )
				{
					twoConnected = true;
					if ( connectedIndex < 0 )
					{
						connectedIndex = j;
					}
					else if ( edgeMap[connectedIndex][1] < edgeMap[j][1] )
					{
						connectedIndex = j;
					}
				}
			}
		}

		// If only one end is connected need to find edge
		// from edge map that connects to another side edge
		if ( oneConnected ^ twoConnected )
		{
			//cout << "Connected edge is: " << edgeMap[connectedIndex][0] << " " << edgeMap[connectedIndex][1] << " " << edgeMap[connectedIndex][2] << " " << edgeMap[connectedIndex][3] << " " << endl;

			Vec2i endCoord;

			int sideEdgeIndex = -1;

			for ( size_t j = 0; j < sideEdges.size(); j++ )
			{
				if ( ( sideEdges[j] != edgeMap[connectedIndex] ) && ( sideEdges[j] != sideEdges[i] ) )
				{
					if ( ( edgeMap[connectedIndex][0] == sideEdges[j][0] ) && ( edgeMap[connectedIndex][1] == sideEdges[j][1] ) )
					{
						endCoord[0] = sideEdges[j][2];
						endCoord[1] = sideEdges[j][3];
						sideEdgeIndex = j;
					}
					if ( ( edgeMap[connectedIndex][2] == sideEdges[j][0] ) && ( edgeMap[connectedIndex][3] == sideEdges[j][1] ) )
					{
						endCoord[0] = sideEdges[j][2];
						endCoord[1] = sideEdges[j][3];
						sideEdgeIndex = j;
					}

					if ( ( edgeMap[connectedIndex][0] == sideEdges[j][2] ) && ( edgeMap[connectedIndex][1] == sideEdges[j][3] ) )
					{
						endCoord[0] = sideEdges[j][0];
						endCoord[1] = sideEdges[j][1];
						sideEdgeIndex = j;
					}
					if ( ( edgeMap[connectedIndex][2] == sideEdges[j][2] ) && ( edgeMap[connectedIndex][3] == sideEdges[j][3] ) )
					{
						endCoord[0] = sideEdges[j][0];
						endCoord[1] = sideEdges[j][1];
						sideEdgeIndex = j;
					}
				}
			}

			//cout << "End coord of connected edge is: " << endCoord[0] << " " << endCoord[1] << endl;

			bool bothConnected = false;

			Vec4i temp;

			// If such an edge is found, try to find edge that is in
			// same directions as the edge that connects the two side
			// edges then find the intersection between that edge and
			// the cut off side edge
			for ( size_t j = 0; j < edgeMap.size(); j++ )
			{
				if ( ( ( ( edgeMap[j][0] == endCoord[0] ) && ( edgeMap[j][1] == endCoord[1] ) ) || ( ( edgeMap[j][2] == endCoord[0] ) && ( edgeMap[j][3] == endCoord[1] ) ) )
					  && ( edgeMap[j] != sideEdges[sideEdgeIndex] ) )
				{
					bothConnected = true;
					if ( ( ( sideEdges[i][0] >= endCoord[0] ) && ( edgeMap[j][0] == endCoord[0] ) ) || ( ( sideEdges[i][0] < endCoord[0] ) && ( edgeMap[j][2] == endCoord[0] ) ) )
					{
						//cout << "Cut-off edge is: " << edgeMap[j][0] << " " << edgeMap[j][1] << " " << edgeMap[j][2] << " " << edgeMap[j][3] << " " << endl;

						int x;
						int y;

						lineIntersect( sideEdges[i], edgeMap[j], x, y );

						//cout << x << " " << y << endl;

						if ( oneConnected )
						{
							if ( x > sideEdges[i][0] )
							{
								temp[0] = sideEdges[i][0];
								temp[1] = sideEdges[i][1];
								temp[2] = x;
								temp[3] = y;
							}
							else
							{
								temp[0] = x;
								temp[1] = y;
								temp[2] = sideEdges[i][0];
								temp[3] = sideEdges[i][1];
							}
						}

						if ( twoConnected )
						{
							if ( x > sideEdges[i][2] )
							{
								temp[0] = sideEdges[i][2];
								temp[1] = sideEdges[i][3];
								temp[2] = x;
								temp[3] = y;
							}
							else
							{
								temp[0] = x;
								temp[1] = y;
								temp[2] = sideEdges[i][2];
								temp[3] = sideEdges[i][3];
							}
						}
					}
				}
			}

			sideEdges.erase( sideEdges.begin() + (int)i );
			i--;

			if ( bothConnected )
			{
				//cout << "New side edge is: " << temp[0] << " " << temp[1] << " " << temp[2] << " " << temp[3] << " " << endl;
				sideEdges.push_back( temp );
				i++;
			}
		}
		else if ( !oneConnected && !twoConnected )
		{
			sideEdges.erase( sideEdges.begin() + (int)i );
			i--;
		}
	}

	//	Compares side edges to remaining edges in edge map
	//	It is a valid side edge if at least one edge is
	//	connected to each end of a side edge.
	//	One edge from the end of a side edge and one edge
	//	from the other end of a side edge should connect to
	//	another side edge. If they don't take the lower to
	//	edges connected and from a new side edge.
	for ( size_t i = 0; i < sideEdges.size(); i++ )
	{
		vector<Vec2i> connectedEdges;

// To implement corner addition add bool var to check
// if edges end up on both sides of edge. If so remove
// current edge and add to back so that corner is added
// on object creation.

		// Loop that compares current side edge against edge map
		// Records the end point opposite the one connected to
		// the side edge.
		for ( size_t j = 0; j < edgeMap.size(); j++ )
		{
			if ( sideEdges[i] != edgeMap[j] )
			{
				if ( ( edgeMap[j][0] == sideEdges[i][0] ) && ( edgeMap[j][1] == sideEdges[i][1] ) )
				{
					Vec2i oneEnd;
					oneEnd[0] = edgeMap[j][2];
					oneEnd[1] = edgeMap[j][3];
					connectedEdges.push_back( oneEnd );
				}
				if ( ( edgeMap[j][2] == sideEdges[i][0] ) && ( edgeMap[j][3] == sideEdges[i][1] ) )
				{
					Vec2i oneEnd;
					oneEnd[0] = edgeMap[j][0];
					oneEnd[1] = edgeMap[j][1];
					connectedEdges.push_back( oneEnd );
				}

				if ( ( edgeMap[j][0] == sideEdges[i][2] ) && ( edgeMap[j][1] == sideEdges[i][3] ) )
				{
					Vec2i twoEnd;
					twoEnd[0] = edgeMap[j][2];
					twoEnd[1] = edgeMap[j][3];
					connectedEdges.push_back( twoEnd );
				}
				if ( ( edgeMap[j][2] == sideEdges[i][2] ) && ( edgeMap[j][3] == sideEdges[i][3] ) )
				{
					Vec2i twoEnd;
					twoEnd[0] = edgeMap[j][0];
					twoEnd[1] = edgeMap[j][1];
					connectedEdges.push_back( twoEnd );
				}
			}
		}

		// Determine if all side connect on left or right side of
		// side edge.
		bool connectedOnLeft = false;
		bool connectedOnRight = false;

		for ( size_t j = 0; j < connectedEdges.size(); j++ )
		{
			if ( ( connectedEdges[j][0] > sideEdges[i][0] ) && ( connectedEdges[j][0] > sideEdges[i][2] ) )
				connectedOnRight = true;
			if ( ( connectedEdges[j][0] < sideEdges[i][0] ) && ( connectedEdges[j][0] < sideEdges[i][2] ) )
				connectedOnLeft = true;
		}

		bool exists_flag = false;

		// Search if any connected edge connects to another side
		// edge that exists in an object.
		// If so, add side edge to object.
		for ( size_t j = 0; j < objects.size(); j++ )
		{
			for ( size_t k = 0; k < objects[j].sideEdges.size(); k++ )
			{
				for ( size_t m = 0; m < connectedEdges.size(); m++ )
				{
					if ( ( ( connectedEdges[m][0] == objects[j].sideEdges[k][0] ) && ( connectedEdges[m][1] == objects[j].sideEdges[k][1] ) )
					  || ( ( connectedEdges[m][0] == objects[j].sideEdges[k][2] ) && ( connectedEdges[m][1] == objects[j].sideEdges[k][3] ) ) )
					{
						connectedEdges.erase( connectedEdges.begin() + (int)m );
						m--;
						exists_flag = true;
					}
				}
			}
			if ( exists_flag )
			{
				objects[j].sideEdges.push_back( sideEdges[i] );
				sideEdges.erase( sideEdges.begin() + (int)i );
				i--;
				break;
			}
		}

		// If connecting edges do not connect to another side
		// edge already in an object, create a new object and
		// add in this side edge and any other side edge that
		// are joined by connecting edges.
		if ( !exists_flag )
		{
			recPrism newObject;

			// Add this edge to new object.
			newObject.sideEdges.push_back( sideEdges[i] );

			sideEdges.erase( sideEdges.begin() + (int)i );
			i--;

			// Check for connecting side edges and add them to newly
			// created object.
			for ( size_t k = 0; k < sideEdges.size(); k++ )
			{
				bool connectedFlag = false;

				for ( size_t m = 0; m < connectedEdges.size(); m++ )
				{
					if ( ( ( connectedEdges[m][0] == sideEdges[k][0] ) && ( connectedEdges[m][1] == sideEdges[k][1] ) )
					  || ( ( connectedEdges[m][0] == sideEdges[k][2] ) && ( connectedEdges[m][1] == sideEdges[k][3] ) ) )
					{
						connectedEdges.erase( connectedEdges.begin() + (int)m );
						m--;

						connectedFlag = true;
						exists_flag = true;
					}
				}

				if ( connectedFlag )
				{
					newObject.sideEdges.push_back( sideEdges[k] );
					sideEdges.erase( sideEdges.begin() + (int)k );
					k--;
				}
			}

			// If any connected edges cannot connect to another
			// side edge then create a new side edge made of the
			// lower to end connecting edges.
			if ( ( !connectedOnLeft ^ !connectedOnRight ) && !exists_flag )
			{
				if ( connectedEdges.size() == 3 )
				{
					int top = -1;
					int middle = -1;
					int bottom = -1;

					int lowTemp = imgHeight;
					int highTemp = 0;

					for ( size_t j = 0; j < connectedEdges.size(); j++ )
					{
						if ( connectedEdges[j][1] > highTemp )
						{
							highTemp = connectedEdges[j][1];
							bottom = j;
						}
						if ( connectedEdges[j][1] < lowTemp )
						{
							lowTemp = connectedEdges[j][1];
							top = j;
						}
					}

					if ( top + bottom == 1 )
					{
						middle = 2;
					}

					if ( top + bottom == 2 )
					{
						middle = 1;
					}

					if ( top + bottom == 3 )
					{
						middle = 0;
					}

					Vec2i middleEdge = connectedEdges[middle];
					Vec2i bottomEdge = connectedEdges[bottom];

					connectedEdges.clear();

//					newObject.corners.push_back( connectedEdges[top] );

					connectedEdges.push_back( middleEdge );
					connectedEdges.push_back( bottomEdge );
				}

				if ( connectedEdges.size() == 2 )
				{
					//cout << "Center edge:" << connectedEdges[0][0] << " " << connectedEdges[0][1] << " " << connectedEdges[1][0] << " " << connectedEdges[1][1] << endl;
					//cout << newObject.sideEdges[0][1] << " " << newObject.sideEdges[0][3] << endl;
					if ( ( ( connectedEdges[0][1] >= newObject.sideEdges[0][1] ) && ( connectedEdges[1][1] >= newObject.sideEdges[0][3] ) )
					  || ( ( connectedEdges[0][1] >= newObject.sideEdges[0][3] ) && ( connectedEdges[1][1] >= newObject.sideEdges[0][1] ) ) )
					{
						Vec4i temp;

						if ( connectedEdges[1][0] >= connectedEdges[0][0] )
						{
							temp[0] = connectedEdges[0][0];
							temp[1] = connectedEdges[0][1];
							temp[2] = connectedEdges[1][0];
							temp[3] = connectedEdges[1][1];
						}
						else
						{
							temp[0] = connectedEdges[1][0];
							temp[1] = connectedEdges[1][1];
							temp[2] = connectedEdges[0][0];
							temp[3] = connectedEdges[0][1];
						}

						newObject.sideEdges.push_back( temp );
					}
					if ( ( ( connectedEdges[0][1] < newObject.sideEdges[0][1] ) && ( connectedEdges[1][1] < newObject.sideEdges[0][3] ) )
					  || ( ( connectedEdges[0][1] < newObject.sideEdges[0][3] ) && ( connectedEdges[1][1] < newObject.sideEdges[0][1] ) ) )
					{
						Vec4i temp;

						if ( connectedEdges[1][0] >= connectedEdges[0][0] )
						{
							temp[0] = connectedEdges[0][0];
							temp[1] = connectedEdges[0][1];
							temp[2] = connectedEdges[1][0];
							temp[3] = connectedEdges[1][1];
						}
						else
						{
							temp[0] = connectedEdges[1][0];
							temp[1] = connectedEdges[1][1];
							temp[2] = connectedEdges[0][0];
							temp[3] = connectedEdges[0][1];
						}

						newObject.sideEdges.push_back( temp );
					}

					connectedEdges.clear();
				}
			}

			// If the new object only has two sides
			// find the corners above the two sides
			// if one corner is obstructed, derive the
			// corner.
			if ( newObject.sideEdges.size() == 2 )
			{
				// Reorganize to make sure left side edge is first
				// right is second
				int leftConnectIndex = -1;
				int rightConnectIndex = -1;

				if ( newObject.sideEdges[0][0] < newObject.sideEdges[1][0] )
				{
					leftConnectIndex = 0;
					rightConnectIndex = 1;
				}
				else
				{
					leftConnectIndex = 1;
					rightConnectIndex = 0;
				}

				Vec4i leftTemp = newObject.sideEdges[leftConnectIndex];
				Vec4i rightTemp = newObject.sideEdges[rightConnectIndex];

				newObject.sideEdges.clear();

				newObject.sideEdges.push_back( leftTemp );
				newObject.sideEdges.push_back( rightTemp );

				//End reorganize

				leftConnectIndex = -1;
				rightConnectIndex = -1;
				bool leftConnected = false;
				bool rightConnected = false;
				int centerConnectIndex = -1;

				// Find connected edges to side that produce top face corners
				for ( size_t j = 0; j < edgeMap.size(); j++ )
				{
					if ( ( newObject.sideEdges[0] != edgeMap[j] ) && ( newObject.sideEdges[1] != edgeMap[j] ) )
					{
						if ( newObject.sideEdges[0][1] <  newObject.sideEdges[0][3] )
						{
							if ( ( edgeMap[j][0] == newObject.sideEdges[0][0] ) && ( edgeMap[j][1] == newObject.sideEdges[0][1] ) )
							{
								leftConnectIndex = j;
							}
							if ( ( edgeMap[j][2] == newObject.sideEdges[0][0] ) && ( edgeMap[j][3] == newObject.sideEdges[0][1] ) )
							{
								leftConnectIndex = j;
							}
						}
						else
						{
							if ( ( edgeMap[j][0] == newObject.sideEdges[0][2] ) && ( edgeMap[j][1] == newObject.sideEdges[0][3] ) )
							{
								leftConnectIndex = j;
							}
							if ( ( edgeMap[j][2] == newObject.sideEdges[0][2] ) && ( edgeMap[j][3] == newObject.sideEdges[0][3] ) )
							{
								leftConnectIndex = j;
							}
						}

						if ( newObject.sideEdges[1][1] <  newObject.sideEdges[1][3] )
						{
							if ( ( edgeMap[j][0] == newObject.sideEdges[1][0] ) && ( edgeMap[j][1] == newObject.sideEdges[1][1] ) )
							{
								rightConnectIndex = j;
							}
							if ( ( edgeMap[j][2] == newObject.sideEdges[1][0] ) && ( edgeMap[j][3] == newObject.sideEdges[1][1] ) )
							{
								rightConnectIndex = j;
							}
						}
						else
						{
							if ( ( edgeMap[j][0] == newObject.sideEdges[1][2] ) && ( edgeMap[j][1] == newObject.sideEdges[1][3] ) )
							{
								rightConnectIndex = j;
							}
							if ( ( edgeMap[j][2] == newObject.sideEdges[1][2] ) && ( edgeMap[j][3] == newObject.sideEdges[1][3] ) )
							{
								rightConnectIndex = j;
							}
						}
					}
				}

				// Check if there is an edge that connects the found edges that form the top face corners
				// if there is no, one of the corners is obstructed
				for ( size_t j = 0; j < edgeMap.size(); j++ )
				{
					if ( ( newObject.sideEdges[0] != edgeMap[j] ) && ( newObject.sideEdges[1] != edgeMap[j] ) && ( j != leftConnectIndex ) && ( j != rightConnectIndex ) )
					{
						if ( ( edgeMap[j][0] == edgeMap[leftConnectIndex][0] ) && ( edgeMap[j][1] == edgeMap[leftConnectIndex][1] ) )
						{
							leftConnected = true;
							centerConnectIndex = j;
						}
						if ( ( edgeMap[j][2] == edgeMap[leftConnectIndex][0] ) && ( edgeMap[j][3] == edgeMap[leftConnectIndex][1] ) )
						{
							leftConnected = true;
							centerConnectIndex = j;
						}
						if ( ( edgeMap[j][0] == edgeMap[leftConnectIndex][2] ) && ( edgeMap[j][1] == edgeMap[leftConnectIndex][3] ) )
						{
							leftConnected = true;
							centerConnectIndex = j;
						}
						if ( ( edgeMap[j][2] == edgeMap[leftConnectIndex][2] ) && ( edgeMap[j][3] == edgeMap[leftConnectIndex][3] ) )
						{
							leftConnected = true;
							centerConnectIndex = j;
						}

						if ( ( edgeMap[j][0] == edgeMap[rightConnectIndex][0] ) && ( edgeMap[j][1] == edgeMap[rightConnectIndex][1] ) )
						{
							rightConnected = true;
							centerConnectIndex = j;
						}
						if ( ( edgeMap[j][2] == edgeMap[rightConnectIndex][0] ) && ( edgeMap[j][3] == edgeMap[rightConnectIndex][1] ) )
						{
							rightConnected = true;
							centerConnectIndex = j;
						}
						if ( ( edgeMap[j][0] == edgeMap[rightConnectIndex][2] ) && ( edgeMap[j][1] == edgeMap[rightConnectIndex][3] ) )
						{
							rightConnected = true;
							centerConnectIndex = j;
						}
						if ( ( edgeMap[j][2] == edgeMap[rightConnectIndex][2] ) && ( edgeMap[j][3] == edgeMap[rightConnectIndex][3] ) )
						{
							rightConnected = true;
							centerConnectIndex = j;
						}
					}
				}

				if ( ( rightConnectIndex >= 0 ) && ( leftConnectIndex >= 0 ) )
				{
					// If corner is obstructed, reconstruct from top face edge
					// and edge from side edge that forms corner
					if ( ( leftConnected ^ rightConnected ) )
					{
						if ( leftConnected )
						{
							int x = 0;
							int y = 0;
							lineIntersect( edgeMap[rightConnectIndex], edgeMap[centerConnectIndex], x, y );

							Vec2i rightCorner;
							rightCorner[0] = x;
							rightCorner[1] = y;

							Vec2i leftCorner;
							leftCorner[0] = edgeMap[centerConnectIndex][0];
							leftCorner[1] = edgeMap[centerConnectIndex][1];

							newObject.corners.push_back( leftCorner );
							newObject.corners.push_back( rightCorner );
						}

						if ( rightConnected )
						{
							int x = 0;
							int y = 0;
							lineIntersect( edgeMap[leftConnectIndex], edgeMap[centerConnectIndex], x, y );

							Vec2i leftCorner;
							leftCorner[0] = x;
							leftCorner[1] = y;

							Vec2i rightCorner;
							rightCorner[0] = edgeMap[centerConnectIndex][2];
							rightCorner[1] = edgeMap[centerConnectIndex][3];

							newObject.corners.push_back( leftCorner );
							newObject.corners.push_back( rightCorner );
						}
					}
					// Else take corners of edge that form top face
					else
					{
						if ( centerConnectIndex > 0 )
						{
							Vec2i leftCorner;
							leftCorner[0] = edgeMap[centerConnectIndex][0];
							leftCorner[1] = edgeMap[centerConnectIndex][1];

							Vec2i rightCorner;
							rightCorner[0] = edgeMap[centerConnectIndex][2];
							rightCorner[1] = edgeMap[centerConnectIndex][3];

							newObject.corners.push_back( leftCorner );
							newObject.corners.push_back( rightCorner );
						}
						else
						{
							newObject.sideEdges.clear();
						}
					}
				}
				else
				{
					newObject.sideEdges.clear();
				}
//				cout << "Left connect edge is: " << edgeMap[leftConnectIndex][0] << " " << edgeMap[leftConnectIndex][1] << " " << edgeMap[leftConnectIndex][2] << " " << edgeMap[leftConnectIndex][3] << " " << endl;
//				cout << "Right connect is: " << edgeMap[rightConnectIndex][0] << " " << edgeMap[rightConnectIndex][1] << " " << edgeMap[rightConnectIndex][2] << " " << edgeMap[rightConnectIndex][3] << " " << endl;
//				cout << "Left corner is: " << newObject.corners[0][0] << " " << newObject.corners[0][1] << endl;
//				cout << "Right corner is: " << newObject.corners[1][0] << " " << newObject.corners[1][1] << endl;
			}

//			if ( connectedEdges.size() == 1 )
//			{
//				newObject.corners.push_back( connectedEdges[0] );
//			}

			// Add new object to object list.
			objects.push_back( newObject );
		}
	}

	//	Go through every object and orthogonalize them
	for ( size_t i = 0; i < objects.size(); i++ )
	{
		int left = -1;
		int right = -1;
		int center = -1;

		int lowTemp = imgWidth;
		int highTemp = 0;

		// Reorganize side in the object
		for ( size_t j = 0; j < objects[i].sideEdges.size(); j++ )
		{
			if ( objects[i].sideEdges[j][0] > highTemp )
			{
				highTemp = objects[i].sideEdges[j][0];
				right = j;
			}
			if ( objects[i].sideEdges[j][0] < lowTemp )
			{
				lowTemp = objects[i].sideEdges[j][0];
				left = j;
			}
		}

		// Orthogonalize when three side edges visible
		if ( objects[i].sideEdges.size() == 3 )
		{
			if ( left + right == 1 )
			{
				center = 2;
			}

			if ( left + right == 2 )
			{
				center = 1;
			}

			if ( left + right == 3 )
			{
				center = 0;
			}

			Vec4i leftEdge = objects[i].sideEdges[left];
			Vec4i rightEdge = objects[i].sideEdges[right];
			Vec4i centerEdge = objects[i].sideEdges[center];

			objects[i].sideEdges.clear();

			objects[i].sideEdges.push_back( leftEdge );
			objects[i].sideEdges.push_back( centerEdge );
			objects[i].sideEdges.push_back( rightEdge );
		// Reorganization ends here

			int xMiddle = imgWidth / 2;

			// Orthogonalize edges in place
			for ( size_t j = 0; j < objects[i].sideEdges.size(); j++ )
			{
				//int height = abs( objects[i].sideEdges[j][3] - objects[i].sideEdges[j][1] );

				objects[i].sideEdges[j] = orthogonalizeEdge( objects[i].sideEdges[j] );
			}

			// Orthogonalize position of edges
			for ( size_t j = 0; j < objects[i].sideEdges.size(); j++ )
			{
				int centerHeight = abs( objects[i].sideEdges[1][3] - objects[i].sideEdges[1][1] );
				int compareHeight = abs( objects[i].sideEdges[j][3] - objects[i].sideEdges[j][1] );

				//cout << centerHeight << " " << compareHeight << endl;

				double z_dif = max( ( centerHeight - compareHeight ), 0 ) * imgWidth * FOCAL_LENGTH / double( centerHeight * APERTURE_WIDTH );
				int y_dif = max( ( centerHeight - compareHeight ), 0 ); //round( z_dif * centerHeight / double( imgWidth ) * ( APERTURE_WIDTH / FOCAL_LENGTH ) );

				int x_dif = round( z_dif * abs( objects[i].sideEdges[j][0] - xMiddle ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif ) );

//				cout << "Edge position change: z, x, y: " << z_dif << " " << x_dif << " " << y_dif << endl;

				objects[i].sideEdges[j] = orthogonalizePosition( objects[i].sideEdges[j], x_dif, y_dif );

				j++;
			}
		}

		// Orthogonalize when only two side edges visible
		if ( objects[i].sideEdges.size() == 2 )
		{
			if ( min( objects[i].sideEdges[0][1], objects[i].sideEdges[0][3] ) >= min( objects[i].sideEdges[0][1], objects[i].sideEdges[0][3] ) )
			{
				center = 0;
				left = 1;
			}
			else
			{
				center = 1;
				left = 0;
			}

			// Orthogonalize edges in place
			for ( size_t j = 0; j < objects[i].sideEdges.size(); j++ )
			{
				//int height = abs( objects[i].sideEdges[j][3] - objects[i].sideEdges[j][1] );

				objects[i].sideEdges[j] = orthogonalizeEdge( objects[i].sideEdges[j] );
			}

			objects[i].sideEdges.push_back( objects[i].sideEdges[1] );

			int centerHeight = abs( objects[i].sideEdges[center][3] - objects[i].sideEdges[center][1] );
			int compareHeight = abs( objects[i].sideEdges[left][3] - objects[i].sideEdges[left][1] );

//			cout << centerHeight << " " << compareHeight << endl;

			double z_dif = max( ( centerHeight - compareHeight ), 0 ) * imgWidth * FOCAL_LENGTH / double( centerHeight * APERTURE_WIDTH );
			int y_dif = max( ( centerHeight - compareHeight ), 0 ); //round( z_dif * centerHeight / double( imgWidth ) * ( APERTURE_WIDTH / FOCAL_LENGTH ) );

			int x_dif = round( z_dif * abs( objects[i].sideEdges[left][0] - imgWidth / 2 ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif ) );

//			cout << "Edge position change: z, x, y: " << z_dif << " " << x_dif << " " << y_dif << endl;

			objects[i].sideEdges[left] = orthogonalizePosition( objects[i].sideEdges[left], x_dif, y_dif );

			centerHeight = abs( objects[i].sideEdges[center][0] - objects[i].sideEdges[left][0] );
			compareHeight = abs( objects[i].corners[0][0] - objects[i].corners[1][0] );

//			cout << centerHeight << " " << compareHeight << endl;

			z_dif = max( ( centerHeight - compareHeight ), 0 ) * imgWidth * FOCAL_LENGTH / double( centerHeight * APERTURE_WIDTH );
			x_dif = max( ( centerHeight - compareHeight ), 0 ); //round( z_dif * centerHeight / double( imgWidth ) * ( APERTURE_WIDTH / FOCAL_LENGTH ) );

			y_dif = round( z_dif * abs( objects[i].corners[center][0] - imgHeight / 2 ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif ) );

//			cout << "Edge position change: z, x, y: " << z_dif << " " << x_dif << " " << y_dif << endl;

			Vec4i tempTopEdge;
			tempTopEdge[0] = objects[i].corners[0][0];
			tempTopEdge[1] = objects[i].corners[0][1];
			tempTopEdge[2] = objects[i].corners[1][0];
			tempTopEdge[3] = objects[i].corners[1][1];

			tempTopEdge = orthogonalizePosition( tempTopEdge, x_dif, y_dif );

			objects[i].corners[0][0] = tempTopEdge[0];
			objects[i].corners[0][1] = tempTopEdge[1];
			objects[i].corners[1][0] = tempTopEdge[2];
			objects[i].corners[1][1] = tempTopEdge[3];

			Vec4i centerEdge = objects[i].sideEdges[center];
			Vec4i otherEdge = objects[i].sideEdges[left];

			Vec4i otherOtherEdge;
			otherOtherEdge[0] = objects[i].corners[center][0];
			otherOtherEdge[1] = objects[i].corners[center][1];
			otherOtherEdge[2] = objects[i].corners[center][0];
			otherOtherEdge[3] = objects[i].corners[center][1] + abs( objects[i].sideEdges[center][3] - objects[i].sideEdges[center][1] );

			objects[i].sideEdges.clear();

			if ( otherEdge[0] < otherOtherEdge[0] )
			{
				objects[i].sideEdges.push_back( otherEdge );
				objects[i].sideEdges.push_back( centerEdge );
				objects[i].sideEdges.push_back( otherOtherEdge );
			}
			else
			{
				objects[i].sideEdges.push_back( otherOtherEdge );
				objects[i].sideEdges.push_back( centerEdge );
				objects[i].sideEdges.push_back( otherEdge );
			}

//			cout << objects[i].sideEdges[0][0] << " " << objects[i].sideEdges[0][1] << " " << objects[i].sideEdges[0][2] << " " <<objects[i].sideEdges[0][3] << endl;
//			cout << objects[i].sideEdges[1][0] << " " << objects[i].sideEdges[1][1] << " " << objects[i].sideEdges[1][2] << " " <<objects[i].sideEdges[1][3] << endl;
//			cout << objects[i].sideEdges[2][0] << " " << objects[i].sideEdges[2][1] << " " << objects[i].sideEdges[2][2] << " " <<objects[i].sideEdges[2][3] << endl;
		}

		if ( objects[i].sideEdges[1][1] > lowestPoint )
		{
			frontObjectIndex = i;
			lowestPoint = max( objects[i].sideEdges[1][3], objects[i].sideEdges[1][1] );
		}
	}

	float leftHeight = abs( min( objects[frontObjectIndex].sideEdges[0][3], objects[frontObjectIndex].sideEdges[0][1] ) - min( objects[frontObjectIndex].sideEdges[1][3], objects[frontObjectIndex].sideEdges[1][1] ) );
	float rightHeight = abs( min( objects[frontObjectIndex].sideEdges[2][3], objects[frontObjectIndex].sideEdges[2][1] ) - min( objects[frontObjectIndex].sideEdges[1][3], objects[frontObjectIndex].sideEdges[1][1] ) );
	float leftWidth = abs( objects[frontObjectIndex].sideEdges[1][0] - objects[frontObjectIndex].sideEdges[0][0] );
	float rightWidth = abs( objects[frontObjectIndex].sideEdges[2][0] - objects[frontObjectIndex].sideEdges[1][0] );

//	( pow( leftHeight, 2 ) + pow( rightHeight, 2 ) =

	float fullTriangleHeight = ( pow( abs( leftHeight - rightHeight ), 2 ) + pow( leftWidth + rightWidth, 2 ) ) - pow( rightWidth, 2 ) - pow( leftWidth, 2 );

	float factor = sqrt( fullTriangleHeight / (  pow( leftHeight, 2 ) + pow( rightHeight, 2 ) ) );

	ANGLE_OF_INCLINATION = atan( 1 / factor ) * 180 / CV_PI;

//	cout << "Angle of inclination: " << ANGLE_OF_INCLINATION << endl;

	// Relative orthogonalization
	for ( size_t i = 0; i < objects.size(); i++ )
	{
//		cout << "i: " << i << " frontObjectIndex: " << frontObjectIndex << " lowestPoint: " << lowestPoint << endl;

		if ( i == frontObjectIndex )
		{
			continue;
		}

		double z_dif;

		if ( ANGLE_OF_INCLINATION > 0 )
			z_dif = round( lowestPoint - max( objects[i].sideEdges[1][3], objects[i].sideEdges[1][1] ) ) / ( tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );
		else
			z_dif = 0;

//		cout << z_dif << endl;

		int xMiddle = imgWidth / 2;
		int yMiddle = imgHeight / 2;

		double a = z_dif * abs( max( objects[i].sideEdges[1][3], objects[i].sideEdges[1][1] ) - yMiddle ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif );
		double b = abs( lowestPoint - max( objects[i].sideEdges[1][3], objects[i].sideEdges[1][1] ) ) - ( z_dif * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );

		while ( abs( a - b ) > 0.5 )
		{
			a = z_dif * abs( max( objects[i].sideEdges[1][3], objects[i].sideEdges[1][1] ) - yMiddle ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif );
			b = abs( lowestPoint - max( objects[i].sideEdges[1][3], objects[i].sideEdges[1][1] ) ) - ( z_dif * tan( ANGLE_OF_INCLINATION * CV_PI / 180 ) );

			if ( a > b )
				z_dif = z_dif - 1;
			else
				z_dif = z_dif + 1;

//			cout << a << " " << b << endl;
		}

//		cout << z_dif << endl;

		for ( size_t j = 0; j < objects[i].sideEdges.size(); j++ )
		{
			int x_dif = round( z_dif * abs( objects[i].sideEdges[j][0] - xMiddle ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif ) );
			int y_dif = round( z_dif * abs( objects[i].sideEdges[j][3] - objects[i].sideEdges[j][1] ) / double( imgWidth * ( FOCAL_LENGTH / APERTURE_WIDTH ) - z_dif ) );

//			cout << x_dif << " " << y_dif << endl;

			objects[i].sideEdges[j] = orthogonalizePosition( objects[i].sideEdges[j], x_dif, y_dif );
		}
	}

	vector<vector<Vec4i> > orthogonalizedObjects;

	for ( size_t i = 0; i < objects.size(); i++ )
	{
		vector<Vec4i> orthogonalizedObject;

		if ( objects[i].sideEdges.size() == 3 )
		{
			orthogonalizedObject.push_back( objects[i].sideEdges[0] );

			int topLeft = min( objects[i].sideEdges[0][1], objects[i].sideEdges[0][3] );
			int topMiddle = min( objects[i].sideEdges[1][1], objects[i].sideEdges[1][3] );
			int topRight = min( objects[i].sideEdges[2][1], objects[i].sideEdges[2][3] );

			int bottomLeft = max( objects[i].sideEdges[0][1], objects[i].sideEdges[0][3] );
			int bottomMiddle = max( objects[i].sideEdges[1][1], objects[i].sideEdges[1][3] );
			int bottomRight = max( objects[i].sideEdges[2][1], objects[i].sideEdges[2][3] );

			Vec4i topLeftEdge;

			topLeftEdge[0] = objects[i].sideEdges[0][0];
			topLeftEdge[1] = topLeft;
			topLeftEdge[2] = objects[i].sideEdges[0][0] + abs( objects[i].sideEdges[2][0] - objects[i].sideEdges[1][0] );
			topLeftEdge[3] = topLeft - abs( topRight - topMiddle );

			orthogonalizedObject.push_back( topLeftEdge );

			Vec4i middleLeftEdge;

			middleLeftEdge[0] = objects[i].sideEdges[0][0];
			middleLeftEdge[1] = topLeft;
			middleLeftEdge[2] = objects[i].sideEdges[0][0] + abs( objects[i].sideEdges[1][0] - objects[i].sideEdges[0][0] );
			middleLeftEdge[3] = topLeft + abs( topLeft - topMiddle );

			orthogonalizedObject.push_back( middleLeftEdge );

			Vec4i bottomLeftEdge;

			bottomLeftEdge[0] = objects[i].sideEdges[0][0];
			bottomLeftEdge[1] = bottomLeft;
			bottomLeftEdge[2] = objects[i].sideEdges[0][0] + abs( objects[i].sideEdges[1][0] - objects[i].sideEdges[0][0] );
			bottomLeftEdge[3] = bottomLeft + abs( bottomLeft - bottomMiddle );

			orthogonalizedObject.push_back( bottomLeftEdge );

			orthogonalizedObject.push_back( objects[i].sideEdges[1] );

			Vec4i topRightEdge;

			topRightEdge[0] = objects[i].sideEdges[2][0] - abs( objects[i].sideEdges[1][0] - objects[i].sideEdges[0][0] );
			topRightEdge[1] = topRight - abs( topLeft - topMiddle );
			topRightEdge[2] = objects[i].sideEdges[2][0];
			topRightEdge[3] = topRight;

			orthogonalizedObject.push_back( topRightEdge );

			Vec4i middleRightEdge;

			middleRightEdge[0] = objects[i].sideEdges[1][0];
			middleRightEdge[1] = topMiddle;
			middleRightEdge[2] = objects[i].sideEdges[1][0] + abs( objects[i].sideEdges[1][0] - objects[i].sideEdges[2][0] );
			middleRightEdge[3] = topMiddle - abs( topRight - topMiddle );

			orthogonalizedObject.push_back( middleRightEdge );

			Vec4i bottomRightEdge;

			bottomRightEdge[0] = objects[i].sideEdges[1][0];
			bottomRightEdge[1] = bottomMiddle;
			bottomRightEdge[2] = objects[i].sideEdges[1][0] + abs( objects[i].sideEdges[1][0] - objects[i].sideEdges[2][0] );
			bottomRightEdge[3] = bottomMiddle - abs( bottomRight - bottomMiddle );

			orthogonalizedObject.push_back( bottomRightEdge );

			orthogonalizedObject.push_back( objects[i].sideEdges[2] );
		}

		orthogonalizedObjects.push_back( orthogonalizedObject );
	}

//	for ( size_t i = 0; i < objects[test].sideEdges.size(); i++ )
//		cout << objects[test].sideEdges[i][0] << endl;

	return orthogonalizedObjects;
	//return sideEdges;
}

