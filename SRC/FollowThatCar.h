#ifndef FOLLOW_THAT_CAR

/********************************************************************************/
/* FILE NAME:   FollowThatCar.h			                                        */
/* CREATED BY:  Noam Rones & Ori Peretz                                         */
/* FILE CREATION DATE: 03/10/13                                                 */
/*==============================================================================*/
/********************************************************************************/

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Tserial.h"
#include <iostream>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

HANDLE hDevice;
int threshold = 30;
const char* wndname = "Square Detection Demo";


/********************
	Declerations:
*********************/

void InitPort();

// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 );

// the function draws all the squares in the image
static void drawSquares( Mat& image, const vector<vector<Point> >& squares );

//retrieve the edges in order to find the height and base of the rectangle
double *getEdges(vector<Point> approx);

//tests whether the square has simillar proportions to the wanted LCP
bool TestLCPRatio(vector<Point> approx);

//searches for a LCP match in a single frame
void FrameProcessing(Mat frame);

// detects licence plates contours
void DetectLCP(vector<vector<Point> > contours);

//checks for LCP location and according send data to move the car
void CheckLCPLocation(vector<Point> approx);


#define FOLLOW_THAT_CAR