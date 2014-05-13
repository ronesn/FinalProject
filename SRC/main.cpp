/********************************************************************************/
/* FILE NAME:   main.cpp				                                        */
/* CREATED BY:  Noam Rones & Ori Peretz                                         */
/* FILE CREATION DATE: 03/10/13                                                 */
/*==============================================================================*/
/*                                                                              */
/* This program recieves a live video feed searches for a Licence Plate         */
/* in case a LCP was detected the program sends a command through the port 	    */
/* in order to track it															*/
/* 							                                                    */
/********************************************************************************/

#include "FollowThatCar.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Mat frame;
	const char* wndname = "Square Detection Demo";
	InitPort();
	VideoCapture cap(1); // open the default camera
	if(!cap.isOpened())  // check if we succeeded
		return -1;

	namedWindow("edges",1);
	namedWindow( wndname, 1 );
	
	for(;;)
	{
		vector<vector<Point> > squares;
		cap >> frame; // get a new frame from camera
		FrameProcessing(frame);
		drawSquares(frame, squares);
		if(waitKey(30) >= 0) break;
	}



	CloseHandle(hDevice);

	return 0;
}
