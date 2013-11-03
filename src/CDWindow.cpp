//
//  CDWindow.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDWindow.h"
#include <math.h>

using namespace std;

static const string CANDIDE_FILE_PATH = "candide3.wfm";

CDWindow::CDWindow(int w, int h, const char* label )
: Fl_Window(w, h, label)
{
	
	CDFaceData faceData( CANDIDE_FILE_PATH );
	
	faceWindow = new CDFaceWindow(10,10,100,200, faceData);
	
	
	resizable(faceWindow);
}

CDWindow::~CDWindow()
{
	delete faceWindow;
}

void CDWindow::resize(int x, int y, int w, int h)
{
	Fl_Window::resize(x,y,w,h);

	// resize the face window
	faceWindow->position(w/2-faceWindow->w()/2, h/2-faceWindow->h()/2);
}
