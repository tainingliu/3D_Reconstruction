/*
 * orthogonalize.h
 *
 *  Created on: 2012-10-21
 *      Author: Tai
 */

#ifndef ORTHOGONALIZE_H_
#define ORTHOGONALIZE_H_

#include "opencv2/opencv.hpp"
#include "stdio.h"
#include <iostream>

using namespace cv;
using namespace std;

vector<vector<Vec4i> > orthogonalize( vector<Vec4i> edgeMap );

#endif /* ORTHOGONALIZE_H_ */
