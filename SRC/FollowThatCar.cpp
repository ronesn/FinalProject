/********************************************************************************/
/* FILE NAME:   FollowThatCar.cpp		                                        */
/* CREATED BY:  Noam Rones & Ori Peretz                                         */
/* FILE CREATION DATE: 03/10/13                                                 */
/*==============================================================================*/
/********************************************************************************/

#include "FollowThatCar.h"

/********************************************************************************
* Function:  void InitPort()													*
*																				*
* Input: none																	*
*																				*
* Output: none																	*
*																				*
* Overview: Initialize the connection with the micro-processor via port			*
*																				*
********************************************************************************/
void InitPort()
{
	hDevice = CreateFile(L"COM5",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		cout<<"ERROR: Open COM5 fail"<<endl;
		CloseHandle(hDevice);
		
		return -1;
	}
	DCB lpTest;
	GetCommState(hDevice,&lpTest);
	lpTest.BaudRate = CBR_9600;
	lpTest.ByteSize = 8;
	lpTest.Parity = NOPARITY;
	lpTest.StopBits = ONESTOPBIT;
	SetCommState(hDevice,&lpTest);

}

/********************************************************************************
* Function:  static double angle( Point pt1, Point pt2, Point pt0 )				*
*																				*
* Input: pt1, pt2, pt3 - three points that represents an angle					*
*																				*
* Output: returns the angle between the three points							*
*																				*
* Overview: calculates an angle between three points							*
*																				*
********************************************************************************/
static double angle( Point pt1, Point pt2, Point pt0 )
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/********************************************************************************
*Function:void drawSquares( Mat& image, const vector<vector<Point> >& squares ) *
*																				*
* Input: image, squares - an array of rectangles								*
*																				*
* Output: none																	*
*																				*
* Overview: draws rectangles on an image according to given points  			*
*																				*
********************************************************************************/
static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
	for( size_t i = 0; i < squares.size(); i++ )
	{
		const Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		polylines(image, &p, &n, 1, true, Scalar(255,255,0), 3, CV_AA);
	}

	imshow(wndname, image);
}

/********************************************************************************
* Function:  getEdges(vector<Point> approx)										*
*																				*
* Input: approx - vector of points that represents a rectangle					*
*																				*
* Output: length of two edges													*
*																				*
* Overview: calculates the base and height of a given rectangle					*
*																				*
********************************************************************************/
double *getEdges(vector<Point> approx)
{
	//array of 2 edges(holds their lengths)
	double edges[2];

	edges[0] = sqrt(pow(approx[0].y-approx[1].y,2)+pow(approx[0].x-approx[1].x,2));
	edges[1] = sqrt(pow(approx[1].y-approx[2].y,2)+pow(approx[1].x-approx[2].x,2));

	return edges;
}

/********************************************************************************
* Function:  TestLCPRatio(vector<Point> approx)									*
*																				*
* Input: approx - vector of points that represents a rectangle					*
*																				*
* Output: boolean answer 														*
*																				*
* Overview: tests whether a give rectangle has the ration of the LCP			*
*																				*
********************************************************************************/
bool TestLCPRatio(vector<Point> approx)
{
	double* edges = getEdges(approx);
	double ratio;
	if(edges[0] > edges[1])
	{
		ratio = edges[0]/edges[1];
	}
	else
	{
		ratio = edges[1]/edges[0];
	}	
	
	//the average LCP ratio is supposed to be around 3.5
	if((ratio>4)||(ratio<3))
	{
		return false;
	}
	
	return true;
}

/********************************************************************************
* Function:  FrameProcessing(Mat frame)											*
*																				*
* Input: frame																	*
*																				*
* Output: none																	*
*																				*
* Overview: displays the edges of the frame by Canny algorithm					*
*			and sends it forward for LCP detection								*
********************************************************************************/
void FrameProcessing(Mat frame){
		
		vector<Vec4i> hierarchy;
		vector<vector<Point> > contours;
		Mat canny_output, edges;
		
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);

		/// Detect edges using canny
		Canny( edges, canny_output, threshold, threshold*2.5, 3 );
		dilate(edges, edges, Mat(), Point(-1,-1));
		imshow("edges", canny_output);

		/// Find contours
		findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		

		DetectLCP(contours);

}

/********************************************************************************
* Function:  DetectLCP(vector<vector<Point> > contours)							*
*																				*
* Input: a vector of closed shapes												*
*																				*
* Output: none																	*
*																				*
* Overview: searches for a LCP in a given array of shapes						*
*																				*
********************************************************************************/
 void DetectLCP(vector<vector<Point> > contours){
		
		vector<Point> approx;
		
		// test each contour
		for( size_t i = 0; i < contours.size(); i++ )
		{
			// approximate contour with accuracy proportional
			// to the contour perimeter
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

			// square contours should have 4 vertices after approximation
			// relatively large area (to filter out noisy contours)
			// and be convex.
			// Note: absolute value of an area is used because
			// area may be positive or negative - in accordance with the
			// contour orientation
			if( approx.size() == 4 && fabs(contourArea(Mat(approx))) > 500 && isContourConvex(Mat(approx)) )
			{
				double maxCosine = 0;

				// if cosines of all angles are small
				// (all angles are ~90 degree) then write quandrange
				// vertices to resultant sequence
				for( int j = 2; j < 5; j++ )
				{
					// find the maximum cosine of the angle between joint edges
					double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
					maxCosine = MAX(maxCosine, cosine);
				}
				
				//if the ratio is suitable continue to check the location of the LCP
				if( (TestLCPRatio(approx))&& ( maxCosine < 0.5 )){
					CheckLCPLocation(approx);
				}
			}
		}
 }

 /********************************************************************************
* Function:  CheckLCPLocation(vector<Point> approx)								*
*																				*
* Input: approx - vector of points that represents a rectangle					*
*																				*
* Output: none																	*
*																				*
* Overview: detects for the exact location of the given LCP and writes to the	*
*			port the needed action accordance to the location					*
********************************************************************************/
 void CheckLCPLocation(vector<Point> approx){
 
	DWORD btsIO;
	
	// get the (absolute) squre area
	double area = fabs(contourArea(Mat(approx)));
	
	if(area > 5000)
	{
		char test[] = "4";
		WriteFile(hDevice,test,strlen(test),&btsIO,NULL);
		cout << " Too close, stop! " << test << endl;
	}
	else
	{	
		double xAxisLocation=(approx[3].x+approx[0].x)/2;
		
		//frame width is 640px
		//therefor the center is located at ~320
		//if the LCP location is above 335 i trun right
		if(xAxisLocation > 335)
		{
			char test[] = "1";
			WriteFile(hDevice,test,strlen(test),&btsIO,NULL);
			cout << "Turn right " << test << endl;
		}
		else
		{
			//if the LCP location is beneath 305 i turn left
			if(xAxisLocation< 305)
			{
				char test2[] = "2";
				WriteFile(hDevice,test2,strlen(test2),&btsIO,NULL);
				cout << "Turn left " << test2 << endl;
			}
			
			//the LCP is at the center i continue straight
			else
			{
				char test2[] = "3";
				WriteFile(hDevice,test2,strlen(test2),&btsIO,NULL);
				cout << "Centered " << test2 << endl;
			}
		}
		
	}
 }